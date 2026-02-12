#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QVector>

class Environment
{
public:
    Environment();
    QVector <QVector <bool> > masks;
};

#endif // ENVIRONMENT_H
