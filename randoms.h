#ifndef RANDOMS_H
#define RANDOMS_H

#include "mainwindow.h"
#include <QTime>
#include <QFile>
#include <QMessageBox>

class randoms
{
public:
    randoms();
    ~randoms();
    quint16 gen_random();
private:
    quint16 random_array[65536];
    quint16 nextrandom;
    void load_randoms();
};

// This is key to making simulation randoms accessible from organism.cpp
extern randoms *simulation_randoms;

#endif // RANDOMS_H
