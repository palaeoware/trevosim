#ifndef ORGANISM_H
#define ORGANISM_H

#include "mainwindow.h"
#include <QVector>

class organism
{
public:
    organism(int genome_size);

    //Genome of species and progenator
    // Do as bools in order to make genome size easily scaleable - speed not much of an issue here
    //resize from method if needed

    QVector <bool> genome;
    QVector <bool> parent_genome;

    // ID of species and progenator
    int species_id;
    int parent_species_id;

    int fitness;

    int born;
    int extinct;
    //Need to record this for terminal branch lengths - for any terminal, branch length is iterations, minus the point of last cladogenesis.
    int cladogenesis;
    //To write tree recursively
    QList<int> children;

    void operator = (const organism &O);
    bool operator < (const organism &O);

    void initialise (int genome_size);
};

#endif // ORGANISM_H
