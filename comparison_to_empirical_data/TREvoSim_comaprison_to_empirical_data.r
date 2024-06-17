library(phangorn)
library(phytools)
library(phylobase)
library(Claddis)
library(tidyverse)
library(vapoRwave)
library(parallel)
library(treestats)

#Clear environment
rm(list = ls())

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
countExtant <- TRUE
###Counts the number of parsimony steps on the true tree
countSteps <- TRUE
###Calculates the treeness of the tree
calcTreeness <- TRUE
###Calculates the tree symmetry
calcTreeshape <- TRUE

#Number of cores for parallelisation (currently only implemented for MK rate fitting)
numCores <- detectCores()

#Not in operator
`%not in%` <- function(x, table) is.na(match(x, table, nomatch = NA_integer_))

########################################################################################################################
#Function to calculate treeness
#i.e. sum of internal branch lengths/total branch lengths
treeness <- function(tree) {
  return(1 - (sum(tree$edge.length[which(tree$edge[, 2] %in% c(1:length(tree$tip.label)))])/sum(tree$edge.length)))
}

########################################################################################################################
#Function to calculate FitMK - this approach works because it doesn't matter what order we spit out the values
#mclapply won't work if you just send the columns to mclapply, but this way we can user an iterator - here characterNo
doFitMK <- function(characterNo, fSimTree, fSimMatrix) {
  #Print to show progress is happening
  ###Note this will only work when called from terminal - won't have any effect in R studio
  cat("*")
  #Fit MK to data
  return(fitMk(simTree, matrix[, characterNo], model = "ER")$rates)
}

########################################################################################################################
#Function to compute the number of excess steps for each character of a binary morphological character matrix.
#Constant characters will return NA.
#tree = object of class 'phylo'.
#mdata = binary character matrix of class 'list', e.g. as produced by the 'read.nexus.data' function

#tree<-simTree
#mdata<-read.nexus.data(matrixFiles[1])
#tree<-read.nexus("/home/russell/Desktop/TREvoSim_output_unrooted/TREvoSim_tree_000.nex")
#mdata<-read.nexus.data("/home/russell/Desktop/TREvoSim_output_unrooted/TREvoSim_000.nex")

steps <- function(tree, mdata) {
  char_results <- data.frame(character = 0, steps = 0)
  levelInData <- levels(as.factor(unlist(mdata)))
  mdata_df <- as.data.frame(mdata)
  for (z in 1:length(unlist(mdata[1]))) {
    list_char <- as.list(mdata_df[z, ])
    phy_char <- phyDat(list_char, type = "USER", levels = levelInData)
    m_steps <- parsimony(tree, phy_char)
    newrow <- data.frame(character = z, steps = m_steps - 1)
    char_results <- rbind(char_results, newrow)
  }
  char_results <- char_results[-c(1), ]
  for (i in 1:length(char_results$steps)) if (char_results$steps[[i]] < 0) {
    char_results$steps[[i]] <- "NA"
  }
  return(char_results)
}

########################################################################################################################
#Function to compute rates (extra steps) and treeness for empirical characters
empiricalTreeness <- function() {
  #Get empirical data
  trees <- list.files(empiricalWD, pattern = ".tre", full.names = TRUE)

  #Treeness for empirical
  empiricalTreenessVector = vector(length = length(trees))

  for (i in 1:length(trees)) {
    tree <- read.tree(trees[i])
    empiricalTreenessVector[i] = treeness(tree)
  }

  return(empiricalTreenessVector)
}

empiricalColless <- function() {
  #Get empirical data
  trees = list.files(empiricalWD, pattern = ".tre", full.names = TRUE)

  #Treeshape for empirical
  empiricalTreeshapeVector = vector(length = length(trees))

  for (i in 1:length(trees)) {
    tree = read.tree(trees[i])
    #Use the corrected colless so number of tips is not an isssue. See:
    #library(treebalance) collessI( method = 'corrected')
    #And associated book
    n = length(tree$tip.label)
    empiricalTreeshapeVector[i] = colless(tree)/((n - 1) * (n - 2)/2)
  }

  return(empiricalTreeshapeVector)
}

