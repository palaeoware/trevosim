## If packages aren't installed, install them, then load them
packages <- c("cli", "phangorn", "phytools", "phylobase", "Claddis", "tidyverse", "treestats", "TreeTools")
# the automatic package installation is nice. vapoRwave is not in CRAN,
# so presumably will need installing from moldach/vapoRwave 
# (which would require a user to set up devtools, build tools, etc).
# If this is just used for colour palettes, might it be simpler to just use
# inbuilt R palettes from e.g. hcl.pals()?
if(length(packages[!packages %in% installed.packages()[,"Package"]]) > 0){
  install.packages(packages[!packages %in% installed.packages()[,"Package"]])
}

library(phangorn)
library(phytools)
library(phylobase)
library(Claddis)
library(tidyverse)
library(vapoRwave)
library(treestats)
library(TreeTools)

#Clear environment
#rm(list = ls()) # This is generally discouraged, in case the user has a 
# precious variable saved in their environment and accidentally deletes it.


#If not running from the included bash script, set working directory here
#To run on the default outputs, this should be the absolute path to the folder comparison_to_empirical_data within the TREvoSim repository
#Otherwise you will need to change some or all of the details below to point to the relevant folders
#setwd("########################")

########################################################################################################################

# This script analyses TREvoSim runs, and then compares them against empirical datasets.
# It was written by Nicolás Mongiardino Koch and Russell Garwood, with an extra steps function courtesy of Joe Keating.
# The empirical datasets are sourced from this paper:
# Mongiardino Koch, N., Garwood, R.J. and Parry, L.A., 2021. Fossils improve phylogenetic analyses of morphological characters. Proceedings of the Royal Society B, 288(1950), p.20210044.
# The above paper cites the source of each dataset used for the analysis.

########################################################################################################################

#Directory with empirical data:
empiricalWD <- "./Empirical_data"
#Directory with TREvoSim outputs in it:
sourceWD <- "./TREvoSim_output_defaults_2024"
#Directory to output graphs
outputWD <- "./Graphs_out/"

#You can use the following switches to determine whether it:
###Counts the number of extant taxa
countExtant <- FALSE
###Counts the number of parsimony steps on the true tree
countSteps <- TRUE
###Calculates the treeness of the tree
calcTreeness <- TRUE
###Calculates the tree symmetry
calcTreeshape <- TRUE

#Not in operator
`%not in%` <- function(x, table) is.na(match(x, table, nomatch = NA_integer_))

########################################################################################################################
#Function to calculate treeness
#i.e. sum of internal branch lengths/total branch lengths
treeness <- function(tree) {
  # `which` and `c` are redundant here; possibly this is more readable?
  return(1 - (sum(tree$edge.length[tree$edge[, 2] <= length(tree$tip.label)])/sum(tree$edge.length)))
}

# doFitMK is unused

########################################################################################################################
#Function to compute the number of excess steps for each character of a binary morphological character matrix.
#Constant characters will return NA.
#tree = object of class 'phylo'.
#mdata = binary character matrix of class 'list', e.g. as produced by the 'read.nexus.data' function

