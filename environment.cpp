#include "environment.h"

#include <algorithm>
#include <QRandomGenerator>

// We call this constructor when we want to create a new, random environment
Environment::Environment(const int &maskNumber, const int &maskLength, const bool matchingPeaksCon)
{
    matchingPeaks = matchingPeaksCon;

    //Set up vectors that will serve as masks for this environment
    for (int j = 0; j < maskNumber; j++)
    {
        masks[j].append(QVector <bool>());
        for (int i = 0; i < maskLength; i++)
        {
            if (QRandomGenerator::global()->generate() > (QRandomGenerator::max() / 2)) masks[j].append(bool(false));
            else  masks[j].append(bool(true));
        }
    }
}

//We call this one when we want to create an environment with matching peaks - we can always do this off the first created environment
Environment::Environment(const Environment &constructorEnvironment)
{
    //We only call this when we are matching peaks
    matchingPeaks = true;

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