empiricalSteps <- function() {
  #Get emprical data
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
      character[which(character == "0")] = "a"
      character[which(character == "1")] = "c"
      character[is.na(character)] = "-"
      character[which(character == "")] = "-"
      character[which(character != "a" & character != "c")] = "-"
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
treeshapes <- list()
meanRates <- list()
simTreenessVector <- vector()
extraStepsDF <- data.frame()

#Do empirical for comparison - this will either calculate the data if not done so before and also output as a CSV
#Or if that CSV exists, will just read it in
cat("Doing empirical data.\n")
empiricalTreenessVector <- empiricalTreeness()
empiricalTreeshapeVector <- empiricalColless()
allRatesEmpirical <- empiricalSteps()
cat("Done.\n")

#Create a list of the tree files from TREvoSim using a filter
treeFiles <- list.files(sourceWD, pattern = "tree", full.names = TRUE)
matrixFiles <- list.files(sourceWD, pattern = ".nex", full.names = TRUE)
#Remove tree files
matrixFiles <- matrixFiles[!grepl("tree", matrixFiles)]

#Now do analysis for each tree/matrix/replicate
for (i in 1:length(treeFiles)) {
  simTree <- read.nexus(treeFiles[i])
  plot(simTree, show.tip.label = FALSE)
  simMatrix <- read_nexus_matrix(matrixFiles[i])
  #ReadMorphNexus(matrixFiles[i]);
  simMatrix <- simMatrix$matrix_1$matrix
  #simMatrix$Matrix_1$Matrix

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
    #See comment in empirical treeshape for mor info on this calculation
    n = length(simTree$tip.label)
    treeshapes[i] <- colless(simTree)/((n - 1) * (n - 2)/2)
  }

  #Calculate treeness if requested
  if (calcTreeness)
    simTreenessVector[i] = treeness(simTree)

  #Count parsimony steps if requested
  #Given the number of characters, no need to go above really
  if (countSteps && i < 25) {
    cat("Running extra steps analysis, dataset ", i, " - will only do the first 25 datasets out of ", length(treeFiles), ".\n")
    tree<-simTree
    mdata<-read.nexus.data(matrixFiles[1])
    mdata2<-read.nexus.data(matrixFiles[1])
    steps_run <- steps(simTree, read.nexus.data(matrixFiles[i]))
    steps_run$plot <- i
    extraStepsDF <- rbind(extraStepsDF, steps_run)
    cat("Mean extra steps is ", mean(as.numeric(steps_run$steps), na.rm = TRUE), "\n")
  }
}

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

  write.csv(outputDF, "counts.csv")
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
  empiricalTreeshapeDF <- data.frame(empiricalTreeshapeVector)
  colnames(empiricalTreeshapeDF) <- "treeshape"
  empiricalTreeshapeDF$plot <- "Empirical"
  simTreeshapeDF <- do.call(rbind, Map(data.frame, ts = treeshapes))

  colnames(simTreeshapeDF) <- "treeshape"
  simTreeshapeDF$plot <- "Simulated"
  allTreeshapeDF <- rbind(empiricalTreeshapeDF, simTreeshapeDF)

  ggplot(data = allTreeshapeDF, aes(y = treeshape, x = plot)) +
    geom_boxplot(fill = "#396ff6") +
    theme_minimal() +
    theme(panel.border = element_rect(color = "black", fill = NA)) +
    ylim(0, 1) +
    labs(title = "TREvoSim vs Empirical treeshape", x = "Data type", y = "Treeshape (corrected Colless)") +
    theme(plot.title = element_text(hjust = 0.5)) +
    theme(legend.position = "none")

  ggsave(paste(outputWD, "TREvoSim_treeshape_plot.pdf", sep = ""))
}

# Now graph the extra steps - empirical and simulated
if (countSteps) {
  # The below is required to get data into a format ggplot can work with - otherwise it seems to treat them all as strings
  extraStepsDF <- apply(extraStepsDF, 2, as.numeric)
  extraStepsDF <- data.frame(extraStepsDF)
  keeps <- c("steps", "plot")
  extraStepsDF2 <- extraStepsDF[keeps]

  #Change plot number for graphing
  extraStepsDF2$plot = "Simulated"

  #Combine DFs
  allSteps <- rbind(allRatesEmpirical, extraStepsDF2)

  #Pretty colours to make graphs A E S T H E T I C
  RJGaesthetic <- palette(c(vapoRwave:::newRetro_palette, "#792096", "#396ff6", "#44B05B", "#FA41CA", "#852942"))
  #Sometimes above doesn't stick - do it twice to see if that helps
  RJGaesthetic <- palette(c(vapoRwave:::newRetro_palette, "#792096", "#396ff6", "#44B05B", "#FA41CA", "#852942"))

  #And write CSV in case it doesn't
  write.csv(allSteps, "allSteps.csv")

  #Plot
  ggplot(data = allSteps, aes(y = steps, x = plot, group = plot, fill = plot)) +
    geom_violin(adjust = 1.5) + theme_minimal() + theme(panel.border = element_rect(color = "black", fill = NA)) +
    scale_y_log10() +
    scale_x_discrete(guide = guide_axis(n.dodge = 2)) +
    scale_fill_manual(values = RJGaesthetic) +
    stat_summary(fun.y = mean, geom = "point", shape = 21, size = 2, fill = "white", color = "black", stroke = 0.5) +
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
