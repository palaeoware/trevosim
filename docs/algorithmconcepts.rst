.. _algorithmconcepts:

Algorithm and Concepts
=======================

TREvoSim Algorithm Description
------------------------------

To do. 



Rooting
-------

TREvoSim trees don't always quite so easily fit our typical thinking for empirical trees. If we consider the root to be the last common ancestor of everything in the tree (a relatively common definition, often applied to a species), then this will be - in our case - an early individual within species zero, but not species zero itself. Due to the nature of this algorithm, under many settings, species zero will kick around for a long time, diversify, and give birth to other species, and so if we consider the root to be the oldest point in the tree, this will be the node splitting species zero (and typically its associated clade) from species one. Species zero may nest uptree somewhere - so as a species it has the oldest origin, but typically an extinction later than a number of other taxa in the tree, and it is a direct ancestor of a number of these (the genome is written at extinction by default, and so its characters best represent its later state). Given this, TREvoSim roots on the earliest node, rather than species zero,- i.e. the root lies between species 1 and (assuming this species does not evolve into its own clade, which is often the case), the other taxa. Typically, this distinction (between the individual and the species) matters most where character polarisation matters, as opposed to the tree topology per se. Given this there is the option to output character states at the root (i.e. the organism that is used to seed the simulation) in the logs (see :ref:`loggingsim` )

To do: Implement \|root\|, update logging documentation, reword above. 