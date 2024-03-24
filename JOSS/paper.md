---
title: 'TREvoSim v3: An individual based simulation for generating trees and character data'
tags:
  - evolution
  - simulation
  - phylogenetics
  - ecology
  - palaeontology
authors:
  - name: Russell J. Garwood
    orcid: 0000-0002-2803-9471
    affiliation: "1, 2" # (Multiple affiliations must be quoted)
    corresponding: true # (This is how to denote the corresponding author)
  - name: Alan R. T. Spencer
    orcid: 0000-0001-6590-405X
    affiliation: "2,3"
  - name: Charles T. M. Bates
    orcid: 0009-0000-8444-6322
    affiliation: 1
  - name: Leah M. Callender-Crowe
    orcid: 0000-0002-5870-1964
    affiliation: 4
  - name: Frances S. Dunn
    orcid: 0000-0001-7080-5283
    affiliation: 5
  - name: Thomas J.D. Halliday
    orcid: 0000-0002-4077-732X
    affiliation: 6
  - name: Joseph N. Keating
    orcid: 0000-0002-2667-7795
    affiliation: 7
  - given-names: Nicolás
    surname: Mongiardino Koch
    orcid: 0000-0001-6317-5869
    affiliation: 8
  - name: Luke A. Parry
    orcid: 0000-0002-3910-0346
    affiliation: 9
  - name: Robert S. Sansom
    orcid: 0000-0003-1926-2556
    affiliation: 1
  - name: Thomas J. Smith
    orcid: 0000-0002-2083-7452
    affiliation: 9
  - name: Mark D. Sutton
    orcid: 0000-0002-7137-7572
    affiliation: 3
  - name: Thomas Vanteghem
    orcid: 0009-0003-3513-8448
    affiliation: 10

affiliations:
 - name: Department of Earth and Environmental Sciences, University of Manchester, Manchester, M13 9PL, UK
   index: 1
 - name: Natural History Museum, London, SW7 5BD, UK
   index: 2
 - name: Department of Earth Science and Engineering, Imperial College, London, SW7 2AZ, UK
   index: 3
 - name: School of Biological Sciences, University of Reading, Reading, UK
   index: 4
 - name: Oxford University Museum of Natural History, University of Oxford, Oxford, OX1 3PW, UK
   index: 5
 - name: School of Geography, Earth and Environmental Sciences, University of Birmingham, Birmingham B15 2TT, UK
   index: 6
 - name: School of Earth Sciences, University of Bristol, Life Sciences Building, Tyndall Avenue, Bristol, BS8 1TQ, UK
   index: 7
 - name: Scripps Institution of Oceanography, UC San Diego, La Jolla, CA 92122, USA
   index: 8
 - name: Department of Earth Sciences, University of Oxford, Oxford, OX1 3AN, UK
   index: 9
 - name: Erasmus Mundus Joint Master Degree PANGEA, Lille University, France
   index: 10
date: 24 March 2024
bibliography: paper.bib

---

# Summary
Simulations provide valuable insights into the patterns and processes of evolution, and the performance of analytical methods used to investigate empirical data
[@Wright_Hillis_2014; @Barido-Sottani_Saupe_Smiley_Soul_Wright_Warnock_2020; @Dolson_Ofria_2021]. This is particularly true for building phylogenies (evolutionary trees) where the true relationships between groups are unknowable in empirical settings and using empirical data.  Simulations can create a known phylogenetic tree alongside associated character data, and therefore play a powerful role in assessing the adequacy of approaches for inferring trees from character data. Here we present TREvoSim v3.0.0: an individual-based model written in C++, with a focus on creating phylogenetic character data and trees. Previous versions have been used to study phylogenetic inference methods [@Keating_Sansom_Sutton_Knight_Garwood_2020; @Mongiardino Koch_Garwood_Parry_2021; @Mongiardino Koch_Garwood_Parry_2023]; the v3.0.0 release adds a range of new features that allow the study of evolutionary processes, in addition to phylogenetic methods.  

