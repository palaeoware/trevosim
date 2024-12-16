#ifndef ORGANISM_H
#define ORGANISM_H

#include <QList>

class Organism
{
public:
    Organism(int genomeSize, bool stochastic);
    //Explicit default copy constructor to avoid compile warnings
    Organism(const Organism &) = default;

    //Genome of species and ancestors
    //Do as vectors in order to make genome size easily scaleable - speed not much of an issue here
    QList <bool> genome; //Organisms genome
    QList <QList <bool> > parentGenomes;//Gnome of any parents, and also any other speciation from within this lineage
    QList <bool> stochasticGenome;//Used for many to one mapping of a genome

    // ID of species and parent
    int speciesID;
    int parentSpeciesID;

    int fitness;

    int born;
    int extinct;

    bool stochasticLayer;
    bool ecosystemEngineer;

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
    void setGenome(QList<bool> genome);
};

#endif // ORGANISM_H
