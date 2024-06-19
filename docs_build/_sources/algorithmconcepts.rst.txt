.. _algorithmconcepts:

Algorithm and Concepts
=======================

TREvoSim Algorithm Description
------------------------------

To do. 


Tree Rooting
------------

TREvoSim trees differ in some respects from how we typically think of empirical trees, and this is particularly true when it comes to rooting. If we consider the root to be the last common ancestor of everything in the tree (a relatively common definition, most often applied to a species), then this will be -- in our case -- an early individual within species zero, but not species zero itself. Due to the nature of the TREvoSim algorithm, under many settings, species zero will remain in existence for a significant period of time after the existence of this individual, diversify, and give birth to other species. As such, if we consider the root to be the oldest point in a tree, it will be the node splitting species zero (and typically its associated clade) from species one. Species zero may nest uptree somewhere: as a species it has the oldest origin, but typically an extinction later than a number of other taxa in the tree, and it is a direct ancestor of a number of the species in a tree (the genome is written at extinction by default, and so its characters best represent its latest state). Given this, TREvoSim roots on the earliest node, rather than species zero- i.e. the root lies between species 1 and (assuming this species does not evolve into its own clade, which is often the case), the other taxa. Typically, this distinction (between the individual and the species) matters most where character polarisation is important, as opposed to the tree topology per se. Given this there is the option to output character states at the root (i.e. the organism that is used to seed the simulation) in the logs (see :ref:`loggingsim` )


Default settings
----------------

TREvoSim's default simulation parameters are chosen so as to represent a sensible place to start with phylogenetic questions in mind. One hundred outputs from runs made using the default parameters are included in the repository (/comparison_to_empirical_data/TREvoSim_output_defaults_2024/), and 

Their outputs (trees, characters) are benchmarked against twelve total evidence empirical datasets. Details of the measures used, the script used to generate them, the source of the empirical data, and the output graphs from the script, are available in the folder *Benchmarking* in the source code repository.
