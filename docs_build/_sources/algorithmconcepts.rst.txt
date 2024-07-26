.. _algorithmconcepts:

Algorithm and Concepts
=======================

TREvoSim Algorithm Description
------------------------------

TREvoSim has been developed across several publications, and the algorithm was first described in Keating et al. (`2020 <https://doi.org/10.1093/sysbio/syaa012>`_, v1.0.0), with modifications outlined in Mongiardino Koch et al (`2021 <https://doi.org/10.1098/rspb.2021.0044>`_, v2.0.0). The simulation approach also shares features with the spatially explicit eco-evolutionary simulation package REvoSim (Garwood et. al. `2019 <https://doi.org/10.1111/pala.12420>`_, Furness et. al. `2023 <https://doi.org/10.21105/joss.05284>`_). The latest version of the algorithm is described here to avoid having to build a picture by accessing multiple publications. Note, this is modified after those previous descriptions, and so employs the same wording in places.

TREvoSim employs digital organisms in the form of binary strings of a user-defined length (e.g. 0010110011101010011011). The zeros and ones comprising the string are here referred to as the genome of the digital organism, and ultimately provide the binary characters that can be used at the end of a simulation for, for example, phylogenetic inference, and they are also used to assign fitness to organisms relative to an environment, as outlined below. When a simulation runs, organisms compete, replicate, and mutate. The simulation incorporates a species concept, and speciation is emergent in the simulation, allowing the software to output a phylogenetic tree showing species relationships during or at the end of a run. 

This overview employs figures from Garwood et al. (2024) -- in all, user defined variables are shown in green font, and the values shown are the default. For clarity, the description eschews listing every potential modification to the algorithm that can be achieved through modifying the software settings, as this would obfuscate important functions in potentially unnecessary detail (although it does mention a number of key choices). Where a setting modifies the algorithm outlined below, this is described fully in the information included on the settings pages in this documentation. The implications of changing a setting are also described in these pages, wherever possible.

Data structures
^^^^^^^^^^^^^^^

Organisms reside in a structure called the playing field -- a list of organisms that, once a simulation is running, are sorted by fitness. The length of this list is user defined (this size is the limit on the number of organisms 'alive' at any given point during the simulation, and can include members of different species, as well as multiple organisms from the same species). All organisms within a single playing field compete with each other, and the default size of the playing field is 20 organisms. Associated with the playing field is one (by default), or several, environments. Environments in TREvoSim are a user-defined quantity of random numbers (= masks) that default to the same length as the organism genome. The default TREvoSim environment consists of 3 masks, of 128 characters. See below for a summary. 

.. figure:: _static/data_structures.png
    :width: 400
    :align: center

If so desired, simulations can employ more than one playing field, which can have the same, or different, environment(s). 

If you wish to modify TREvoSim code, bear in mind that there are numerous other data structures present in the underlying code. You can find an overview of these at the start of mainwindow.cpp

Fitness calculation 
^^^^^^^^^^^^^^^^^^^

Every iteration, the fitness of every organism in the playing field is calculated (using an approach first described in Garwood et al. 2019, and further detailed in Keating et al. 2020). To calculate fitness, TREvoSim uses an exclusive OR function (i.e. one that takes two inputs, outputs 1 where they differ, and 0 when they are the same) to calculate the `Hamming distance  <https://en.wikipedia.org/wiki/Hamming_distance>`_ between the genome of the organism, and each mask in an environment in turn. For example, if there are three masks, the genome is compared to each in turn. The output of this operation has a one wherever inputs differ. The ones are summed across all output strings, so with three masks, by comparing every bit of the organism genome to the equivalent bit of each mask and summing the results, TREvoSim calculates a value between 0 and 3n. Fitness is then an integer calculated as the distance from a used-defined target value: by default that is zero, creating a sharp fitness peak in the environment (a small number of possible genome configurations can achieve maximal fitness), but setting this to e.g. 1.5n will allow a far wider range of genomic configurations to achieve maximal fitness. This approach is summarised in the figure below.

.. figure:: _static/fitness_calculation.png
    :width: 400
    :align: center

Where multiple environments exist, each organism within a playing field is compared to all environments associated with that playing field, and assigned the maximal fitness achieved across all comparisons. This fitness calculation is carried out independently for each playing field when there are multiple playing fields.

TREvoSim algorithm - Summary
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The algorithm used for each TREvoSim iteration is described in full below. Key points are: members of the playing field compete, and their fitness is linked to reproductive success; and species within TREvoSim are defined based on Hamming distance (character distance) to past organisms within an evolving lineage. Key points are shown in the image below.

.. figure:: _static/algorithm.png
    :width: 400
    :align: center

