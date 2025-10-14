#include "organism.h"

#include <QRandomGenerator>

Organism::Organism(int genomeSize, bool stochastic)
{
    stochasticLayer = stochastic;

    // Initialise everything to false/-1 to make it obvious if it has not been changed.
    parentGenomes.append(QList <bool>());
    for (int i = 0; i < genomeSize; i++)
    {
        genome.append(false);
        parentGenomes[0].append(false);
    }

    if (stochasticLayer)
        for (int i = 0; i < genomeSize * 4; i++)
            stochasticGenome.append(false);

    ecosystemEngineer = false;

    speciesID = -1;
    parentSpeciesID = -1;

    fitness = -1;
}

void Organism::initialise(int genomeSize)
{
    // Initialise genome; this is the first organism; parent genome is initialised as identical to starting point (genome will evolve away from this)
    for (int i = 0; i < genomeSize; i++)
    {
        if (QRandomGenerator::global()->generate() > (QRandomGenerator::max() / 2))
        {
            genome[i] = true;
            parentGenomes[0][i] = true;
        }
        else
        {
            genome[i] = false;
            parentGenomes[0][i] = false;
        }
    }

    //This is true for first organism, which is the only time one is initialised - otherwise they are just copied.
    speciesID = 0;
    parentSpeciesID = 0;

    ecosystemEngineer = false;

    born = 0;
    extinct = 0;
    cladogenesis = 0;

    fitness = 0;
}

void Organism::initialise(int genomeSize, const int *stochasticMap)
{
    // Initialise genome, assume this is first organism, and prog genome same as genome
    for (int i = 0; i < genomeSize * 4; i++)
        if (QRandomGenerator::global()->generate() > (QRandomGenerator::max() / 2)) stochasticGenome[i] = true;
        else stochasticGenome[i] = false;
    this->mapFromStochastic(stochasticMap);

    for (int i = 0; i < genomeSize; i++) parentGenomes[0][i] = genome[i];

    //This is true for first organism. Which is pretty much only time I initialise one currently.
    speciesID = 0;
    parentSpeciesID = 0;

    ecosystemEngineer = false;

    born = 0;
    extinct = 0;
    cladogenesis = 0;

    fitness = 0;
}


void Organism::operator = (const Organism &O)
{
    // Copy all attributes
    genome = O.genome;
    parentGenomes = O.parentGenomes;
    stochasticLayer = O.stochasticLayer;
    if (stochasticLayer) stochasticGenome = O.stochasticGenome;
    speciesID = O.speciesID;
    parentSpeciesID = O.parentSpeciesID;
    fitness = O.fitness;
    born = O.born;
    extinct = O.extinct;
    cladogenesis = O.cladogenesis;
    ecosystemEngineer = O.ecosystemEngineer;
    children = O.children; // Note this is only used in the species list.
}

bool Organism::operator == (const Organism &O)
{
    if (genome != O.genome) return false;
    if (parentGenomes != O.parentGenomes) return false;
    if (stochasticLayer != O.stochasticLayer) return false;
    if (stochasticLayer && (stochasticGenome != O.stochasticGenome)) return false;
    if (speciesID != O.speciesID) return false;
    if (parentSpeciesID != O.parentSpeciesID) return false;
    if (fitness != O.fitness) return false;
    if (fitnessRecord != O.fitnessRecord) return false;
    if (born != O.born) return false;
    if (extinct != O.extinct) return false;
    if (cladogenesis != O.cladogenesis) return false;
    if (ecosystemEngineer != O.ecosystemEngineer) return false;
    if (children != O.children) return false;
    return true;
}


bool Organism::operator < (const Organism &O)
{
    return (fitness < O.fitness);
}

void Organism::mapFromStochastic(const int *stochasticMap)
{

    quint16 lookups[4];
    lookups[0] = 1;
    for (int i = 1; i < 4; i++)lookups[i] = lookups[i - 1] * 2;

    int genomePosition = 0;

    for (int i = 0; i < stochasticGenome.count(); i += 4)
    {
        quint16 map = 0;
        if (stochasticGenome[i]) map  = map | lookups[0];
        if (stochasticGenome[i + 1]) map  = map | lookups[1];
        if (stochasticGenome[i + 2]) map  = map | lookups[2];
        if (stochasticGenome[i + 3]) map  = map | lookups[3];

        genome[genomePosition] = stochasticMap[map];
        genomePosition ++;
    }

    //Debug code
    /*
    QString mapString;

    for (int i = 0; i < 16 ; i++)
        if (stochasticMap[i])mapString.append("1");
        else mapString.append("0");

    qDebug() << "Map is: " << mapString;

    QString genomeString;
    for (int i = 0; i < genome.count() ; i++)
        if (genome[i])genomeString.append("1");
        else genomeString.append("0");

    qDebug() << "Genome is: " << genomeString;

    genomeString.clear();

    for (int i = 0; i < stochasticGenome.count() ; i++)
    {
        if (stochasticGenome[i])genomeString.append("1");
        else genomeString.append("0");
        if ((i + 1) % 4 == 0)genomeString.append(" ");
    }

    qDebug() << "Stochastic genome is: " << genomeString;
    */
}

void Organism::setGenome(QList<bool> genome)
{
    this->genome = genome;
}