#tree<-simTree
#mdata<-read.nexus.data(matrixFiles[1])
#tree<-read.nexus("/home/russell/Desktop/TREvoSim_output_unrooted/TREvoSim_tree_000.nex")
#mdata<-read.nexus.data("/home/russell/Desktop/TREvoSim_output_unrooted/TREvoSim_000.nex")
# As `steps` is used as a variable name in other functions, it would reduce
# ambiguity to give this function a distinct name
excessSteps <- function(tree, mfile) {
  mdata <- read.nexus.data(mfile)
  mphy <- phyDat(mdata, type = "USER", levels = unique(unlist(mdata)))
  matt <- attributes(mphy)
  
  # Subtracting one led to negative scores for invariant characters.
  # Instead subtract minumum steps.
  # Taking a character at a time within vapply is also more efficient,
  # speeding this bottleneck
  xs <- vapply(seq_len(matt[["nr"]]), function (z) {
    phy_char <- mphy[, z]
    nStates <- length(unique(as.character(phy_char)))
    minSteps <- nStates - 1L
    parsimony(tree, phy_char) - minSteps
  }, double(1))
  
  if (any(xs < 0)) {
    # Vectorised version of the below
    xs[xs < 0] <- NA
    # Is there a legitimate reason to see negative steps?
    stop("Minimum steps incorrectly calculated")
  }
  
  char_results <- data.frame(steps = xs[matt[["index"]]])
  
  for (i in 1:length(char_results$steps)) if (char_results$steps[[i]] < 0) {
    char_results$steps[[i]] <- NA # I think NA is meant: the string "NA" coerces
                                  # $steps to mode 'character'
  }
  return(char_results)
}

########################################################################################################################
#Function to compute rates (extra steps) and treeness for empirical characters
empiricalTreeness <- function() {
  #Get empirical data
  trees <- list.files(empiricalWD, pattern = ".tre", full.names = TRUE)
  nTrees <- length(trees)
  if (!nTrees) {
    # To help a user who might not have read the instructions properly (ahem).
    stop("No trees found in ", empiricalWD)
  }

  #Treeness for empirical
  empiricalTreenessVector = vector(length = length(trees))

  for (i in seq_along(trees)) { # seq_along(0) != 1:0, which might be
                                # encountered if a user has not set the
                                # directory correctly
    tree <- read.tree(trees[i])
    empiricalTreenessVector[i] = treeness(tree)
  }

  return(empiricalTreenessVector)
}

# Using a single function to calculate this makes it easier to try
# different normalizations
TCI <- function(tree) {
  context <- TCIContext(tree)
  minimum <- context$minimum
  (TotalCopheneticIndex(tree) - minimum) / (context$uniform.exp - minimum)
}

empiricalTreeshape <- function() {
  #Get empirical data
  trees = list.files(empiricalWD, pattern = ".tre", full.names = TRUE)

  #Treeshape for empirical
  empiricalTreeshapeVector = vector(length = length(trees))

  for (i in 1:length(trees)) {
    # When first coded (19 June 24) there is an error in the maximum value of the index for the trees below, and thus these are skipped
    # This was immediately fixed in TreeTools, so this skipping code can be removed when that filters through to a release
    if (packageVersion("TreeTools") <= "1.11.1") {
      if (i==4 || i==9) next
    }
    tree = read.tree(trees[i])
    # Might it be appropriate to subtract $minimum before normalizing, 
    # and perhaps to normalize against the uniform.expected?
    # Or you might use the J1Index instead...
    empiricalTreeshapeVector[i] <- TCI(tree)
  }

  return(empiricalTreeshapeVector)
}

#J1 balance (shape) index for empirical trees
empiricalJ1 <- function() {
  stopifnot(packageVersion("TreeTools") >= "1.11.1.9001")
  vapply(list.files(empiricalWD, pattern = ".tre", full.names = TRUE),
         function(tr) J1Index(read.tree(tr)), double(1), USE.NAMES = FALSE)
}