TREvoSim algorithm - Initialisation 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
To initialise the simulation, the software fills the playing field (or all, if there are several) with a clone of an individual designated as the first member of species zero. Initializing with a single organism is a necessary simplification to allow all organisms in the simulation to belong to the same phylogeny. By default, the chosen organism is close to a fitness peak for the starting masks (thus preventing the simulation being a single lineage adapting to one fitness peak; if multiple playing fields are present TREvoSim opts for the maximal possible fitness of starting individual that is the same across playing fields). Masks are initialised with random strings.

A limited number of options are provided to give elements of user control to the initialisation: the initialising organism can be changed to a random individual if required, and fitness peaks across environments can be of equal height -- see :ref:`settings2`. 


TREvoSim algorithm - Iteration 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After initialisation, a simulation can be run, either until the requested number of species has evolved, or until a user defined iteration. The simulation iteration algorithm employs the following steps, which are applied every iteration to every playing field (if there are more than one) independently.

#. Calculate fitness for organisms in playing field, and sort playing field by fitness, with the fittest organisms at the top of the list. If a number of organisms have the same fitness (e.g., at initialization), these are randomly ordered.

#. An organism is picked to be duplicated via from a geometric distribution with p = 0.5 by default (i.e. there is a 50% chance of selecting the first organism in the list, then if that is not chosen, a 50% chance of selecting the second, and so on). If the simulation reaches the end of the playing field without selecting an organism, it starts from the beginning again. Alternatively, there is the option to select a random individual from the playing field, without linking this to fitness ('No selection' mode). 

#. The organism selected for duplication has a user-defined chance of mutation (defined as mutations per hundred characters per iteration; default 5.0 mutations per iteration per 100 genome bits). The user can select whether deleterious mutations are accepted.

#. If the duplicated organism, after mutation, is sufficiently different to its character string at origination it is defined as a new species -- the amount of difference required is the user defined variable species difference (default 4). Note that if this is not the first speciation in the lineage, distance is measured from the last species to diverge from its lineage -- see discussion below. If it is a new species this is recorded, and the tree display on the graphical user interface is updated.

#. The duplicated, mutated organism is then returned to the playing field, overwriting the least fit organism in the playing field by default (or randomly selecting one of the least fit if multiple least fit organisms exist). Alternatively ('Random overwrite') a random member of the playing field can be selected for overwriting.

#. Organisms in the playing field typically represent multiple species once a simulation is running. Thus the playing field is checked each iteration, and any species that have become extinct are identified. Dy default the characters of the last surviving organism are appended to the character matrix on extinction (this is the 'Sansomian speciation' option, and ensures that if a single lineage has given birth to multiple species, the recorded character set is closest to the most recently branching terminal; when this is turned off, the characters recorded for a species are its genome at origination).

#. Masks are then mutated (user-defined chance of mutation per hundred bits per iteration, default 1.0), providing environmental change throughout the simulation.

#. Optionally, other operations can occur during the iteration phase. For example, if requested, there can be mixing between playing fields. At half way through the simulation, a perturbation (elevated rates of environmental change) or ecosystem engineering (organism environment feedback) can occur. The operation of these options is described under the relevant settings in the documentation.

Once the requested number of species or iterations has been achieved, the simulation finishes. At this point (assuming the Sansomian speciation option is enabled), the character data of all extant taxa are entered into the character matrix (the fittest organism, or one of these, is selected if multiple organisms within a species are alive). The final character matrix contains all extinct and extant species. 

If stripping of uninformative characters is requested, the number of characters, and species difference, are increased at the start of a run and then informative characters are randomly subsampled at this stage to achieve the requested number of characters. A check for identical taxa is also conducted (the data are discarded and simulation repeated if the number of identical terminals is above a user-defined cut off). End of run logs are then written, and the simulation terminates.

Tree and speciation
^^^^^^^^^^^^^^^^^^^

TREvoSim employs a lineage-based species concept, based on genomic distance. Under many settings, TREvoSim species are best thought of as a population with an element of genomic diversity, which might be considered as a pangenome. When an organism is duplicated, its current genomic configuration is compared to a reference genome. At first this is a copy of the species genome at origin (i.e. for the first speciation in any given lineage, once an individual is Hamming distance == species difference from its starting point, it is considered a new species). At any speciation within a lineage, the reference genome for comparison in the parent lineage is updated to the speciating genome. Hence, after the first speciation in any lineage, comparison is to the genome at last speciation: this prevents bursts of speciation from closely related organisms sharing a common parent, but still allows cladogenesis within a species. 

.. figure:: _static/speciation_and_tree.png
    :align: center

