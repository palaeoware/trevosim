#ifndef ORGANISM_H
#define ORGANISM_H

#include <QVector>
#include <QList>

class Organism
{
public:
    Organism(int genomeSize);

    //Genome of species and progenator
    // Do as bools in order to make genome size easily scaleable - speed not much of an issue here
    //resize from method if needed

    QVector <bool> genome;
    QVector <bool> parentGenome;
    QVector <bool> stochasticGenome;

    // ID of species and progenator
    int speciesID;
    int parentSpeciesID;

    int fitness;

    int born;
    int extinct;

    bool stochasticLayer;

    //Need to record this for terminal branch lengths - for any terminal, branch length is iterations, minus the point of last cladogenesis.
    int cladogenesis;
    //To write tree recursively
    QList<int> children;

    void operator = (const Organism &O);
    bool operator == (const Organism &O);
    bool operator < (const Organism &O);

    void initialise (int genomeSize);
    void initialise (int genomeSize, const int *stochasticMap);
    void mapFromStochastic(const int *stochasticMap);
    void setGenome(QVector<bool> genome);
};

#endif // ORGANISM_H
