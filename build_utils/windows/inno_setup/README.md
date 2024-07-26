# TREvoSim - Inno Setup
## [Tr]ee [Evo]lutionary [Sim]ulator program

Coded by:
 - Russell J. Garwood (russell.garwood@gmail.com)

GUI theme, and Inno guidance by:
 - Alan R.T. Spencer (alan.spencer@imperial.ac.uk)

______


## Relevant references:
Keating, J.N., Sansom, R.S., Sutton, M.D., Knight, C.G. and Garwood, R.J. 2019. Morphological phylogenetics evaluated using novel evolutionary simulations.

_____

CONTENTS:

1. Creating Windows Installer Program - Inno Setup

_____

## 1. Creating Windows Installer Program - Inno Setup

Inno Setup is a free installer for Windows programs by Jordan Russell and Martijn Laan. First introduced in 1997, Inno Setup today rivals and even surpasses many commercial installers in feature set and stability.

The program can be downloaded from: [http://jrsoftware.org/isinfo.php](http://jrsoftware.org/isinfo.php)

The file TREvoSiminstaller.iss contained in this folder should be modified and used to create the Windows Installation Binary. It expects all support .DLLs and the .EXE to be placed in a ./bin folder. The Inno Setup will create the new install binary in a folder called ./build

The TREvoSiminstaller.iss should be altered with the latest version number etc... before being run.

_____


t:@palaeoware

e:palaeoware@gmail.com

w:https://github.com/palaeoware
