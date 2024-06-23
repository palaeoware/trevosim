.. _filecommands:
.. include:: <isonum.txt>

Menu commands
=============

TREvoSim has a series of options which can be selected using the menu options at the top left of the program window (above the run button). These are as follows.

File
----

Save Current Settings
^^^^^^^^^^^^^^^^^^^^^

By default TREvoSim saves the simulation settings between sessions (it writes a settings file to the same folder as the software binary / executable on close). This menu command (or shortcut ctrl + s) rewrites that setting file without closing the program.

Save settings as...
^^^^^^^^^^^^^^^^^^^

This opens a file save dialogue, and then writes a TREvoSim settings file to the requested location (shortcut ctrl + shift + s). TREvoSim settings files are written in an XML document that can be opened in any text application.

Load settings from file...
^^^^^^^^^^^^^^^^^^^^^^^^^^

Load settings file (ctrl + o) opens a saved settings file and updates the simulation settings accordingly.

Commands 
--------


Fitness histogram
^^^^^^^^^^^^^^^^^

The fitness algorithm for TREvoSim is described in Keating et al. (2020). The fitness landscape varies depends on the simulation settings, and the randomly chosen sequences that are used to create the environmental masks. This menu command (shortcut ctrl + shift + h) provides a basic assessment of that landscape by showing the distribution of fitnesses for every possible genome for the current settings. It provides an option to select how many bits in the test organism's genome: run times increase exponentially, and larger genome sizes (e.g. |gt| 20) can take a significant amount of time. After testing the fitness for every possible rearrangement of bits, it outputs the results to a file with the base name TREvoSim_fitness_histogram in the save path defined in the toolbar. For all genome lengths, this text file includes the settings and a histogram of the number of fitnesses for each potential fitness. For genome sizes below 25, it also includes a full list of every genome for every fitness level. Note that the lower the number assigned by the fitness algorithm, the fitter the organism is. 

Run tests
^^^^^^^^^

TREvoSim includes a test mode, which is launched by clicking the Tests button on the main toolbar, this menu command, or the shortcut ctrl + shift + t (see :ref:`tests` )

Set uninformative factor
^^^^^^^^^^^^^^^^^^^^^^^^

This menu command (or shortcut ctrl + shift + f) opens a dialogue that allows the strip uninformative factor to be set manually (see :ref:`Simulations` - Strip uninformative characters via subsampling).

Recalculate uninformative factor for current settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The multiplication factor required to achieve a set number of informative characters via subsampling depends on the settings of any given run. Selecting this option (or ctrl + shift + r) calculates and sets this factor empirically after the settings dialogue is closed by conducting a ten-run batch and working out the proportion of informative characters within those runs. This needs to be recalculated after any settings are changed. If it is not, or has not been set, the software will run with a large factor, and thus be slower than necessary.

Restore default settings
^^^^^^^^^^^^^^^^^^^^^^^^

If at any point you require default settings, this menu option (or shortcut ctrl + shift + d) restores all settings to default.