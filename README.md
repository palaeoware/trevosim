# TREvoSim
## [Tr]ee [Evo]lutionary [Sim]ulator program

This package is distributed for review purposes only. Not for redistribution until publication.

Coded by:
 - Russell J. Garwood (russell.garwood@gmail.com)

GUI theme by:
 - Alan R.T. Spencer (alan.spencer@imperial.ac.uk)

<p align="center">
  <img width="250" height="250" src="./resources/palaeoware_logo_square.png">
</p>

_____

## Relevant references:
Keating, J.N., Sansom, R.S., Sutton, M.D., Knight, Chris G., and Garwood, R.J. 2020. Morphological phylogenetics evaluated using novel evolutionary simulations. Systematic Biology.

_____


## Contents:

1. Copyright and Licence
2. Installation
3. Usage
4. Contributing to the code
5. Bug reporting and feature requests

______

## 1. Copyright and Licence

All TREvoSim code is released under the GNU General Public License. See LICENSE.md files in the programme directory.

All TREvoSim code is Copyright 2018 by Russell J Garwood and Alan R.T. Spencer.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but **without any warranty**.
______

## 2. Installation

TREvoSim may be compiled using QT from the source code or installed/run from the binary releases on our [GitHub](https://github.com/palaeoware).

If you wish to compile TREvoSim on your system this requires the [Qt library](https://www.qt.io). The software has been tested with Qt versions 5.6-5.10 and should be compatible with all Qt versions greater than 5.3. It has been tested on Windows, Mac OS and Ubuntu Linux, but will in theory work on any Qt-capable system.

To compile (a) install Qt, (b) open the .pro file in Qt Creator, and (c) specify build kit. TREvoSim includes random numbers in the executable, but alternatives can be loaded through the menus as required. The package uses a GUI theme designed by Alan R.T. Spencer, and implemented by Russell J. Garwood.
_____

## 3. Usage

Descriptions of the algorithms used in this package are included in the reference above.

### Version 1

To start a run press the play button in the toolbar. Runs can be paused (pause button), stopped (escape key), and reset if required (reset button). Outputs will be written to a new folder created in the *Save path* text box on the toolbar. Batch mode will complete *n* runs, labelling the output files with the run number. Both the settings and output buttons launch new dialogues for user input.

#### Settings - Organisms and simulation

Number of characters/genome size: The size of the genome for the digital organisms in the simulation, which is also the number of characters in the matrix when simulating cladistic matrices.

Rate of organism mutation: Average mutations per hundred bits in the organism genome.

Taxon number: The simulation is run until this number of species have evolved.

Species difference: The number of bits an organism is from either its parent, or - if this is not the first speciation in the lineage the last species to diverge - before an organism is considered a new species (Note - Comparison to last speciation rather than parent prevents bursts of speciation from closely related individuals sharing a common parent). A value of around 10% the size of the genome size is generally suitable.

Unresolvable cutoff: TREvoSim will discard runs with more unresolvable (i.e. identical) taxa above this number.

Sansomian speciation: Species genomes can either be recorded when a species is born, or they can be recorded when a species goes extinct. Sansomian speciation is the latter, and is by default enabled in TREvoSim (this is because under some settings a species can give rise to many others, and for simulation studies to provide sensible results its genome should be most closely related to that which most recently speciated).

Discard deleterious mutations: With this setting enabled a check is conducted each iteration after mutations are applied to the duplicating organism and if that organism is less fit than it was before the mutations are discarded.

Strip uninformative characters via subsampling: If this option is selected, the software will generate *s* (s=strip uninformative factor, see below) more characters than required, and then subsample these to the requested character number. Note: this will multiply not just character number by *s*, but also the species difference, but not the unresolvable cutoff. Runs which have not achieved the requested number of characters will be discarded.

Recalculate multiplication factor on dialogue close: The factor by which characters need to be overgenerated (*s*) for enough informative charaters to exist and meet the requested number relies upon the species difference, mutation rate, and a host of other factors within TREvoSim. Rather than setting *s* to be a very high number, and thus necessitating long runs, TREvoSim can recalculate this factor on the close of the settings dialogue. If this option is selected, ten runs will occur on dialogue close and a new *s* will be calculated for the chosen settings. Note that further changes in settings will require that *s* is recalculated.

#### Settings - Environment

Rate of environmental mutation: Average mutations of each environmental mask per hundred bits.

Number of masks: This defines the number of masks in the environment, and ultimately the fitness landscape. To a first approximation, the higher the mask number, the flatter the fitness landscape.

Playing field size: This is the size of the playing field. The smaller the playing field, the shorter species lifespan, and higher species turnover.

#### Output

TREvoSim provides a system by which two different customised log files can be created for any given run. Simply open the output tab, and type the text requried for output files into the editor provided. The software allows you to define the file extension (although all outputs are plain text formatted using your OS conventions) and filename. There are a number of commands allowing you to customise the output in each run, with the format ||Keyword||. Depending on the keyword, these are replaced with the matrix, tree, or a number of other options, all listed in the pop up.

Append: with this option selected, batches or subsequent runs of TREvoSim appended results to File One and File Two, rather than creating a separate file for every run.

Output nex tree file: This will output a .nex tree file in standard formatting for any run.

Output working log: This outputs a text file reporting the status of TREvoSim at different points during each iteration and outlining the actions it is taking throughout. This can be used to check the software is working as intended, and record a fill history of runs, but be aware that with all but the smallest runs it will create a sizable text file.
_____

## 4. Contributing to the code

Contribution to the code in this repository is welcome. Please read the [Repository Contribution](https://github.com/palaeoware/repoconventions)  document for a guide to our coding style and repository structure before submitting any code updates.
_____

## 5. Bug reporting and feature requests

Bug reporting and feature requests should be made through the [GitHub Issues](../../issues) page for this repository. We will aim to respond to all issues and feature requests in a timely manner.

t:@palaeoware

e:palaeoware@gmail.com

w:https://github.com/palaeoware
