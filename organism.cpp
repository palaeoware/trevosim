#include "organism.h"


organism::organism(int genome_size)
{
    // Initialise everything to false/-1 to make it obvious if it has not been changed.
    for (int i=0;i<genome_size;i++){genome.append(false);parent_genome.append(false);}

    species_id=-1;
    parent_species_id=-1;

    fitness=-1;
}

void organism::initialise(int genome_size)
{
    // Initialise genome, assume this is first organism, and prog genome same as genome
        for (int i=0;i<genome_size;i++)
            {
                if (simulation_randoms->gen_random() > (MAX_RAND/2)){genome[i]=true;parent_genome[i]=true;}
                else {genome[i]=false;parent_genome[i]=false;}
            }

        //This is true for first organism. Which is pretty much only time I initialise one currently.
        species_id=0;
        parent_species_id=0;

        born=0;
        extinct=0;
        cladogenesis=0;

        fitness=0;
}

void organism::operator = (const organism &O)
{
    // Copy all attributes
    int genome_size=O.genome.size();
    for (int i=0;i<genome_size;i++)genome[i]=O.genome[i];
    for (int i=0;i<genome_size;i++)parent_genome[i]=O.parent_genome[i];
    species_id=O.species_id;
    parent_species_id=O.parent_species_id;
    fitness=O.fitness;
    born=O.born;
    extinct=O.extinct;
    cladogenesis=O.cladogenesis;
}

bool organism::operator < (const organism &O)
{
    return (fitness<O.fitness);
}