empiricalSteps <- function() {
  #Get empirical data
  empiricalStepsDF <- data.frame()

  if (length(list.files(empiricalWD, pattern = "empirical_steps.csv")) == 1) {
    empiricalStepsDF <- read.csv(list.files(empiricalWD, pattern = "empirical_steps.csv", full.names = TRUE), stringsAsFactors = FALSE)
    return(empiricalStepsDF)
  }

  matrices = list.files(empiricalWD, pattern = ".nex", full.names = TRUE)
  if ("matrix.nex" %in% matrices) {
    matrices = matrices[1:(length(matrices) - 1)]
  }
  trees = gsub(".nex", ".tre", matrices)

  #Now extra steps for empirical
  lengths = vector(length = length(trees))

  for (i in 1:length(matrices)) {
    matrix = ReadMorphNexus(matrices[i])
    matrix2 = matrix$Matrix_1$Matrix
    tree = read.tree(trees[i])

    if (length(tree$tip.label) != nrow(matrix2)) {
      tree = drop.tip(tree, tree$tip.label[which(tree$tip.label %not in% rownames(matrix$Matrix_1$Matrix))])
    }

    if (length(which(rownames(matrix2) %not in% tree$tip.label)) > 0) {
      matrix2 = matrix2[-which(rownames(matrix2) %not in% tree$tip.label), ]
    }

    if (!is.binary(tree)) {
      tree = multi2di(tree, random = T)
    }

    binary = (matrix$Matrix_1$MaxVals == 1)
    to_calculate = which(binary)

    steps = vector(length = length(to_calculate))

    for (j in 1:length(to_calculate)) {
      character = matrix2[, to_calculate[j]]
      character[character == "0"] <- "a"
      character[character == "1"] <- "c"
      character[is.na(character)] <- "-"
      character[character == ""] <- "-"
      character[character != "a" & character != "c"] <- "-"
      steps[j] = parsimony(tree, as.phyDat(character))
    }
    steps <- data.frame(steps)
    steps$plot <- rep(gsub(".nex", "", matrices[i]))
    # allRatesEmpirical = c(allRatesEmpirical, rate)
    empiricalStepsDF <- rbind(empiricalStepsDF, steps)
  }
  write.csv(empiricalStepsDF, "empirical_steps.csv", row.names = FALSE)
  return(empiricalStepsDF)
}

########################################################################################################################
#Here starts the analysis elements of the script that call the above functions

#Create data structures and file lists
files <- list.files(sourceWD)
counts <- list()
meanRates <- list()
extraStepsDF <- data.frame()

#Do empirical for comparison - this will either calculate the data if not done so before and also output as a CSV
#Or if that CSV exists, will just read it in
cat("Doing empirical data.\n")
empiricalTreenessVector <- empiricalTreeness()
empiricalTreeshapeVector <- empiricalTreeshape()
empiricalJ1Vector <- empiricalJ1()
allRatesEmpirical <- empiricalSteps()
cat("Done.\n")

#Create a list of the tree files from TREvoSim using a filter
treeFiles <- list.files(sourceWD, pattern = "tree", full.names = TRUE)
nTreeFiles <- length(treeFiles)
treeshapes <- double(length(treeFiles))
j1s <- double(length(treeFiles))
simTreenessVector <- double(length(treeFiles))
matrixFiles <- list.files(sourceWD, pattern = ".nex", full.names = TRUE)
#Remove tree files
matrixFiles <- matrixFiles[!grepl("tree", matrixFiles)]


# Progress bar possibly nicer than flooding console with repeated messages?
detailedAnalyses <- min(nTreeFiles, 25)
prog <- cli::cli_progress_bar(
  format = "Calculating extra steps [{i}/{detailedAnalyses}]. Mean = {progExtra}",
  total = detailedAnalyses)