# Background 
TREvoSim v1.0.0 was developed to investigate the accuracy and precision of phylogenetic inference methods [@Keating_Sansom_Sutton_Knight_Garwood_2020]. After further development, TREvoSim v2.0.0 was used to investigate the impact fossils have on phylogenetic inference and evolutionary timescales [@Mongiardino Koch_Garwood_Parry_2021; @Mongiardino Koch_Garwood_Parry_2023]. In brief, TREvoSim is a non spatially-explicit model in which organisms — which consist of a genome of binary characters — compete within a structure called the playing-field to echo natural selection (Figure 1). Their chance of replication is dictated by a fitness algorithm that assesses organismal fit against a series of random numbers (masks, constituting an environment). On replication, organisms have a chance of mutation, and descendents overwrite a current member of the playing field. The simulation has a lineage-based species concept, and at the end of a simulation can output trees and characters (species genomes), as well as logging the simulation state as the model runs.

![Figure 1 - A simplified overview of TREvoSim. Green text represents a user-defined variable and the given value is default. Top left are data structures, and the algorithm employed in simulations is shown top right. Bottom shows how a tree is built: by default genomes for each species are recorded on their extinction. A full description of the algorithm is available in Keating et al. (2020) . ](./Figure_01.png)

# New features

TREvoSim v3.0.0 includes a suite of new features that allow the investigation of an expanded range of evolutionary processes. These new features are briefly introduced (in alphabetical order) below, and fully described in the TREvoSim documentation [TREvoSim documentation](https://trevosim.readthedocs.io/en/latest/).

## Character options

New options allow finer control of TREvoSim functions that employ genome characters. In addition to a user-defined number of total characters, a separate limit on character count used for species selection and/or the fitness calculation can now be applied (in the default settings, all are the same). When this differs from the total character number, only a subset of characters (those from zero to the limit) are included in the defined operations, and others can evolve independent of these processes (i.e., in the absence of selective forces, akin to more neutral drift-like processes).  

## Default simulation parameters

New default values for simulation parameters are introduced with this release. These are benchmarked against three properties of 12 empirical, total evidence analyses (following the approach of Mongiardino Koch et al. 2021). The empirical data, analysis script, and resulting graphs have been placed in the folder Benchmarking within the source code repository.

## Ecosystem engineering

A new ecosystem engineering system allows organism-environment feedback to be investigated. When the ecosystem engineering option is enabled (it is disabled in default settings), a species is assigned ecosystem engineering status halfway through a run, and passes this status to descendents. When this occurs, the genome of that organism is either used to overwrite an environmental mask, or added to the environment as an additional mask. This can occur once (‘one-shot’ ecosystem engineering) or repeatedly (‘persistent’) after this point. When a mask is added in the first application, it is overwritten in subsequent applications when ecosystem engineers are persistent. A new facility to log the ecosystem-engineering status of individuals is provided. 

## Expanding playing field 

This new option alters competition such that it occurs between species rather than between individuals. When enabled (disabled by default), only one individual from each species is present in the playing field at any time, and this grows to accommodate new species following the standard speciation rules. On duplication, juveniles overwrite the previous member of their species in the playing field. This removes intra-species competition from the simulation. 

## Logging

A new logging system has been added that allows the user to create a customised file recording many aspects  of the simulation state — for example, the tree and the character data — during a run (previous versions only supported outputs at the end of a simulation). The user can define all log outputs using the logging options. 

## Match peaks

When there are multiple environments for any given playing field, by default masks are seeded with random numbers, and may thus have different peak fitness values. This new option instead seeds each playing field with environments that have the same peak fitness. Additionally, it uses a heuristic algorithm to generate an initial organism that has the same fitness in each environment (in >99% of cases). This option allows finer control of the fitness landscape. 

## No selection

Another new addition is a no selection mode — when this is enabled, organisms for replication are chosen from the playing field at random, rather than using fitness to determine replication probability. When this option is enabled, the simulation functions under drift. 

## Playing field mixing

By default TREvoSim playing fields are independent data structures, and organisms in one playing field do not compete with those in others during a simulation. This new option allows configurable mixing of organisms between playing fields, which can be asymmetrical if desired. Playing field mixing  can facilitate the study of, for example, the dynamics of invasive species or biotic interchanges. 

## Perturbations

This implements a limited period of increased rates of environmental change that occurs halfway through a run (when half the requested species have evolved, or at half the requested iteration number). This is intended for study of scenarios where evolutionary dynamics are driven by variations in the rate of environmental change. There is an option to also increase mixing between playing fields during a perturbation. 

## Simulation modes

Simulation-termination can now be configured to occur after either a user-defined number of iterations, or when a user-defined number of species have evolved; only the latter was previously available. This provides increased flexibility in experimental design. 

## Stochastic layer

Provides a layer of abstraction between an organism’s genome and the bits used for the fitness calculation. It achieves this using many-to-one mapping, the map being defined by the user. This feature removes direct control of fitness from the organism genome, allowing e.g. neutral mutations and less strongly adaptationist dynamics

## Codebase Tests

TREvoSim v3.0.0 introduces a test suite covering all aspects of the simulation mechanics. These are included in the build pipeline, which will fail if any tests return a failure. 

# Statement of need

Typically, phylogenetic simulations are conducted using deterministic or stochastic approaches [e.g. @Puttick_O'Reilly_Pisani_Donoghue_2019; Guillerme_Puttick_Marcy_Weisbecker_2020], such as birth-death models or randomly generated data. TREvoSim complements these by using a selection-driven, agent-based approach: the data generated are different in a number of ways to those created using a stochastic model (Keating et al. 2020). The data generated by the software is likely to violate the assumptions of many common models used in the process of phylogenetic inference, incorporating a level of model misspecification resembling that expected from empirical datasets. Default settings have also been validated to reflect a number of features of empirical data matrices and trees. Given that (true) phylogenetic trees and character data are an emergent property of the simulation, the software is particularly well suited to simulation studies that can be analysed through phylogenetic trees and character data matrices. These include, for example: the impact of missing data on phylogenetic inference; the impact of rates of environmental change on character evolution; and the nature of evolution under different fitness landscapes. 

# Current associated projects 

In addition to the published studies, cited previously, future directions include:
TJS, FSD, LAP, RJG - The Macroevolutionary consequences of ecosystem engineering. 
CTMB, TJS, LAP, RJG, FSD - Fitness landscapes and disparity.
TJDH, RJG - Mixing and perturbations.
RSS, ARTS, RJG, JNK - Impact of character number and correlated characters in a phylogenetic context.
NMK, LAP, RJG - Phylogenetic method development.
JNK, RJG - Impact of evolutionary mode on phylogenetic character data.
MDS, RJG - Fossilisation and phylogenetic inference.

# Author contributions

RJG developed and coded TREvoSim, with support on testing and releasing from ARTS. RSS, LMC-C, MDS, and JNK contributed ideas during the initial phases and continued development of the software, and JK proposed the stochastic layer. Peak matching and associated features were developed in collaboration with CTMB, FSD, LAP and TJS, and ecosystem engineering was developed in collaboration with LAP, FSD and TJS. Mixing and perturbations were proposed by TJDH. Development of many of the features released in v2.0.0 was conducted in collaboration with, and empirical benchmarking was led by, NMK. The expanding playing field was developed with and analysed by TV. 

# Availability 
##TODO
TREvoSim v3.0.0 source code and binaries are freely available from [Zenodo](https://doi.org/10.5281/zenodo.10866261) and [GitHub](https://github.com/palaeoware/trevosim). Newer releases of the REvoSim software will be available on GitHub. Fully documentation is available from [ReadTheDocs](https://trevosim.readthedocs.io/en/latest/).

# Acknowledgements 

RJG was supported by the NERC award NE/T000813/1 and the Alexander von Humboldt Foundation. Development began during the BBSRC grant BB/N015827/1 awarded to RSS and RJG. FSD is supported by the NERC fellowship NE/W00786X/1.

# References