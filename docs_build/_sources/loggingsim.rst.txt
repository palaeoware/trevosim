.. _loggingsim:

Logging the Simulation
======================

Logging system
--------------

End run log
^^^^^^^^^^^

TREvoSim has a versatile logging system which allows the user to define outputs in a range of formats required for phylogenetic inference packages, or e.g. R, provided these allow plain text inputs. Clicking on the output button of the toolbar will launch the output dialogue, which has two tabs. The first is a log that is output at the end of the run:

.. figure:: _static/output_01.png
    :align: center
    :alt: The output settings page for the end run log in TREvoSim

    The TREvoSim End Run Log settings

A typical use case for this is when you would like to use the outputs of a finished simulation to do downstream analyses for e.g. phylogenetic methods. This tab provides options for two custom log files, which are placed, after a run, within the *TREvoSim_output* folder created on the save path. Basename defines the start of the filename, and if you so wish, this can include forward slashes before the start of the file name to denote a path, that TREvoSim will create for you. TREvoSim then adds a number to the start of the filename you have suggested, and this number iterates with subsequent runs. The file extension is also defined in this dialogue in a separate text box. 

The two text boxes allow custom file content to be written: text from these is written as provided to the file -- for example with run instructions or program commands -- and any of a series of keywords defined below included within two vertical bar ( | ) symbols are replaced as a file is written. The default outputs are shown in the figure above -- they create a vanilla nexus file which allows you to, for example, load the character matrix into R, and a TNT file which conducts a parsimony search on TREvoSim data before outputting the most parsimonious trees. 

In addition to these two custom files, TREvoSim can output a tree for each run in a standard nexus format. The options at the bottom of the output tab allow you to define the basename for this file. The output includes the tree and translate block, as well as a comment with the settings of the run written to it. 

.. note::
    See also :ref:`Data analysis` for pointers on how to analyse TREvoSim outputs. 

Running log
^^^^^^^^^^^

The other tab is a running log which is output during a run:

.. figure:: _static/output_02.png
    :align: center
    :alt: The output settings page for the running log in TREvoSim

    The TREvoSim Running Log settings

This is of utility for studying processes as they occur during a run, as it is output on a regular basis as the simulation progresses. Any text entered into the Custom running log text box is output to a new file (appended with an iterator) at the requested frequency. This can be used to record anything in the state of the simulation required to study a process of interest. At the top of the dialogue are two options:

Write running log
  By default the running log is not created or written to when a simulation runs. In order to write the files this option needs to be enabled.
Frequency
  This dictates the frequency, in iterations, with which a new running log file is written. 

Below this is a free text box. This works the same way, and uses the same keywords (shown below) as the end run log, outputting data in the current state for the iteration at which the log is written -- it is only possible to enter text into this box when the Write running log checkbox is ticked.

There are then two further options at the base of the dialogue:

Write full working log 
  When this is checked, TREvoSim outputs a text file that it appends to as it runs. This file outlines every step of each iteration, such as the state of the playing field, the environment, and the processes the software is going through. This helps understand and fact check any given run, but for significant playing field sizes, taxon numbers, or character numbers, it creates a large text file (10s -- 100s of MB). 
Write ecosystem engineers
  There is a custom log for simulations in which ecosystem engineers are enabled (the nature of this functionality does not allow all required information to be easily output using the running log: this is primarily a convenience function).

Keywords
--------

Keywords within two vertical bars ( e.g. \|\|Matrix\|\| ) are replaced as a file is written as follows:

Character_Number
  This outputs the character number.
Count
  This is replaced with a counter for batch runs; starting from zero and incrementing by one. This is padded with leading zeroes to three figures.

Ecosystem_Engineers
  This prints a list of species, and their ecosystem engineering status (i.e. whether any members of this species are ecosystem engineers). A minimal example is shown below.

  ::

    Applying ecosystem engineers for 1 time on iteration 10
    Masks before ecosystem engineers :
    Playingfield 0
    Environment 0
    Mask number 0 :	0001101111
    Mask number 1 :	0111110101
    Mask number 2 :	1111001000


    Playing field(s):

    Playing field number,Playingfield position,Species ID,Ecosystem engineer,Genome
    0,0,0,0,0111111101
    0,1,0,0,0101111101
    0,2,10,0,1111111001
    0,3,0,0,0111101101
    0,4,0,0,0111110101


    Playing field 0 organism number 0 selected. Genome is 0111111101.
    Masks after ecosystem engineers:
    Playingfield 0
    Environment 0
    Mask number 0 :	0001101111
    Mask number 1 :	0111110101
    Mask number 2 :	0111111101


    Playing field(s) after ecosystem engineers:

    Playing field number,Playingfield position,Species ID,Ecosystem engineer,Genome
    0,0,0,1,0111111101
    0,1,0,0,0101111101
    0,2,10,0,1111111001
    0,3,0,0,0111101101
    0,4,0,0,0111110101

Identical
  This outputs the number of identical taxa in a run. 

Iteration
  This outputs the current iteration number. 

Masks


