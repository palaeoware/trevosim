#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "organism.h"
#include "mainwindow.h"

#include <QVector>
#include <QRandomGenerator>

class Environment
{
public:
    Environment(const simulationVariables &simSettingsCon);
    Environment(const int &maskNumber, const int &maskLength, const bool initialiseState); //used for tests of fitness algorithm
    Environment(Environment const &constructorEnvironment, bool matchingPeaksCon);

    void operator = (const Environment &E);
    bool operator == (const Environment &E);
    bool operator != (const Environment &E);

    bool mutate();

    bool error = false;

    QString printMasks();
    quint32 bitCount(Organism const *o) const;
    bool addMask();
    void overwriteMask(Organism const *o);
    void setMasksToZero();
    void setMasksToOne();
    bool compareOrganism(Organism const *o);
    int countDifferences(Environment const &externalEnvironment);
    int bitCount();
    void setUpPerturbation(int startIteration, int endIteration);
    void applyPerturbation(int currentIteration);

private:
    QVector <QVector <bool> > masks;
    double mutationRate;
    //We have different types of environment - set this on creation
    int environmentType;

    //We need a few data structures if we're doing a perturbation - I could in theory make this inherit environment. Not sure it's worth it for the current setup
    //One vector to keep a copy of masks, the other to keep a copy of which elements of the original masks have been copied over the perturbation masks
    QVector <QVector <bool> > environmentalPerturbationMasksCopy;
    QVector <QVector <bool> > environmentalPerturbationOverwriting;
    int perturbationStart, perturbationEnd;
    int environmentalPerturbationCopyRate;
    double environmentMutationJump, environmentMutationMaxJump;
};

#endif // ENVIRONMENT_H
