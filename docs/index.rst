.. _index:

Welcome to TREvoSim's User Manual
=================================

The [Tr]ee [Evo]lutionary [Sim]ulator program.

TREvoSim is an individual-based evolutionary model, focussing on the simulation of evolutionary trees and associated character data through a first-principles approach. It shares a number of elements in common its sister package REvoSim (see Garwood et al. 2019, Furness et al. 2023 in relevant references), but it differs in species concept, and eschews both space and sexual reproduction.

.. figure:: _static/palaeoware_logo_square.png
    :align: center

TREvoSim has been released with the intention that it can be used as a multi-purpose platform for the study of many evolutionary phenomena, but in particular, acts as one of a limited number of methods for simulating character data and tree topology at the same time and is one of very few individual-based simulations capable of doing so. We note that, as with REvoSim, this package is complementary to the many other approaches of studying evolution on a range of different timescales, and will be continually developed by the core team to expand its capabilities.

A full description of the algorithm can be found in Keating *et al.* (2020), referenced below. In brief, a simulation employs digital organisms comprising binary strings. These are used to calculate fitness through comparison to against one or several sets of random binary strings (the environment(s)), and ultimately provide the character matrices and trees output by the simulation. Organisms exist in a playing field and compete with each other; one is selected, the probability of which being based on a fitness algorithm, for reproduction each iteration. When this occurs, the organism is mutated, and returned to the playing field, overwriting the least fit organism. A new species is defined based on hamming distance to the parent species, or last species to originate within the lineage. Species are kept in a list, and their genomes are recorded on extinction. The simulation runs until the requested number of species have existed, and then writes data in the requested format.

e: `palaeoware@gmail.com <mailto:palaeoware@gmail.com>`_

w: `https://github.com/palaeoware <https://github.com/palaeoware>`_


Relevant references
-------------------

Garwood, R.J., Spencer A.R.T. and Sutton, M.D., 2019. REvoSim: Organism-level simulation of macro- and microevolution. Palaeontology 62(3),339-355. `https://doi.org/10.1111/pala.12420 <https://doi.org/10.1111/pala.12420>`_ (describes the initial release of REvoSim, including fitness algorithm).

Keating, J.N., Sansom, R.S., Sutton, M.D., Knight, C.G. and Garwood, R.J., 2020. Morphological phylogenetics evaluated using novel evolutionary simulations. Systematic Biology 69(5):897-912. `DOI: 10.1093/sysbio/syaa012 <https://doi.org/10.1093/sysbio/syaa012>`_ (describes, and utilises, TREvoSim v1.0.0)

Mongiardino Koch, N., Garwood, R.J. & Parry, L.A. 2021. Fossils improve phylogenetic analyses of morphological characters. Proceedings of the Royal Society B 288(1950):20210044. `DOI: 10.1098/rspb.2021.0044 <https://doi.org/10.1098/rspb.2021.0044>`_ (describes, and utilises, TREvoSim v2.0.0)

Mongiardino Koch, N., Garwood, R.J. & Parry, L.A. 2023. Inaccurate fossil placement does not compromise tip-dated divergence times. Palaeontology 66 (6): e12680. `DOI: 10.1111/pala.12680 <https://doi.org/10.1111/pala.12680>`_ (utilises TREvoSim v2.0.0)

Table of Contents
=================
.. toctree::
   :maxdepth: 3
   :numbered:

   introduction
   requirementsinstallation
   buildingfromsource
   algorithmconcepts
   windowlayout
   settings
   settings2
   loggingsim
   filecommands
   software_tests