Matrix
  This is replaced with the matrix from the run. For example:

  ::

    Species_00	0111011110
    Species_01	0101011011
    Species_02	0001011100
    Species_03	0110111111
    Species_04	1110101110
    Species_05	1111111010

MrBayes_Tree
  This writes a tree in standard Newick format, including branch lengths (these are based on iteration number throughout):

  ::

    (Species_01:2,(Species_02:2,(Species_03:2,Species_00:19):5):1):13

  .. note:: 
    The mechanism used for tree writing differs between TNT and MrBayes outputs -- the tree topology is the same, but the taxon order differs.

PlayingField, PlayingField_semiconcise,PlayingField_concise
  These commands print the current state of the playing fields in expansive, through to concise, formats. The first option, PlayingField, will output the following details for every organism in every playing field:

  ::

    Playingfield pos: 0 
    Species ID: 35
    Genome:	01111100111010111100110001011100
    Parent genomes:
    Genome 0	01111100111110111100110001011100
    Genome 1	01111010101011111111110001111100
    
    Fitness:	45
    Ecosystem engineer:	0

  Using PlayingField_semiconcise, the playing field will be printed as follows:
  
  ::

    Playing field number,Playingfield position,Species ID,Ecosystem engineer,Genome
    0,0,35,0,01111100111010111100110001011100
    0,1,36,0,01001010101110111111110001101101
    0,2,32,0,00010010011110011101110001011100
    0,3,31,0,01111010101110111111110001101101
    0,4,35,0,01111100111110111100110001011100
    0,5,28,0,01111100011111111100110001011100
    0,6,9,0,01011010101110111111111001111100
    0,7,24,0,00011010011110011101110001011100
    0,8,0,0,01111010101110111101110001111101
    0,9,9,0,01011110101110110111111001111100

  And using PlayingField_concise, the format will be:

  ::

    Playing field number,Playingfield position,Species ID,Ecosystem engineer
    0,0,35,0
    0,1,36,0
    0,2,32,0
    0,3,31,0
    0,4,35,0
    0,5,28,0
    0,6,9,0
    0,7,24,0
    0,8,0,0
    0,9,9,0

Root
  Writes the genome of the organism used to seed the simulation, thereby allowing characters to be polarised correctly with respect to the root of the tree (see :ref:`algorithmconcepts`):

  ::
    
    01000100000010010000011100001111

Settings
  Writes the current TREvoSim settings to the file (this is provided as a useful way to record, with any output data, the state of the TREvoSim variables for any given run). Shown below for the defaults:

  ::
    
    variables : genomeSize 128 speciesSelectSize 128 fitnessSize 128 runForTaxa 64 runForIterations 1000 playingfieldSize 20 speciesDifference 4 environmentMutationRate 1 organismMutationRate 5 unresolvableCutoff 5 environmentNumber 1 maskNumber 3 runMode 1 stripUninformative 0 writeTree 1 writeRunningLog 0 writeFileOne 1 writeFileTwo 1 writeEE 0 noSelection 0 randomSeed 0 sansomianSpeciation 1 discardDeleterious 0 fitnessTarget 0 playingfieldNumber 1 mixing 0 mixingProbabilityZeroToOne 0 mixingProbabilityOneToZero 0 playingfieldMasksMode 0 selection 10 randomOverwrite 0 ecosystemEngineers 0 ecosystemEngineersArePersistent 0 ecosystemEngineeringFrequency 10 ecosystemEngineersAddMask 0 runningLogFrequency 50 replicates 25 expandingPlayingfield0 stochasticLayer 0 stochasticDepth 1 matchFitnessPeaks 0 stochasticMap 0000000000000000

Stochastic_Matrix
  This writes the matrix in the format introduced above, but writes the stochastic layer for each taxon (see :ref:`Organisms`).

TNT_Tree
  This writes a tree, if required, in TNT format (i.e. only brackets and terminal labels):

  ::

    (((00 03) 02) 01)

  .. note:: 
    The mechanism used for tree writing differs between TNT and MrBayes outputs -- the tree topology is the same, but the taxon order differs.

Time
  Adds a timestamp:

  ::

    Written on Fri Jun 28 07:09:14 2024

Taxon_Number
  Writes the number of species that have existed since the start of the simulation at the iteration it is called.
Unresolvable
  This prints a list of unresolvable taxa (or a notice that there are none if required).
Uninformative
  Writes the number of uninformative characters.





Keywords example 
^^^^^^^^^^^^^^^^

As an example, the following -- entered as one of the custom files at the end of the run -- would output a block of text that could be run as a macro in tnt:

::
 
  mxram 100;
  NSTATES nogaps;
  xread
  'Written on ||Time|| Variables: ||Settings||'
  ||Character_Number|| ||Taxon_Number||
  ||Matrix||
  ;
  piwe-;
  keep 0; hold 100000;
  rseed *;
  xmult = level 10; bbreak;
  export - TREvoSim_run_||Count||_mpts.nex;
  xwipe;

Should any other output options be required, please file a `feature request <https://github.com/palaeoware/trevosim/issues>`_. Keywords are not case sensitive.
