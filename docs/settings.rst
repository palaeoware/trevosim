.. _settings:

Settings dialogue
=================

Clicking on the settings tab of the toolbar will launch a settings dialogue which has two tabs - one with settings for the organisms in the simulation, and one controlling the environment for the simulation.

Organisms and simulation tab
----------------------------

.. figure:: _static/settings_01.png
    :align: center

**Organisms**

:Number of characters/genome size: This defines the number of bits in character string for each organism, and ultimately the character number of the matrix output by the software.
:Rate of organism mutation: This is the rate of mutation for the organisms in the simulation, in units of mutations per hundred characters per iteration.

**Simulations (Organisms)**

:Taxon number: A simulation will run until this number of species has evolved, and then terminate.
:Species difference: The hamming distance between a selected organism, post mutation, and either the character string when the species originated, or the last species to originate within the lineage (see paper for rationale) is used within the TREvoSim species concept. This setting defines the required hamming distance for a speciation to have occurred.
:Unresolvable cutoff: With low character numbers, especially when TREvoSim is set to strip out uninformative characters, terminals can have the same character string within a matrix, and thus be unresolvable. This setting defines upper limit for identical terminals. If there are more unresolvable taxa than this number in a single run, output files are not written and a warning is provided. If batch mode is underway, the current run is discarded and started again.
:Sansomian speciation: This option dictates when the character string of a species is recorded: at speciation, or at extinction. Sansomian speciation, the default, is to record the this when a species goes extinct. This ensures if it is a long-surviving species that the recorded characters string best reflects that closest to its sister group. When the option is not selected, genome is recorded at speciation.
:Discard deleterious mutations: Optionally, TREvoSim can accept only mutations which are neutral, or improve the fitness of an organism.
:Strip uninformative characters via subsampling: TREvoSim gives the option of writing matrices of only parsimony informative characters.
