# TREvoSim
## [Tr]ee [Evo]lutionary [Sim]ulator Program

Code by:
 - Russell J. Garwood (russell.garwood@gmail.com).

GUI theme and test automation by:
 - Alan R.T. Spencer (alan.spencer@imperial.ac.uk).

<p align="center">
  <img width="250" height="250" src="./resources/palaeoware_logo_square.png">
</p>

_____

## Relevant references:
Garwood, R.J., Spencer A.R.T. and Sutton, M.D., 2019. REvoSim: Organism-level simulation of macro- and microevolution. Palaeontology 62(3),339-355. [DOI: 10.1111/pala.12420](https://doi.org/10.1111/pala.12420) (describes fitness algorithm)

Keating, J.N., Sansom, R.S., Sutton, M.D., Knight, C.G., and Garwood, R.J. 2020. Morphological phylogenetics evaluated using novel evolutionary simulations. Systematic Biology 69(5):897-912. [DOI: 10.1093/sysbio/syaa012](https://doi.org/10.1093/sysbio/syaa012) (describes, and utilises, TREvoSim v1.0.0)

Mongiardino Koch, N., Garwood, R.J. and Parry, L.A. 2021. Fossils improve phylogenetic analyses of morphological characters. Proceedings of the Royal Society B: 288(1950):20210044. [DOI: 10.1098/rspb.2021.0044](https://doi.org/10.1098/rspb.2021.0044) (describes, and utilises, TREvoSim v2.0.0)

Mongiardino Koch, N., Garwood, R.J. and Parry, L.A. 2022. Inaccurate fossil placement does not compromise tip-dated divergence times. bioRxiv. [DOI: 10.1101/2022.08.25.505200](https://doi.org/10.1101/2022.08.25.505200) (utilises TREvoSim v2.0.0) 

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

All TREvoSim code is copyright 2023 by Russell J. Garwood and Alan R.T. Spencer.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but **without any warranty**.
______

## 2. Installation

TREvoSim may be compiled using QT from the source code or installed/run from the binary releases on our [GitHub](https://github.com/palaeoware).

If you wish to compile TREvoSim on your system this requires the [Qt library](https://www.qt.io). The software has been tested with Qt versions 5.6-5.15 (v1,2), and 6.2-6.5 (v3). Versions 1 and 2 should be compatible with all Qt versions greater than 5.3, v3 wil compile with Qt versions >6.0. TREvoSim has been tested on Windows, Mac OS and Ubuntu Linux, but will in theory work on any Qt-capable system.

To compile (a) install Qt, (b) open the .pro file in Qt Creator, and (c) specify build kit. 
_____

## 3. Usage

Descriptions of the algorithms used in this package are included in the reference above. Full documentation is available on [readthedocs](https://trevosim.readthedocs.io/en/latest/).

### Version 1

To start a run press the play button in the toolbar. Runs can be paused (pause button), stopped (escape key), and reset if required (reset button). Outputs will be written to a new folder created in the *Save path* text box on the toolbar. Batch mode will complete *n* runs, labelling the output files with the run number. Both the settings and output buttons launch new dialogues for user input.

#### Settings - Organisms and simulation

Number of characters/genome size: The size of the genome for the digital organisms in the simulation, which is also the number of characters in the matrix when simulating cladistic matrices.

Rate of organism mutation: Average mutations per hundred bits in the organism genome.

Taxon number: The simulation is run until this number of species have evolved.

Species difference: The number of bits an organism is from either its parent, or - if this is not the first speciation in the lineage the last species to diverge - before an organism is considered a new species (Note - Comparison to last speciation rather than parent prevents bursts of speciation from closely related individuals sharing a common parent). A value of around 10% the size of the genome size is generally suitable.

Unresolvable cut off: TREvoSim will discard runs with more unresolvable (i.e. identical) taxa above this number.

Sansomian speciation: Species genomes can either be recorded when a species is born, or they can be recorded when a species goes extinct. Sansomian speciation is the latter, and is by default enabled in TREvoSim (this is because under some settings a species can give rise to many others, and for simulation studies to provide sensible results its genome should be most closely related to that which most recently speciated).

Discard deleterious mutations: With this setting enabled a check is conducted each iteration after mutations are applied to the duplicating organism and if that organism is less fit than it was before the mutations are discarded.

Strip uninformative characters via subsampling: If this option is selected, the software will generate *s* (s=strip uninformative factor, see below) more characters than required, and then subsample these to the requested character number. Note: this will multiply not just character number by *s*, but also the species difference, but not the unresolvable cut off. Runs which have not achieved the requested number of characters will be discarded.

Recalculate multiplication factor on dialogue close: The factor by which characters need to be over-generated (*s*) for enough informative characters to exist and meet the requested number relies upon the species difference, mutation rate, and a host of other factors within TREvoSim. Rather than setting *s* to be a very high number, and thus necessitating long runs, TREvoSim can recalculate this factor on the close of the settings dialogue. If this option is selected, ten runs will occur on dialogue close and a new *s* will be calculated for the chosen settings. Note that further changes in settings will require that *s* is recalculated.

#### Settings - Environment

Rate of environmental mutation: Average mutations of each environmental mask per hundred bits.

Number of masks: This defines the number of masks in the environment, and ultimately the fitness landscape. To a first approximation, the higher the mask number, the flatter the fitness landscape.

Playing field size: This is the size of the playing field. The smaller the playing field, the shorter species lifespan, and higher species turnover.

#### Output

TREvoSim provides a system by which two different customised log files can be created for any given run. Simply open the output tab, and type the text required for output files into the editor provided. The software allows you to define the file extension (although all outputs are plain text formatted using your OS conventions) and filename. There are a number of commands allowing you to customise the output in each run, with the format ||Keyword||. Depending on the keyword, these are replaced with the matrix, tree, or a number of other options, all listed in the pop up.

Output nex tree file: This will output a .nex tree file in standard formatting for any run.

Output working log: This outputs a text file reporting the status of TREvoSim at different points during each iteration and outlining the actions it is taking throughout. This can be used to check the software is working as intended, and record a fill history of runs, but be aware that with all but the smallest runs it will create a sizeable text file.
_____

## 4. Contributing to the code

Contribution to the code in this repository is welcome. Please read the [Repository Contribution](https://github.com/palaeoware/repoconventions)  document for a guide to our coding style and repository structure before submitting any code updates.
_____

## 5. Bug reporting and feature requests

Bug reporting and feature requests should be made through the [GitHub Issues](../../issues) page for this repository. We will aim to respond to all issues and feature requests in a timely manner.

t:@palaeoware

e:palaeoware@gmail.com

w:https://github.com/palaeoware