#Now do analysis for each tree/matrix/replicate
for (i in seq_along(treeFiles)) {
  simTree <- read.nexus(treeFiles[i])
  plot(simTree, show.tip.label = FALSE) # Not sure why this is here?
  simMatrix <- read_nexus_matrix(matrixFiles[i])[[c("matrix_1", "matrix")]]
  
  #Count extant if requested
  if (countExtant) {
    #identify extinct and extant taxa (0/1)
    #dist_taxa <- distRoot(simTree)
    #Below is quicker than above dist.nodes gives the distance to the root of all nodes as a matrix
    #square brackets asking for the row corresponding to the root node (which is always ntax + 1), and the columns from 1 to ntax
    dist_taxa <- dist.nodes(simTree)[(length(simTree$tip.label) + 1), (1:length(simTree$tip.label))]
    maxt <- max(dist_taxa)

    #Count number of fossils - note L ensures this is an integer.
    extantCount = length(which(dist_taxa == maxt))
    cat("Extant count is", extantCount, "\n")
    counts[i] <- extantCount
  }

  #Treeshape if requested
  if (calcTreeshape) {
    #See comment in empirical treeshape for more info on this calculation
    n <- length(simTree$tip.label)
    treeshapes[[i]] <- TCI(simTree)
    if (packageVersion("TreeTools") >= "1.11.1.9001") {
      j1s[[i]] <- J1Index(simTree)
    }
    #treeshapes[i] <- colless(simTree)/((n - 1) * (n - 2)/2)
  }

  #Calculate treeness if requested
  if (calcTreeness)
    simTreenessVector[[i]] <- treeness(simTree)

  #Count parsimony steps if requested
  #Given the number of characters, no need to go above really
  if (countSteps && i <= detailedAnalyses) {
    steps_run <- excessSteps(simTree, matrixFiles[[i]])
    steps_run$plot <- i
    extraStepsDF <- rbind(extraStepsDF, steps_run)
    progExtra = round(mean(as.numeric(steps_run$steps), na.rm = TRUE), 2)
    cli::cli_progress_update(prog, i)
  }
}
cli::cli_progress_done(prog)


########################################################################################################################
#Now move onto the graphing of this data, and save it in the outputWD
## At this point data is in:
## Counts - extant count for sim data
## extrastepsDF - extra steps for the simulation data
## Allratesempirical - extra steps for all empirical datasets
## empiricalTreenessVector - treeness for empirical datasets
## simTreenessVector - treeness for simulated datasets

#Histograms from extant number and the treeshape (asymmetry)
if (countExtant) {
  ## Do outputs for counts
  outputDF <- data.frame()

  outputDF <- do.call(rbind, Map(data.frame, counts = counts))

  #write.csv(outputDF, "counts.csv")
  cat("Mean extant is", mean(outputDF$counts), "\n")
  cat("Min extant is", min(outputDF$counts), "\n")
  cat("Max extant is", max(outputDF$counts), "\n")

  mid <- (max(outputDF) + min(outputDF))/2

  ggplot(data = outputDF, aes(x = counts, fill = ..x..)) +
    geom_histogram(color = "#000000") +
    theme_minimal() +
    scale_fill_gradient2(low = vapoRwave:::newRetro_palette[3], mid = vapoRwave:::newRetro_palette[2], high = vapoRwave:::newRetro_palette[1], midpoint = mid) +
    theme(panel.border = element_rect(color = "black", fill = NA)) +
    scale_y_continuous(expand = expansion(mult = c(0, 0.1))) +
    labs(title = "TREvoSim extant count", x = "Number of extant terminals", y = "Count") +
    theme(plot.title = element_text(hjust = 0.5)) + theme(legend.position = "none")

  ggsave(paste(outputWD, "Counts_histogram.pdf", sep = ""))
}

if (calcTreeshape) {
  empiricalTreeshapeDF <- data.frame(tci = empiricalTreeshapeVector,
                                     j1 = empiricalJ1Vector)
  empiricalTreeshapeDF$plot <- "Empirical"
  
  simTreeshapeDF <- data.frame(tci = treeshapes, j1 = j1s)
  simTreeshapeDF$plot <- "Simulated"
  allTreeshapeDF <- rbind(empiricalTreeshapeDF, simTreeshapeDF)

  ggplot(data = allTreeshapeDF, aes(y = tci, x = plot)) +
    # Notches are helpful to judge whether difference in medians is significant
    geom_boxplot(fill = "#396ff6", notch = TRUE) +
    theme_minimal() +
    theme(panel.border = element_rect(color = "black", fill = NA)) +
    ylim(0, 1) +
    labs(title = "TREvoSim vs Empirical tree asymmetry", x = "Data type", y = "Tree asymmetry (normalised total cophenetic index)") +
    theme(plot.title = element_text(hjust = 0.5)) +
    theme(legend.position = "none")
  ggsave(paste(outputWD, "TREvoSim_treeshape_plot.pdf", sep = ""))
  
  ggplot(data = allTreeshapeDF, aes(y = j1, x = plot)) +
    geom_boxplot(fill = "#396ff6", notch = TRUE) +
    theme_minimal() +
    theme(panel.border = element_rect(color = "black", fill = NA)) +
    ylim(0, 1) +
    labs(title = "TREvoSim vs Empirical tree symmetry", x = "Data type", y = "Tree asymmetry (J1 index)") +
    theme(plot.title = element_text(hjust = 0.5)) +
    theme(legend.position = "none")
  ggsave(paste(outputWD, "TREvoSim_j1_plot.pdf", sep = ""))
}

