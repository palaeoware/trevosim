.. _commandline:

Command Line Options
==================

REvoSim provides command line options to enable full control of simulations from command line or batch file environments. Every parameter configurable in the GUI can be set from the commandline, and the program can be instructed to execute a simulation and then exit.

This document does not provide instructions on how to write a batch file, or determine correct paths - if you don't know how to do this, you should probably not be looking into command line options in the first place.

39 command line switches are provided; these are documented below, and examples are given of their usage. Any combination of switches in any order is allowed. Note that on some systems, path names with spaces may cause problems; these are best avoided, but alternatively paths can be encased in double quotes if they contain a space.

Note in particular the --auto command, which automatically starts a simulation, and exits the program at it's end. This will be required in most batch scenarios, as will the -k switch to turn on one or both types of logging.

e.g. a command line to run a similation on the environment files in "c:\\revosim\\data\\env1" for 50000 iterations, using normal (running) logs, and with settings at defaults except for 'toroidal' (on):

c:\\revosim\\revosim.exe -e c:\\revosim\\data\\env1 -t=On -k=Normal --auto=50000


Single-letter switches
----------------------
These can be used as either in single letter form (-a) or long form (--startage - note the double -). All require a value, which can be separated from the switch using a space or an equals - or in single character mode, without a separator at all (this doesn't work in -- long form, where a separator IS required). Where no parameter is noted in square brackets this is a boolean option, which is either On or Off (you can also use 1/0, true/false, yes/no, y/n - which are not case sensitive). e.g.

- -a30
- -a=30
- -a 30
- --startage=30
- --startage 30

for a boolean:
- -hOn
- -h 1
- -h=truE
- -hY
- --sexual False
- --sexual=Y

Full list of single-letter switches:
- -a --startage [int]
- -b --breedthreshold [int]
- -c --breedcost [int]
- -d --maxdifftobreed [int]
- -e --environment [path without trailing /]  - folder containing environment files. All image files in this folder are used
- -f --usemaxdifftobreed
- -g --breedwithinspecies
- -h --sexual
- -i --dispersal [int]
- -j --outputpath [path without trailing /] 
- -k -logtype [Phylogeny|Normal|Both] - which log types to turn on. Not case sensitive.
- -l --excludenodescendents
- -m --environmentmode [Static|Once|Loop|Bounce]
- -n --energy [int] - energy input
- -o --tolerance [int]  - settle tolerance
- -p --phylogeny [Off|Ba:sic|Phylogeny|Metrics] - you can also use None for Off. Not case sensitive.
- -q --recalcfitness
- -r --refreshrate [integer] - environment refresh rate
- -s --slots [integer]
- -t --toroidal
- -u --mutation [int]
- -v --csv
- -w --interpolate  
- -x --gridx [integer]
- -y --gridy [integer]
- -z --moredata - the 'more data in logs' switch

Long option only switches
-------------------------
We ran out of letters! These require the long format, with --. Otherwise they work as above.
- --polling [int]
- --auto [int] - set to 0 for don't terminate (though it WILL terminate if m is set to Once)
- --nonspatial
- --minspeciessize [int]
- --fitnesstarget [int]

Image logging switches
----------------------
These are a set of long-option only switches to control image logging. They are all booleans.
- --li_population
- --li_fitness
- --li_coding
- --li_noncoding
- --li_species
- --li_genes
- --li_settles
- --li_fails
- --li_environment

[Formatting not working - will fix later]
