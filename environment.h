#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QVector>

class Environment
{
public:
    Environment(const int &maskNumber, const int &maskLength, const bool matchingPeaksCon);
    Environment(Environment const &constructorEnvironment);

    void operator = (const Environment &E);

private:
    QVector <QVector <bool> > masks;
    bool matchingPeaks;

    //This doesn't need the playing field structure - can just spit back mask strings
    //QString printMasks(const QVector <playingFieldStructure *> &playingFields, int playingfield);
    //QString printMasks(const QVector <playingFieldStructure *> &playingFields);
};

#endif // ENVIRONMENT_H