# Now graph the extra steps - empirical and simulated
if (countSteps) {
  # MRS: Conversion to numeric unnecessary now using NA instead of "NA"
  keeps <- c("steps", "plot")
  extraStepsDF2 <- extraStepsDF[keeps]

  #Change plot number for graphing
  extraStepsDF2$plot = "Simulated"

  #Combine DFs
  allSteps <- rbind(allRatesEmpirical, extraStepsDF2)

  #Pretty colours to make graphs A E S T H E T I C
  # hard-coding colours saves a user having to download a non-CRAN package
  RJGaesthetic <- c("#9239F6", "#903495", "#6F3460", "#4A354F", "#D20076",
                    "#FF0076", "#FF4373", "#FF6B58", "#F8B660", "#792096",
                    "#396FF6", "#44B05B", "#FA41CA", "#852942")
  
  #And write CSV in case it doesn't
  #write.csv(allSteps, "allSteps.csv")

  #Plot
  ggplot(data = allSteps, aes(y = steps, x = plot, group = plot, fill = plot)) +
    geom_violin(adjust = 1.5) + theme_minimal() + theme(panel.border = element_rect(color = "black", fill = NA)) +
    # scale_y_log10() + # Log10 unsatisfactory as many have zero extra steps
    #                   # Perhaps enough to truncate plot at ~12 extra steps?
    scale_x_discrete(guide = guide_axis(n.dodge = 2)) +
    scale_fill_manual(values = RJGaesthetic) +
    stat_summary(fun = mean, geom = "point", shape = 21, size = 2, fill = "white", color = "black", stroke = 0.5) + # fun.y deprecated
    labs(title = "TREvoSim vs Empirical extra parsimony steps", x = "Dataset", y = "Number of steps") +
    theme(plot.title = element_text(hjust = 0.5)) + theme(legend.position = "none")

  ggsave(paste(outputWD, "TREvoSim_homoplasy_plots.pdf", sep = ""))
}

if (calcTreeness) {
  #Now graph treeness
  empiricalTreenessDF <- data.frame(empiricalTreenessVector)
  colnames(empiricalTreenessDF) <- "treeness"
  empiricalTreenessDF$plot <- "Empirical"
  simTreenessDF <- data.frame(simTreenessVector)
  colnames(simTreenessDF) <- "treeness"
  simTreenessDF$plot <- "Simulated"
  allTreenessDF <- rbind(empiricalTreenessDF, simTreenessDF)

  ggplot(data = allTreenessDF, aes(y = treeness, x = plot)) +
    geom_boxplot(fill = "#792096") +
    theme_minimal() +
    theme(panel.border = element_rect(color = "black", fill = NA)) + ylim(0, 1) +
    labs(title = "TREvoSim vs Empirical treeness", x = "Data type", y = "Treeness") + theme(plot.title = element_text(hjust = 0.5)) +
    theme(legend.position = "none")

  ggsave(paste(outputWD, "TREvoSim_treeness_plots.pdf", sep = ""))
}

