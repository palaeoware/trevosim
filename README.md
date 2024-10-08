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

Mongiardino Koch, N., Garwood, R.J. and Parry, L.A. 2023. Inaccurate fossil placement does not compromise tip‐dated divergence times. Palaeontology, 66(6): e12680.  [DOI: 10.1111/pala.12680](https://doi.org/10.1111/pala.12680) (utilises TREvoSim v2.0.0) 

Garwood, R.J., Spencer, A.R.T., Callender-Crowe, L.M., Dunn, F.S., Halliday, T.J., Keating, J.N., Koch, N.M., Parry, L.A., Sansom, R.S., Smith, T.J. and Sutton, M.D. 2024. TREvoSim v3: An individual based simulation for generating trees and character data. Journal of Open Source Software, 9(101):6722. [DOI: 10.21105/joss.06722](https://doi.org/10.21105/joss.06722) (describes TREvoSim v3.0.0)

Smith, T.J., Parry, L.A., Dunn, F.S. and Garwood, R.J. 2024. Exploring the macroevolutionary impact of ecosystem engineers using an individual‐based eco‐evolutionary simulation. Palaeontology, 67(5): e12701. [DOI: 10.1111/pala.12701](https://doi.org/10.1111/pala.12701)

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

All TREvoSim code is copyright 2024 by Russell J. Garwood and Alan R.T. Spencer. Please see the [Repository Contribution](https://github.com/palaeoware/repoconventions) document for further information on copyright. 

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but **without any warranty**.
______

## 2. Installation

TREvoSim can be installed and installed/run from the binary releases on our [GitHub](https://github.com/palaeoware): see the page [releases](https://github.com/palaeoware/trevosim/releases) for the latest release. Installation instructions can be found in the documentation (linked below).

If you wish to modify the software, and thus compile TREvoSim on your system this requires the [Qt library](https://www.qt.io). The software has been tested with Qt versions 5.6-5.15 (v1,2), and 6.2-6.5 (v3). Versions 1 and 2 should be compatible with all Qt 5 versions greater than 5.3, v3 will compile with Qt versions >6.0. TREvoSim has been tested on Windows, Mac OS and Ubuntu Linux, but will in theory work on any Qt-capable system. Build guides are provided in the documentation.
_____

## 3. Documentation

Full descriptions of the algorithms used in this package, and all simulation variables, as well as guidance on experimental design and data analysis are included in the documentation on [readthedocs](https://trevosim.readthedocs.io/en/latest/).

_____

## 4. Contributing to the code

Contribution to the code in this repository is welcome. Please read the [Repository Contribution](https://github.com/palaeoware/repoconventions) document for information on our licencing, code of conduct, coding style, and repository structure before submitting any code updates.

We are grateful to the following for contributing code to TREvoSim:

- Martin R Smith (@ms609)

_____

## 5. Bug reporting and feature requests

Bug reporting and feature requests should be made through the [GitHub Issues](../../issues) page for this repository. We will aim to respond to all issues and feature requests in a timely manner.

e:palaeoware@gmail.com

w:https://github.com/palaeoware
