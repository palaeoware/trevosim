#include "environment.h"

#include <algorithm>
#include <QDebug>

//To do:
//deal with error bool if true when called from simulation - add relevant message:
//Constructor - initialisation
//Mutate - warning("Oops", "There has been an error at mutating the environment with matching peaks - not enough pairs. Returning with no mutations made.");
//Perturbations
//Also move mutate organism to organism object - why is this in simulation it makes no sense?
//check when done that all attributes are correctly copied in equals
//Failing tests 0,1,2,5,17
//Environment types - can't do matching peaks and random environments. Either make exclusive, or make matching peaks a type of environment? Also make perturbation and environemnt type

// We call this constructor when we want to create a new environment from scratch
Environment::Environment(const int &maskNumber, const int &maskLength, const bool matchingPeaksCon, const double mutationRateCon, const int environmentTypeCon = ENVIRONMENT_TYPE_CONSTANT)
{
    matchingPeaks = matchingPeaksCon;
    mutationRate = mutationRateCon;
    environmentType = environmentTypeCon;

    //Set up vectors that will serve as masks for this environment
    for (int j = 0; j < maskNumber; j++)
    {
        masks.append(QVector <bool>());
        for (int i = 0; i < maskLength; i++)
        {
            //This will generate a random integer between 0 (inclusive) and 2 (exclusive) - so either 0 or 1. Break it out for clarity
            int random = QRandomGenerator::global()->bounded(0, 2);
            if (random == 0) masks[j].append(bool(false));
            else if (random == 1) masks[j].append(bool(true));
            else error = true; //This should never happen
        }
    }
}

//We call this when we want to initialise to a known state to test the fitness algorithm
Environment::Environment(const int &maskNumber, const int &maskLength, const bool initialiseState)
{
    //Set up vectors that will serve as masks for this environment
    for (int j = 0; j < maskNumber; j++)
    {
        masks.append(QVector <bool>());
        for (int i = 0; i < maskLength; i++)masks[j].append(bool(initialiseState));
    }
}

//We call this one when we want to create an environment from another - either shuffled but with matching peaks, or just copying the first
Environment::Environment(const Environment &constructorEnvironment,  bool matchingPeaksCon)
{
    if (matchingPeaksCon)
    {
        matchingPeaks = true;
        mutationRate = constructorEnvironment.mutationRate;
        environmentType = constructorEnvironment.environmentType;

        //If we need to make sure fitness peaks are the same height, in TREvoSim, we need to initialise with the same number of 1s in each site
        //An easy way to do this is to shuffle the columns/sites between the incoming environment and the one we are creating

        //First let's create an iota vector and shuffle it, using this to change the sites (i.e. bits)
        QVector<int> sites(constructorEnvironment.masks[0].length());
        std::iota(sites.begin(), sites.end(), 0);
        std::shuffle(sites.begin(), sites.end(), *QRandomGenerator::global());

        //Use this to write the new environment based on the incoming one
        for (int j = 0; j < constructorEnvironment.masks.length(); j++)
        {
            masks.append(QVector <bool>());
            for (int i = 0; i < sites.length(); i++)
            {
                //Here we use the ith entry in the shuffled sites list to define the site we copy over from the incoming mask
                if (constructorEnvironment.masks[j][sites[i]]) masks[j].append(bool(true));
                else  masks[j].append(bool(false));
            }
        }
    }
    else
    {
        matchingPeaks = constructorEnvironment.matchingPeaks;
        mutationRate = constructorEnvironment.mutationRate;
        environmentType = constructorEnvironment.environmentType;

        for (int j = 0; j < constructorEnvironment.masks.length(); j++)
        {
            masks.append(QVector <bool>());
            for (int i = 0; i < constructorEnvironment.masks[j].length(); i++)
            {
                //Here we use the ith entry in the shuffled sites list to define the site we copy over from the incoming mask
                if (constructorEnvironment.masks[j][i]) masks[j].append(bool(true));
                else  masks[j].append(bool(false));
            }
        }
    }
}

void Environment::operator = (const Environment &E)
{
    // Copy all attributes
    masks = E.masks;
    matchingPeaks = E.matchingPeaks;
    mutationRate = E.mutationRate;
}

bool Environment::operator == (const Environment &E)
{
    if (masks != E.masks) return false;
    if (matchingPeaks != E.matchingPeaks) return false;
    if (mutationRate != E.mutationRate) return false;
    return true;
}

bool Environment::operator != (const Environment &E)
{
    if (masks == E.masks) return false;
    if (matchingPeaks == E.matchingPeaks) return false;
    if (mutationRate == E.mutationRate) return false;
    return true;
}


