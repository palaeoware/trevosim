.. _commandline:

Command Line Options 
====================

TREvoSim has command line options that allow simulations to be launched from a terminal or from batch files. It  has a limited number of command line switches (below): the expected use case is to load a settings file, and run replicates using these settings. An example is given below. Any combination of switches in any order is allowed. Note that on some systems, path names with spaces may cause problems: these are best avoided, but alternatively paths can be encased in double quotes if they contain a space.

Running via SSH
---------------

Due to the framework used to create the TREvoSim user interface, launching the software via the command line also launches a GUI. Launching remotely can be achieved with ease using remote desktopping solutions or SSH coupled with X-forwarding. If, however, it is required to run TREvoSim in headless mode where a GUI is not possible (e.g. on an HPC or similar), this can be achieved in a through a number of solutions that can bypass this requirement, assuming no interaction with the GUI is required. The simplest, that will work on the majority of systems with Qt installed, is to include the flag -platform offscreen when running:

.. code-block:: console
    
    ./trevosim -platform offscreen

This co-opts a Qt platform plugin to render to an offscreen buffer. Another option is to install XVFB - this is an “X server that can run on machines with no display hardware and no physical input device”  (`More information <https://www.x.org/releases/X11R7.6/doc/man/man1/Xvfb.1.xhtml>`_). Once installed, you can launch TREvoSim as follows:

.. code-block:: console
    
    xvfb-run -a  ./trevosim \--auto 1000 

You can then use e.g. SFTP to collect output files as required.

Switches
--------

These can be used as either in single letter form (-o) or long form (\--open - note the double -). Values are separated from the switch using a space or an equals, e.g. 

- -b=30
- -b 30
- \--batch=30
- \--batch 30


A full list of switches (also available using switch -h, help):

-  -h, --help                                        Displays help on commandline options.
- --help-all                                         Displays help including Qt specific options.
- -v, --version                                      Displays version information.
- -o, --open <Path to file (string)>                 Settings file to load on start.
- -b, --batch <Number of replicates (integer)>       Run in batch mode.

When no settings file is defined, TREvoSim will run with the defaults (however, this will not override the settings that persist between GUI runs). The code below, assuming the working directory is that with a TREvoSim binary, will launch TREvoSim, load the file TREvoSim_settings.xml in the same working directory, and then run 500 replicates: 

.. code-block:: console
    
    ./TREvoSim -o ./TREvoSim_settings.xml -b 100

When TREvoSim runs from the command line, information and error messages are written to the console, for instance:

.. code-block:: console
    
    Program launched from command line
    Will try to open file ./TREvoSim_settings.xml.
    File loaded, settings now as follows:
    "variables : genomeSize 128 speciesSelectSize 128 fitnessSize 128 runForTaxa 64 runForIterations 1000 playingfieldSize 20 speciesDifference 4 environmentMutationRate 1 organismMutationRate 5 unresolvableCutoff 5 environmentNumber 1 maskNumber 3 runMode 1 speciationMode 1 stripUninformative 0 writeTree 1 writeRunningLog 0 writeFileOne 1 writeFileTwo 1 writeEE 0 noSelection 0 randomSeed 0 genomeOnExtinction 1 discardDeleterious 0 fitnessTarget 0 playingfieldNumber 1 mixing 0 mixingProbabilityZeroToOne 0 mixingProbabilityOneToZero 0 playingfieldMasksMode 0 selection 10 randomOverwrite 0 ecosystemEngineers 0 ecosystemEngineersArePersistent 0 ecosystemEngineeringFrequency 10 ecosystemEngineersAddMask 0 runningLogFrequency 50 replicates 25 expandingPlayingfield0 stochasticLayer 0 stochasticDepth 1 matchFitnessPeaks 0 stochasticMap 0000000000000000"
    TREvoSim will now run your simulation. The following information may be useful:
    -- The first replicate will be shown on the GUI.
    -- If there is an error, a pop up will ask you how you would like to continue
    -- If you select to cancel runs, the program will exit, otherwise you can continue, but the software may run your simulation forevermore.
    -- If there is no error after the first replicate, TREvoSim will then run all subsequent replicates in parallel.
    Starting first pass of your remaining batch. Running 99 remaining simulations on 22 cores.
    TREvoSim has finished its replicates (or you cancelled), and will now quit.


.. note:: Some of this material is sourced from the manual for REvoSim 3.0.0.
