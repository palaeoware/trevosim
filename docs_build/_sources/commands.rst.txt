.. _commands:
.. include:: <isonum.txt>

Menu commands
=============

TREvoSim has a series of options which can be selected using the Commands menu at the top left of the program (above the run button). These are as follows.

Save Current Settings
---------------------

By default TREvoSim saves the simulation settings between sessions (it writes a settings file to the same folder as the software binary / executable on close). This menu command (or shortcut ctrl + s) rewrites that setting file without closing the program.


Save settings as...
-------------------

This opens a file save dialogue, and then writes a TREvoSim settings file to the requested location (shortcut ctrl + shift + s). TREvoSim settings files are written in an XML document that can be opened in any text application.

Load settings from file...
--------------------------

Load settings file (ctrl + o) opens a saved settings file and updates the simulation settings accordingly.

Restore default settings
------------------------

If at any point you require default settings, this menu option (or shortcut ctrl + shift + d) restores all settings to default.

Fitness histogram
-----------------

The fitness algorithm for TREvoSim is described in Keating et al. (2020). The fitness landscape varies depends on the simulation settings. This menu command (shortcut ctrl + shift + h) provides a basic assessment of that landscape by showing the distribution of fitnesses for every possible genome for the current settings. It provides an option to select how many bits in the test organism's genome as larger genome sizes (e.g. |gt|16) take exponentially longer given the need to try every potential genome rearrangement.

Run tests
---------

TREvoSim includes a test mode, which is launched by clicking the Tests button on the main toolbar, this menu command, or the shortcut ctrl + shift + t (see :ref:`tests` )

Set uninformative factor
------------------------

This menu command (or shortcut ctrl + shift + f) opens a dialogue that allows the strip uninformative factor to be set manually.

Random Seed
-----------

By default, the organism used to initialise a simulation is one near peak fitness for the starting environment(s), to prevent the resulting tree from documenting a lineage adapting to a fitness peak (this results in a highly asymmetrical tree). This menu command (shortcut ctrl + shift + r) toggles between this approach, and one in which a random individual is used to initialise the simulation.
