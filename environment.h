#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "organism.h"
#include "mainwindow.h"

#include <QVector>
#include <QRandomGenerator>

class Environment
{
public:
    Environment(const int &maskNumber, const int &maskLength, const double mutationRateCon, const int environmentTypeCon);
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

private:
    QVector <QVector <bool> > masks;
    double mutationRate;
    int environmentType;
};

#endif // ENVIRONMENT_H
