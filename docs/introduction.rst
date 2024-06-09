Introduction
============

Overview
--------

TREvoSim is an individual-based evo-evolutionary model that lacks complex spatial structure (if you want space, check out sister package `REvoSim <https://github.com/palaeoware/revosim>`_). It is abstracted, and particularly well-suited to simulating discrete binary (~phenomic) data for phylogenetic research, and the outcomes of other evolutionary and environmental processes in the absence of space. The papers on the page :ref:`index` provide useful information regarding the software, its approach, and its limitations. 

In brief, individuals comprise binary strings, referred to throughout this documentation as their genome, which can be output as a phylogenetic matrix at the end of the run (i.e. these form the character data in the simulation). Genomes are compared with one or several environments, also comprise a series of binary strings (masks). Fitness of the individuals is calculated based on the hamming (i.e. character) distance between their genome and the masks. For a full description of the fitness algorithm, see Garwood et al. (2019) and Keating et al. (2020), cited on the :ref:`index` page. Individuals are placed in a list structure called the playing field (of which there can be one, or many, with the same -- or different -- environments), and compete with other individuals within their own playing field. An individual's fitness dictates its probability of duplication, which happens for one individual per playing field per iteration. On duplication there is a user-defined chance of a mutation being applied. TREvoSim employs a lineage-based species concept: a new species is defined based on genomic distance to either its parent species, or to that parent's last daughter species to appear. As a simulation runs, a tree representing the evolutionary relationships between the species is recorded, and the character data for each species is recorded at species extinction. A phylogeny and character data can be output after a requested number of taxa has evolved, or after a set number of iterations. 


Simulation setup
----------------

Variables can be defined by clicking on the Settings button in the toolbar. This creates a pop up window, which has two tabs: 

:Organisms and simulation: This tab contains a series of settings that defines the parameters for the simulation setup, and properties of the organisms. See :ref:`settings`.
:Environment and extinction: Variables defined within this tab control extrinsic factors that impact on the organisms. See :ref:`settings2`.

Output setup
------------

TREvoSim provides functionality for two types of logs. All logs are placed within a folder called TREvoSim_output in the save path defined in the toolbar. To change this click on change in the toolbar. The options associated with these can be accessed by clicking on the Output button in the toolbar, which creates a pop up window with the following tabs:

:End run log: 

    On this tab you can configure outputs at the end of a log, including:
    
    - Two custom output files -- the options allow  you to define file name, content (replacing keywords in ||double pipes|| with the requested information -- see :ref:`loggingsim`) and extension.
    - A standard nex tree file.
    - A working log which includes all operations the software performs across a run if you want to inspect its workings (this file can be very large).

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

Default settings
----------------

TREvoSim's default simulation parameters are intended to be a good general place to start when thinking about phylogenetic questions. Their outputs (trees, characters) are benchmarked against twelve total evidence empirical datasets. Details of the measures used, the script used to generate them, the source of the empirical data, and the output graphs from the script, are available in the folder *Benchmarking* in the source code repository.

Issue and support
-----------------

Should you wish to report issues with the software, request new features, or seek guidance on any aspect of TREvoSim, you can do so by  `submitting an issue on the TREvoSim GitHub page <https://github.com/palaeoware/trevosim/issues>`_.