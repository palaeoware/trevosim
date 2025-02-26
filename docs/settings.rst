.. _settings:

Settings - Organisms and simulation
===================================

Clicking on the settings button of the toolbar will launch a settings dialogue which has two tabs - one with settings for the organisms in the simulation, and one controlling the environment for the simulation.

.. figure:: _static/settings_01.png
    :align: center
    :width: 550
    :alt: A screenshot of the TREvoSim settings panel for Organisms and simulation

    The TREvoSim Organisms and Simulations settings panel

Organisms
---------

Number of characters/genome size
    This defines the number of bits in character string for each organism, and ultimately the character number of the matrix output by the software.
Characters for species identification
    This defines the number of characters used to identify species. When this is different to the genome size, only the characters between zero and this number are used in the species delineation parts of the TREvoSim algorithm. By default (when the genome size is changed in the settings) this number will update to be the same value as the genome size, but changing it to a smaller value allows a portion of characters that are not tied to species identity.
Characters for fitness calculation
    This defines the number of characters used in the fitness algorithm. When this is different to the genome size, on the characters between zero and this number are used to calculate fitness. By default this will update to be the same value as the genome size, but changing it to a smaller value allows for a 'non-coding' portion of the genome, with a more drift-like/stochastic behaviour (note, however, that this portion of genome is still part of an organism that is under selection).
Rate of organism mutation
    This is the rate of mutation for the organisms in the simulation, in units of mutations per hundred characters per iteration.
Stochastic layer
    This tick box dictates whether a stochastic layer is used. The stochastic layer is a secondary string of bits that can be used to abstract the fitness calculation from the genome and its mutations, through a mechanism based on many-to-one mapping. When this box is ticked, on settings close, another dialogue is launched that allows you to - for every possible combination of 4 bits - define an output, i.e. 0 or 1, referred to here as the map. When the setting is enabled, each organism stores an internal genome four times the length of the organism's gnome. During the simulation, mutations are applied to the stochastic layer (at the user defined rate per 100 characters), but all other operations employ the standard genome. When mutations are applied, the map is then used to convert bits from the stochastic layer into the genome that is used elsewhere (i.e. for every four stochastic bits, the output is 1 standard bit, based on the map). This abstraction process could be considered, for instance, a highly simplified parallel to e.g. ontogeny: it removes the fitness of the organism from direct control of its genome, and thus facilitates silent mutations and neutral theory of evolution type / less strongly adaptationist dynamics. Note: The stochastic layer only works currently when the character number, characters for species identification, and characters for fitness calculation are the same. As such, selecting this option sets all values to be the same as total character number, and prevents changes when selected to all but total character number. 
Depth of layer
    In future versions, to allow further abstraction, varying the number of layers created from this map is planned (e.g. if the depth is two, then a second stochastic layer will be created from the first, the second being sixteen times the length of the organismal genome, and dictating the makeup of the first). This is not currently implemented, and so at present, this option is limited to one. 

Simulations
-----------

Run mode radio
    This has two options, run for iteration number or run to taxon number, which switches between the two options below.
Run to taxon number
    If run mode is taxon number, a simulation will run until this number of species has evolved, and then terminate.
Run for iterations number
    If run mode is iteration number, a simulation will run until this number of iterations, and then terminate.
Species difference
    The Hamming distance between a selected organism, post mutation, and the character string defined by the speciation mode setting (below) is used by the TREvoSim species concept. This setting defines the required Hamming distance for a speciation to have occurred.
Species mode
    From v3.1.0 TREvoSim offers three different speciation modes: when assessing whether a new species has appeared within a lineage, these modes differ in the genome comparisons made -- the genome at species birth, the last speciation from the lineage, or all the origin and all speciating genomes that have occurred. See `Issue #59 for discussion <https://github.com/palaeoware/trevosim/issues/59>`_.
Unresolvable cut off
    With low character numbers, especially when TREvoSim is set to strip out uninformative characters, terminals can have the same character string within a matrix, and thus be unresolvable. This setting defines upper limit for identical terminals. If there are more unresolvable taxa than this number in a single run, output files are not written and a warning is provided. If batch mode is underway, the current run is discarded and started again.
Fitness target
    This is the target value for the count of 1s in the fitness calculation. See :ref:`algorithmconcepts` for details of the fitness algorithm. At low and high values (max being mask number multiplied by character number) fewer genomes will have peak fitness, at half max value, a larger number of peak fitness organisms exist. This can be quantified using the :ref:`Fitness histogram` menu command.
No selection
    If this option is enabled, fitnesses are not calculated for digital organisms in the playing field, and instead every iteration and random individual is selected for replication.
Record genomes on extinction
    This option controls when the character string of a species is recorded, which can occur either at the point of speciation, or at extinction. When this option is checked, which it is by default, the genome/characters of a species are written when that species goes extinct. This is useful, in particular, with respect to species zero -- that used to seed the simulation. Because this tends to survive for longer than other species (see discussion in :ref:`Tree Rooting`), the genome at the start of a species lifespan could differ significantly from that at its extinction. Recording the genome of the last individual of the species ensures in such long-surviving species that the recorded characters are closest that of its sister species (and also the last common ancestor of these two species). When the option is not selected, genome is recorded at the birth of each species.
Discard deleterious mutations
    Optionally, TREvoSim can accept only mutations which are neutral, or improve the fitness of an organism.
Strip uninformative characters via subsampling
    TREvoSim gives the option of writing matrices of only parsimony informative characters. When this option is checked, the software attempts to provide the requested character number of only informative characters. It achieves this by multiplying the number of characters and species difference by a factor (the strip uninformative factor) at the start of a run, that defaults to a value of 5.0. After a run has completed, informative characters are randomly subsampled to the requested number of characters. If there are not enough characters to achieve this, in batch mode the run is discarded and restarted, in single run mode an error message is provided. Note that the strip uninformative factor can be set manually, or recalculated empirically for the current settings (see :ref:`Set uninformative factor` / :ref:`Recalculate uninformative factor for current settings` ).
Random starting individual
    By default, the organism used to initialise a simulation is one near peak fitness for the starting environment(s), to prevent the resulting tree from documenting a lineage adapting to a fitness peak (this results in a highly asymmetrical tree). When checked, this option switches to using a random individual to initialise the simulation.