Welcome to TREvoSim's User Manual
=================================

The [Tr]ee [Evo]lutionary [Sim]ulator program.

TREvoSim is an individual-based evolutionary model, focussing on the simulation of evolutionary trees and associated character data through a simplified first-principles approach. It shares a number of elements in common with the model REvoSim (see Garwood et al. 2019, relevant references), but differs in species concept, and eschews both space and sexual reproduction.

.. figure:: _static/palaeoware_logo_square.png
    :align: center

TREvoSim has been released with the intention that it can be used as a multipurpose platform for the study of many evolutionary phenomena, but in particular, acts as one of the few methods for simulating character data and tree topology at the same time, and one of a highly limited number of individual-based simulations capable of doing so. We note that, as with REvoSim, this package is complementary to the many other approaches of studying evolution on a range of different timescales, and will be continually developed by the core team to expand its capabilities.

A full description of the algorithm can be found in Keating *et al.* (2020), referenced below. In brief, a simulation employs digital organisms comprising binary strings. These are used to calculate fitness relative to the environment, and ultimately provide the character matrices output by the simulation. Organisms exist in a playing field and compete with each other; one is selected, the probability of which being based on a fitness algorithm, for reproduction each iteration. When this occurs, the organism is mutated, and returned to the playing field, overwriting the least fit organism. A new species is defined based on hamming distance to the parent species, or last species to originate within the lineage. Species are kept in a list, and their genomes are recorded on extinction. The simulation runs until the requested number of species have existed, and then writes data in the requested format.

t: `@palaeoware <https://twitter.com/palaeoware>`_

e: `palaeoware@gmail.com <mailto:palaeoware@gmail.com>`_

w: `https://github.com/palaeoware <https://github.com/palaeoware>`_


Relevant references
-------------------

Garwood, R.J., Spencer A.R.T. and Sutton, M.D., 2019. REvoSim: Organism-level simulation of macro- and microevolution. Palaeontology 62(3),339-355. `DOI: 10.1111/pala.12420 <https://doi.org/10.1111/pala.12420>`_

Keating, J.N., Sansom, R.S., Sutton, M.D., Knight, C.G. and Garwood, R.J., 2020. Morphological phylogenetics evaluated using novel evolutionary simulations. Systematic Biology 69(5):897-912. `DOI: 10.1093/sysbio/syaa012 <https:/https://doi.org/10.1111/pala.12420/doi.org/10.1093/sysbio/syaa012>`_

Koch, N.M., Garwood, R.J. and Parry, L.A., 2020. Fossils improve phylogenetic analyses of morphological characters. bioRxiv.

Table of Contents
=================
.. toctree::
   :maxdepth: 3
   :numbered:

   requirements
   windowlayout
   settings
   loggingsim
   commands
