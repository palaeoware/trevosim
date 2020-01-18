ptm <- proc.time()
library(phangorn)
library(ggplot2)
meanRF <- list()
distances <- data.frame(RF_Bayesian=0, RF_Parsimony=0, RF_IW=0, SPR_Bayesian=0, SPR_Parsimony=0, SPR_IW=0)
end = "Distances calculated!" 
Pro = "Processing: Run" 
for(i in 0:199) {
	cat("[",(proc.time() - ptm), "]", Pro, (sprintf( "%d", i))) 
	readMB <- paste(i,".run1.t.nex", sep = "")
    readSim <- paste(i,"_sim.nex", sep = "")
    readTNT <- paste(i,"_POUT.tre", sep = "")
    readIW <- paste(i,"_POUT_IW.tre", sep = "")
    MB_list <- read.nexus(readMB) # iterate over every mb tree, read nexus
    TNT_list <- read.nexus(readTNT) # iterate over every mb tree, read nexus
    IW_list <- read.nexus(readIW) # iterate over every mb tree, read nexus 
    Sim_tre <- read.nexus(readSim) #  iterate over every sim tree, read nexus
    MB_list <- unroot(MB_list)
	TNT_list <- unroot(TNT_list)
	IW_list <- unroot(IW_list)
	Sim_tre <- unroot(Sim_tre) 
    resultRF1 <- list() # an empty list to store the RF distances
    resultRF2 <- list() # an empty list to store the RF distances
    resultRF3 <- list() # an empty list to store the RF distances
    resultRF1 <- RF.dist(MB_list, Sim_tre) # For each tree in 'MB_list' the RF distances against all trees in 'Sim_tre' will be collected
    resultRF2 <- RF.dist(TNT_list, Sim_tre) # For each tree in 'TNT_list' the RF distances against all trees in 'Sim_tre' will be collected
    resultRF3 <- RF.dist(IW_list, Sim_tre) # For each tree in 'IW_list' the RF distances against all trees in 'Sim_tre' will be collected
	uresultRF1 <- unlist(resultRF1)
    uresultRF2 <- unlist(resultRF2)
    uresultRF3 <- unlist(resultRF2)
    meanRF1 <- mean(uresultRF1)
    meanRF2 <- mean(uresultRF2)
    meanRF3 <- mean(uresultRF3)
    resultSPR1 <- list() # an empty list to store the SPR distances
    resultSPR2 <- list() # an empty list to store the SPR distances
    resultSPR3 <- list() # an empty list to store the SPR distances
    resultSPR1 <- SPR.dist(MB_list, Sim_tre) # For each tree in 'MB_list' the SPR distances against all trees in 'Sim_tre' will be collected
    resultSPR2 <- SPR.dist(TNT_list, Sim_tre) # For each tree in 'TNT_list' the SPR distances against all trees in 'Sim_tre' will be collected
    resultSPR3 <- SPR.dist(IW_list, Sim_tre) # For each tree in 'IW_list' the SPR distances against all trees in 'Sim_tre' will be collected
	uresultSPR1 <- unlist(resultSPR1)
    uresultSPR2 <- unlist(resultSPR2)
    uresultSPR3 <- unlist(resultSPR3)
    meanSPR1 <- mean(uresultSPR1)
    meanSPR2 <- mean(uresultSPR2)
    meanSPR3 <- mean(uresultSPR3)    
    newrow <- data.frame(RF_Bayesian=meanRF1, RF_Parsimony=meanRF2, RF_IW=meanRF2, SPR_Bayesian=meanSPR1, SPR_Parsimony=meanSPR2, SPR_IW=meanSPR3)
    distances <- (rbind (distances, newrow)) 
    cat("...\n")}
distances <- distances[-c(1), ] 
rownames(distances) <- seq(length=nrow(distances))
write.csv(distances, file = "distances.csv")
cat(end)