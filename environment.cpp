#include "environment.h"

#include <QRandomGenerator>

// We call this constructor when we want to create a new, random environment
Environment::Environment(int maskNumber, int maskLength)
{
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

//We call this one when we want to do one with matching peaks
Environment::Environment(Environment &constructorEnvironment)
{

}
