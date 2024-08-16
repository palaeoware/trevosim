Introduction
============

Overview
--------

TREvoSim is an individual-based evo-evolutionary model that lacks complex spatial structure (if you want space, check out sister package `REvoSim <https://github.com/palaeoware/revosim>`_). It is abstracted, and particularly well-suited to simulating discrete binary (~phenomic) data for phylogenetic research, and the outcomes of other evolutionary and environmental processes in the absence of space. The papers listed under :ref:`Relevant references` provide useful information regarding the software, its approach, and its limitations. 

In brief, individuals comprise binary strings, referred to throughout this documentation as their genome, which can be output as a phylogenetic matrix at the end of the run (i.e. these form the character data in the simulation). Genomes are compared with one or several environments, also comprise a series of binary strings (masks). Fitness of the individuals is calculated based on the hamming (i.e. character) distance between their genome and the masks. For a full description of the fitness algorithm, see Garwood et al. (`2019 <https://doi.org/10.1111/pala.12420>`_) and Keating et al. (`2020 <https://doi.org/10.1093/sysbio/syaa012>`_), cited under  :ref:`Relevant references`. Individuals are placed in a list structure called the playing field (of which there can be one, or many, with the same -- or different -- environments), and compete with other individuals within their own playing field. An individual's fitness dictates its probability of duplication, which happens for one individual per playing field per iteration. On duplication there is a user-defined chance of a mutation being applied. TREvoSim employs a lineage-based species concept: a new species is defined based on genomic distance to either its parent species, or to that parent's last daughter species to appear. As a simulation runs, a tree representing the evolutionary relationships between the species is recorded, and the character data for each species is recorded at species extinction. A phylogeny and character data can be output after a requested number of taxa has evolved, or after a set number of iterations. 

Statement of Need
-----------------

The JOSS paper, listed under :ref:`Relevant references` includes a Statement of Need, which serves to introduce the utility of TREvoSim. For convenience, this statement is included here too.

TREvoSim employs a selection-driven, agent-based approach: it incorporates key elements of biological evolution (selection, reproduction, heritability and mutation). The (true) phylogenetic trees and character data are an emergent property of a TREvoSim simulation, and as such the software is particularly well suited to simulation studies that can be analysed through phylogenetic trees and character data matrices. These include, for example: the impact of missing data on phylogenetic inference; the impact of rates of environmental change on character evolution; and the nature of evolution under different fitness landscapes. This complements other simulation approaches where, typically, phylogenetic trees or character data are simulated using stochastic tools such as birth-death models (e.g. `Guillerme 2024 <https://doi.org/10.1111/2041-210X.14306>`_) or data based on random numbers (e.g. `Puttick, O'Reilly, Pisani & Donoghue 2019 <https://doi.org/10.1111/pala.12388>`_) that do not incorporate, for example, selection acting on individuals. The data TREvoSim generates are different in a number of ways to those created using stochastic models (`Keating et al. 2020 <https://doi.org/10.1093/sysbio/syaa012>`_), and are also likely to violate the assumptions of models commonly used in phylogenetic inference. Incorporating a level of model misspecification resembling that expected from empirical datasets is desirable in simulation studies that assess the efficacy of inference methods. Given the complexity of morphological evolution, the subsequent impact of character coding, and our current understanding of the patterns present in empirical character data, it is challenging to demonstrate, beyond the inclusion of empirically grounded concepts in its generation, the naturalism of TREvoSim data. The default settings have been chosen to reflect a number of features of empirical data matrices and trees to try and minimise the mismatch between TREvoSim and real world data, however, there are a broad range of potential alternative means of quantifying outputs. Which of these is most appropriate is likely to depend on the area of study and specific question at hand, and as such, TREvoSim provides granular control over the simulation parameters, allowing users to generate data that best serve their needs. TREvoSim is intended as a versatile platform that might be used to study a broad range of topics. 

Simulation setup
----------------

Variables can be defined by clicking on the Settings button in the toolbar. This creates a pop up window, which has two tabs: 

:Organisms and simulation: This tab contains a series of settings that defines the parameters for the simulation setup, and properties of the organisms. See :ref:`settings`.
:Environment and extinction: Variables defined within this tab control extrinsic factors that impact on the organisms. See :ref:`settings2`.

Variables are saved on software exit, and thus persist between sessions. If required at any point, it is possible to :ref:`Restore default settings`.

Output setup
------------

TREvoSim provides functionality for two types of logs. All logs are placed within a folder called TREvoSim_output in the save path defined in the toolbar. To change this click on change in the toolbar. The options associated with these can be accessed by clicking on the Output button in the toolbar, which creates a pop up window with the following tabs:

:End run log: 

    On this tab you can configure outputs at the end of a log, including:
    
    - Two custom output files -- the options allow  you to define file name, content (replacing keywords in ||double pipes|| with the requested information -- see :ref:`loggingsim`) and extension.
    - A standard nex tree file.
    
:Running log: 

    This tab allows a running log to be defined that can record the state of the simulation at user-requested frequencies (see :ref:`loggingsim`). The tab includes options for:

    - Header text
    - Body text 
    - An option to write the running log (if not requested this is not output)
    - A tick box to output for a specialised Ecosystem Engineering log
    - A spin box that dictates the frequency with which which the running log is written


Quick start
-----------

