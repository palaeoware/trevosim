#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QVector>

class Environment
{
public:
    Environment(int maskNumber, int maskLength);
    Environment(Environment &constructorEnvironment);

private:
    QVector <QVector <bool> > masks;

    //This doesn't need the playing field structure - can just spit back mask strings
    //QString printMasks(const QVector <playingFieldStructure *> &playingFields, int playingfield);
    //QString printMasks(const QVector <playingFieldStructure *> &playingFields);
};

#endif // ENVIRONMENT_H
