#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "organism.h"

#include <QVector>
#include <QRandomGenerator>

class Environment
{
public:
    Environment(const int &maskNumber, const int &maskLength, const bool matchingPeaksCon, const double mutationRateCon);
    Environment(const int &maskNumber, const int &maskLength, const bool initialiseState); //used for tests of fitness algorithm
    Environment(Environment const &constructorEnvironment);

    void operator = (const Environment &E);
    bool operator == (const Environment &E);
    bool operator != (const Environment &E);

    bool  mutate();

    bool error = false;

    QString printMasks();
    int bitCount(Organism const *o) const;
    bool addMask();
    void overwriteMask(Organism const *o);
    void setMasksToZero();
    void setMasksToOne();

private:
    QVector <QVector <bool> > masks;
    bool matchingPeaks;
    double mutationRate;
    QRandomGenerator randoms;
};

#endif // ENVIRONMENT_H
