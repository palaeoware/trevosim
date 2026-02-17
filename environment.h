#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QVector>
#include <QRandomGenerator>

class Environment
{
public:
    Environment(const int &maskNumber, const int &maskLength, const bool matchingPeaksCon, const double mutationRateCon);
    Environment(Environment const &constructorEnvironment);

    void operator = (const Environment &E);
    void mutate();

private:
    QVector <QVector <bool> > masks;
    bool matchingPeaks;
    bool error = false;
    double mutationRate;
    QRandomGenerator randoms;

    //This doesn't need the playing field structure - can just spit back mask strings
    //QString printMasks(const QVector <playingFieldStructure *> &playingFields, int playingfield);
    //QString printMasks(const QVector <playingFieldStructure *> &playingFields);
};

#endif // ENVIRONMENT_H