bool Environment::mutate()
{
    //Set our mutation rate
    double localMutationRate = mutationRate;
    //If we are matching peaks, we want the mutation rate to be halved because we will need to switch a zero to a one and one to a zero or vice versa.
    //So every mutation is two bit changes
    if (matchingPeaks) localMutationRate /= 2;

    //Define the mask length as we will need to use it multiple times
    int maskLength = masks[0].length();
    int maskNumber = masks.length();

    //As per docs, mutations are set per 100 bits in the genome - calculate for this environment, first total bit number
    int totalBitsPerEnvironment = masks[0].length() * masks.length();
    //Then calculate mutation # for this environment
    double numberEnvironmentMutationsDouble = (static_cast<double>(totalBitsPerEnvironment) / 100.) * localMutationRate;
    //This will be used to store the integral part of the above - it needs to be a double as this is what is passed to the modf function
    double numberEnvironmentMutationsIntegral = numberEnvironmentMutationsDouble;
    //Next sort out the probabilities of extra mutation given remainder
    double numberEnvironmentMutationsFractional = modf(numberEnvironmentMutationsDouble, &numberEnvironmentMutationsIntegral);
    int numberEnvironmentMutationsInteger = (static_cast<int>(numberEnvironmentMutationsIntegral));
    if (QRandomGenerator::global()->generateDouble() < numberEnvironmentMutationsFractional) numberEnvironmentMutationsInteger++;
    //note that due to saturation / multiple hits on one site, the number of recorded mutations in e.g. our tests may sneak in under the expected value

    if (matchingPeaks)
    {
        //If we are matching peaks, it's best to think of bit positions as columns. We want to shuffle around columns in the 'x' direction to achieve on swapped bit (= two bit changes, hence the half rate above - it is impossible to do this without swapping two bits)
        //The way this is organised, we want to do this within each environment
        //reminder: masks[environment #][mask #][bit #]

        //Create lists of columns separated by one bit to do swap
        QList <int> pairOne;
        QList <int> pairTwo;

        //Used to do the exhaustively, but this was massive overkill for most settings, and made the function slooooow
        //Now use heuristic approach with an appopriate escape and error message
        int count = 0;
        do
        {
            int firstBit = QRandomGenerator::global()->bounded(maskLength);
            int secondBit = QRandomGenerator::global()->bounded(maskLength);
            if (firstBit == secondBit) continue;

            int bitDifference = 0;
            for (int n = 0; n < maskNumber; n++)
                if (masks[n][firstBit] != masks[n][secondBit]) bitDifference++;

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
        if (pairOne.length() < numberEnvironmentMutationsInteger) return false;
        //Otherwisse apply the mutations
        else for (int x = 0; x < numberEnvironmentMutationsInteger; x++)
            {
                //Swap one pair of columns
                int swap1 = pairOne[x];
                int swap2 = pairTwo[x];
                for (int m = 0; m < maskNumber; m++)
                {
                    bool storeBit = masks[m][swap1];
                    masks[m][swap1] = masks[m][swap2];
                    masks[m][swap2] = storeBit;
                }
            }
    }
    //Or if we don't have matching peaks, we can just do the mutations
    else
    {
        for (int x = 0; x < numberEnvironmentMutationsInteger; x++)
        {
            //Select random mask and random bit
            int mutationPosition = QRandomGenerator::global()->bounded(maskLength);
            int mutationMask = QRandomGenerator::global()->bounded(maskNumber);
            //qDebug() << "m" << mutationPosition << mutationMask;
            masks[mutationMask ][mutationPosition] = !masks[mutationMask ][mutationPosition];
        }
    }
    return true;
}

QString Environment::printMasks()
{
    QString maskText;
    QTextStream out(&maskText);

    for (int maskNumber = 0; maskNumber < masks.length(); maskNumber++)
    {
        out << "Mask number " << maskNumber << " :\t";
        for (auto b : masks[maskNumber]) b ? out << 1 : out << 0 ;
        out << "\n";
    }

    return maskText;
}

//Count bits for fitness algorithm
quint32 Environment::bitCount(Organism const *o) const
{
    quint32 counts = 0;
    for (auto m : masks)
    {
        //Check length here
        for (int j = 0; j < m.length(); j++)
            if (o->genome[j] != m[j]) counts++;
    }
    return counts;
}

//Need to add mask in some flavours of EE
bool Environment::addMask()
{
    masks.append(QVector <bool>());
    int newMask = masks.length() - 1;

    for (int i = 0; i < masks[0].length(); i++)
    {
        //This will generate a random integer between 0 (inclusive) and 2 (exclusive) - so either 0 or 1. Break it out for clarity
        int random = QRandomGenerator::global()->bounded(0, 2);
        if (random == 0) masks[newMask].append(bool(false));
        else if (random == 1) masks[newMask].append(bool(true));
        else return false; //This should never happen
    }

    return true;
}

//For EE we need to overwrite a mask
void Environment::overwriteMask(Organism const *o)
{
    //EE works either by copying over genome to a prexisting mask - thus improving fitness of the EE species - or to the mask added
    //Either way, we can write over the last mask for each environment (-1 because indexing starts at zero)
    //We always do this on the last one since this was just added if we're adding a mask rather than just overwriting (makes no difference if not adding one)
    for (int i = 0; i < masks[masks.length() - 1].length(); i++) masks[masks.length() - 1][i] = o->genome[i];
}

bool Environment::compareOrganism(Organism const *o)
{
    for (auto &m : masks)
        if (o->genome == m) return true;

    return false;
}

int Environment::countDifferences(Environment const &externalEnvironment)
{
    int maskCount = masks.length();
    int bitCount = masks[0].length();
    if ((maskCount != externalEnvironment.masks.length()) || (bitCount != externalEnvironment.masks[0].length())) return -1;
    int count = 0;
    for (int m = 0; m < masks.length(); m++)
        for (int b = 0; b < masks[m].length(); b++)
            if (masks[m][b] != externalEnvironment.masks[m][b]) count++;
    return count;
}

int Environment::bitCount()
{
    int count = 0;
    for (auto m : masks)
        for (auto b : m)
            if (b) count++;

    return count;
}