This is summarised in the image above: the modal genome of a species is shown with solid lines, and others with a dashed line. Species 2 arises in a mutated individual in Species 0 after Species 1 arises, though this taxon is relatively close to the root genome of Species 0, because the reference genome for Species 0 (the parent of both) is updated on the birth of Species 1. This is most notable as an edge effect with Species 0 due to simulation initialisation, as outlined below. 

Tree Rooting
------------

TREvoSim trees differ in some respects from how we typically think of empirical trees, and this is particularly true when it comes to rooting. If we consider the root to be the last common ancestor of everything in the tree (a relatively common definition, most often applied to a species), then this will be -- in our case -- an early individual within species zero, but not species zero itself. Due to the nature of the TREvoSim algorithm under many settings, species zero will remain in existence for a significant period of time after the existence of this individual (the simulation is initialised with a playing field full of clones of a species zero individual, and so this species will takes longer to go extinct than most others), allowing the population of species zero organisms to further diversify, and give birth to other species. As such, if we consider the root to be the oldest point in a tree, it will be the node splitting species zero (and typically its associated clade) from species one. Species zero may nest up-tree somewhere: as a species it has the oldest origin, but typically an extinction later than a number of other taxa in the tree, and it is a direct ancestor of a number of the species in a tree (the genome is written at extinction by default, and so its characters best represent its latest state). Given this, TREvoSim roots on the earliest node, rather than species zero- i.e. the root lies between species 1 and (assuming this species does not evolve into its own clade, which is often the case), the other taxa. Typically, this distinction (between the individual and the species) matters most where character polarisation is important, as opposed to the tree topology per se. Given this, there is the option to output character states at the root (i.e. the organism that is used to seed the simulation) in the logs (see :ref:`loggingsim` ).


Default settings
----------------

TREvoSim's default simulation parameters are chosen so as to represent a sensible place to start with phylogenetic questions in mind. One hundred outputs from runs made using the default parameters are included in the repository (folder /comparison_to_empirical_data/TREvoSim_output_defaults_2024/), in addition to twelve empirical, total evidence datasets (folder /comparison_to_empirical_data/Empirical_data/ ;  data source can be found in Mongiardino Koch et al. 2021, see :ref:`index`). Also in the folder /comparison_to_empirical_data/ is an R script that compares three aspects of the output data to the properties of the empirical trees/data, outlined below. This script can either be run within R by following the instructions to update the working directory within the R script, or on systems that can run a bash script, by providing the file run_rscript.sh with execute permissions and running it, which by default outputs comparison graphs (below) to /comparison_to_empirical_data/Graphs_out. The script can be modified to quantify other outputs as desired; if you would like to add further quantification to the script but are not able to do so, feel free to raise an issue (see :ref:`Issues and support`).

Homoplasy - Extra steps
^^^^^^^^^^^^^^^^^^^^^^^

To quantify the homoplasy within data generated under default settings, the script graphs the average number of extra steps for each character for the empirical datasets (evaluated against the total evidence tree), and for the simulated data (evaluated against the true tree). This comparison is shown in the violin plot below:

.. figure:: _static/steps.png
    :align: center

The TREvoSim data ("simulated"; actually the results for all characters of the first 25 replicates provided with in the repository) is shown far right, in blue; all other datasets are labelled with taxonomic group, and represent the distribution of extra steps for all characters in the analysis. The dots show the mean for each dataset. The number of extra steps against the comparison tree of choice (total evidence for empirical, true tree for TREvoSim data), is comparable across datasets.

Tree shape - Asymmetry
^^^^^^^^^^^^^^^^^^^^^^

The tree asymmetry (= imbalance) and symmetry, is also calculated from the script. The box plots below show the symmetry for all TREvoSim trees (100 replicates) next to those for all empirical trees (twelve total evidence analyses) quantified through the J :sup:`1` index of (`Lemant et al. (2022) <https://academic.oup.com/sysbio/article/71/5/1210/6567363>`_):

.. figure:: _static/tree_asymmetry.png
    :align: center

With other metrics, the mechanism normalisation appears to have an impact on the results, whereas this measure is intrinsically normalized. There is further discussion in this pull request on the  (`TREvoSim github <https://github.com/palaeoware/trevosim/pull/53>`_). TREvoSim trees ("simulated") are generally slightly less symmetrical than those in the empirical data, and the two have a broadly similar range. Thanks to Martin Smith for his contributions to this discussion and associated improvements to the analysis and scripts.

Tree shape - Treeness
^^^^^^^^^^^^^^^^^^^^^

The script also provides a quantification of the normalised treeness (=stemminess) of a tree: the fraction of total tree length that is on internal branches of the tree. This comparison is shown in the box plots below, between 100 TREvoSim replicates and the 12 total evidence trees:

.. figure:: _static/treeness.png
    :align: center

TREvoSim ("simulated") trees have a similar median, and spread, of normalised treeness values to that found in empirical data. 