The TREvoSim defaults will allow you to create exemplar data in TREvoSim by hitting the Run button on the toolbar. This will output a nexus file with the final character matrix in it that can be loaded in e.g. R for analysis, placed in a folder called *TREvoSim_output*, the default location of which is your desktop. It will also output a nexus formatted tree file in this folder. 


Experimental design
-------------------

TREvoSim is designed to offer the user flexibility, but at the same time, this requires experiments to incorporate a wide range of decisions regarding the simulation variables, which will need to reflect the question at hand. In some cases the impact of modifying one variable can be derived from first principles (for example, the larger the playing field, and thus population of digital organisms, the longer species are likely to survive before going extinct, and the longer tips are likely to be on the resulting tree). In other instances, an easier approach is to get a feel of the impact that modifying each variable has on the measures of interest (e.g. tree symmetry, homoplasy) by iteratively changing one variable at a time and studying the outputs. Given the significant parameter space of the simulation, we provide an example here in which TREvoSim has been used to study the impact of including fossils on phylogenetic inference -- full details can be found in the paper of Mongiardino Koch et al. (`2021 <https://doi.org/10.1098/rspb.2021.0044>`_). Simulation parameters were selected by comparing generated outputs (once subsampled as outlined below) with empirical total evidence datasets, and comparing a number of different metrics. This was achieved using an R script provided in the `supplementary information for that paper <https://doi.org/10.5061/dryad.4xgxd2585>`_ (see also, below: a modified, simpler version of that script is included in the source code of TREvoSim). Through iterative modification of the parameter settings, we were able to ensure that the outputs of TREvoSim were within the range of our empirical datasets for the metrics in question. Ultimately, our chosen parameters included:

* A genome of size 500 (i.e. 500 characters in the phylogenetic matrix), allowing at least 300 parsimony-informative characters for our study after taxon subsampling and missing data imputation.
* Simulations that ran to 999 terminals allowing us to subsequently subsample these (reasoning outlined below). 
* Multiple playing fields to facilitate an early rapid radiation in our trees (one clade per playing field; ancient short internodes are often challenging to resolve for phylogenetic inference), and number of extant terminals at the end of a simulation. 
* Random overwrite when returning a duplicated organism to a playing field, providing a wider variance in terminal branch lengths. 
* To achieve the desired measures of tree symmetry and branch length we employed a fitness target of 0 and selection coin toss of 2, plus 5 playing fields of size 40, each with 5 non-identical environments of 3 masks, and a species difference of 8: all of these settings contribute towards the topology of the tree that evolves during a simulation.
* Species difference also impacts on the homoplasy of the underlying character data, which informed the choice of this parameter value. 
* An unresolvable cut off of 2, precluding identical terminals.

Subsequent data manipulation, conducted in R, included the following:

* We removed trees with zero length branches (`see the supplementary methods for details of why these exist, and why we chose to remove them <https://royalsocietypublishing.org/doi/suppl/10.1098/rspb.2021.0044>`_).
* The remaining trees had a mean of ~150 extant terminals, and we removed fossils from these at random until only 300 terminals remained, to reflect the loss of biodiversity in the fossil record through the process of fossilisation.
* At this point we compared simulations to twelve empirical datasets -- full discussion of our comparison is available in the `supplementary methods <https://royalsocietypublishing.org/doi/suppl/10.1098/rspb.2021.0044>`_. 
* We then subsampled these again for our analyses to create our independent variables -- we varied the proportion of fossils in our datasets to assess the impact this had, and also imputed different levels of missing data. 
* Through this process, we ultimately created 11,250 datasets for our experiments, which we subjected to phylogenetic inference, before comparing the inferred trees to the true tree topology for our results and discussion. 

Note that this is a summary, and further discussion of all points is available in that paper, as well as areas in which our data demonstrably differ to empirical datasets (such as the distribution of missing data). 

Data analysis
-------------

The scripts linked from the section above contain a full work flow from TREvoSim outputs to the publication figures, and can be used as an example of how to load, manipulate and analyse TREvoSim datasets in R. They are, however, also quite complex given the nature of the paper. A potentially gentler learning curve for achieving this same goal may be found in the R script included in the source code, in the folder comparison_to_empirical_data. This was used to quantify and compare TREvoSim outputs to total evidence datasets, but without the data manipulation included in the above workflow to allow the impact of including fossils to be investigated. Because the repository also includes TREvoSim datasets, it will run from the repository  without modification (although if launched within e.g. RStudio, you will need to set the working directory, as per the comments in the script). The script if fully commented throughout. 

Defaults
--------

TREvoSim's default simulation parameters are intended to be a good general place to start when thinking about phylogenetic questions. See :ref:`Default settings` for more information. 

Software behaviour 
------------------

Within each session, TREvoSim keeps a counter of the run number, which it increments when you press play (to run a simulation) or when you run a batch. Cancelling a batch part way through will result in this counter not being updated, and any runs written at the point of cancellation being subsequently overwritten. This counter is reset between sessions, and runs from subsequent instances will overwrite older ones if the output location remains unchanged. TREvoSim saves all other settings between  sessions -- although these can be reset to default if required (see :ref:`Restore default settings`). 


Issues and support
------------------

Should you wish to report issues with the software, request new features, or seek guidance on any aspect of TREvoSim, you can do so by  `submitting an issue on the TREvoSim GitHub page <https://github.com/palaeoware/trevosim/issues>`_.