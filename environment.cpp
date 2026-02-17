#include "environment.h"

#include <algorithm>
#include <QDebug>

//To do:
//deal with error bool if true when called from simulation

// We call this constructor when we want to create a new, random environment
Environment::Environment(const int &maskNumber, const int &maskLength, const bool matchingPeaksCon, const double mutationRateCon)
{
    matchingPeaks = matchingPeaksCon;
    mutationRate = mutationRateCon;
    randoms = QRandomGenerator::securelySeeded();

    //Set up vectors that will serve as masks for this environment
    for (int j = 0; j < maskNumber; j++)
    {
        masks[j].append(QVector <bool>());
        for (int i = 0; i < maskLength; i++)
        {
            //This will generate a random integer between 0 (inclusive) and 2 (exclusive) - so either 0 or 1. Break it out for clarity
            int random = randoms.bounded(0, 2);
            if (random == 0) masks[j].append(bool(false));
            else if (random == 1) masks[j].append(bool(true));
            else error = true;
        }
    }
}

//We call this one when we want to create an environment with matching peaks - we can always do this off the first created environment
Environment::Environment(const Environment &constructorEnvironment)
{
    //We only call this when we are matching peaks
    matchingPeaks = true;
    mutationRate = constructorEnvironment.mutationRate;
    randoms = QRandomGenerator::securelySeeded();

    //If we need to make sure fitness peaks are the same height, in TREvoSim, we need to initialise with the same number of 1s in each site
    //An easy way to do this is to shuffle the columns/sites between the incoming environment and the one we are creating

    //First let's create an iota vector and shuffle it, using this to change the sites (i.e. bits)
    QVector<int> sites(constructorEnvironment.masks[0].length());
    std::iota(sites.begin(), sites.end(), 0);
    std::shuffle(sites.begin(), sites.end(), QRandomGenerator::global());

    //Use this to write the new environment based on the incoming one
    for (int j = 0; j < constructorEnvironment.masks.length(); j++)
    {
        masks[j].append(QVector <bool>());
        for (int i = 0; i < sites.length(); i++)
        {
            //Here we use the ith entry in the shuffled sites list to define the site we copy over from the incoming mask
            if (constructorEnvironment.masks[j][sites[i]]) masks[j].append(bool(true));
            else  masks[j].append(bool(false));
        }
    }
}

void Environment::operator = (const Environment &E)
{
    // Copy all attributes
    masks = E.masks;
    matchingPeaks = E.matchingPeaks;
}

void Environment::mutate()
{
    //Set our mutation rate
    double localMutationRate = mutationRate;
    //If we are matching peaks, we want the mutation rate to be halved because we will need to switch a zero to a one and one to a zero or vice versa.
    //So every mutation is two bit changes
    if (matchingPeaks) localMutationRate /= 2;

    //As per docs, mutations are set per 100 bits in the genome - calculate for this environment, first total bit number
    int totalBitsPerEnvironment = masks[0].length() * masks.length();
    //Then calculate mutation # for this environment
    double numberEnvironmentMutationsDouble = (static_cast<double>(totalBitsPerEnvironment) / 100.) * localMutationRate;
    //This will be used to store the integral part of the above - it needs to be a double as this is what is passed to the modf function
    double numberEnvironmentMutationsIntegral = numberEnvironmentMutationsDouble;
    //Next sort out the probabilities of extra mutation given remainder
    double numberEnvironmentMutationsFractional = modf(numberEnvironmentMutationsDouble, &numberEnvironmentMutationsIntegral);
    int numberEnvironmentMutationsInteger = (static_cast<int>(numberEnvironmentMutationsIntegral));
    if (static_cast<double>(QRandomGenerator::global()->generate()) < (numberEnvironmentMutationsFractional * static_cast<double>(maxRand))) numberEnvironmentMutationsInteger++;
    //note that due to saturation / multiple hits on one site, the number of recorded mutations in e.g. our tests may sneak in under the expected value

    if (simSettings->matchFitnessPeaks)
    {
        //If we are matching peaks, it's best to think of bit positions as columns. We want to shuffle around columns in the 'x' direction to achieve on swapped bit (= two bit changes, hence the half rate above - it is impossible to do this without swapping two bits)
        //The way this is organised, we want to do this within each environment
        //reminder: masks[environment #][mask #][bit #]

        for (auto pf : std::as_const(playingFields)) // Treat playing fields separately
            for (int j = 0; j < runEnvironmentNumber; j++) //Treat environments separately
            {
                //Create lists of columns separated by one bit to do swap
                QList <int> pairOne;
                QList <int> pairTwo;

                //Used to do the exhaustively, but this was massive overkill for most settings, and made the function slooooow
                //Now use heuristic approach with an appopriate escape and error message
                int count = 0;
                do
                {
                    int firstBit = QRandomGenerator::global()->bounded(runFitnessSize);
                    int secondBit = QRandomGenerator::global()->bounded(runFitnessSize);
                    if (firstBit == secondBit) continue;

                    int bitDifference = 0;
                    for (int n = 0; n < runMaskNumber; n++)
                        if (pf->masks[j][n][firstBit] != pf->masks[j][n][secondBit]) bitDifference++;

                    if (bitDifference == 1)
                        if (!pairOne.contains(firstBit) && !pairOne.contains(secondBit) && !pairTwo.contains(firstBit) && !pairTwo.contains(secondBit))
                        {
                            pairOne.append(firstBit);
                            pairTwo.append(secondBit);
                        }
                    count++;
                }
                while (pairOne.length() < numberEnvironmentMutationsInteger && count < 10000);

                //Add warning if there are not enough columns to swap: with any decent size genome, I don't expect this to happen all that much
                if (pairOne.length() < numberEnvironmentMutationsInteger)
                {
                    warning("Oops", "There has been an error at mutating the environment with matching peaks - not enough pairs. Returning with no mutations made.");
                    return;
                }
                else
                {
                    //Now apply the mutations
                    for (int x = 0; x < numberEnvironmentMutationsInteger; x++) //How many mutations?
                    {
                        //Swap one pair of columns
                        int swap1 = pairOne[x];
                        int swap2 = pairTwo[x];
                        for (int m = 0; m < runMaskNumber; m++)
                        {
                            bool storeBit = pf->masks[j][m][swap1];
                            pf->masks[j][m][swap1] = pf->masks[j][m][swap2];
                            pf->masks[j][m][swap2] = storeBit;
                        }
                    }
                }
            }
    }
    else
    {
        for (auto pf : std::as_const(playingFields)) // Treat playing fields separately
            for (int j = 0; j < runEnvironmentNumber; j++) //Treat environments separately
                for (int x = 0; x < numberEnvironmentMutationsInteger; x++) //How many mutations?
                {
                    //Select random mask and random bit
                    int mutationPosition = QRandomGenerator::global()->bounded(runFitnessSize);
                    int maskNumber = QRandomGenerator::global()->bounded(runMaskNumber);
                    pf->masks[j][maskNumber][mutationPosition] = !pf->masks[j][maskNumber][mutationPosition];
                }
    }
}
