# R script to calculate distances of simulated vs estimated trees. Written by J. Keating 2018.

library(phangorn)
library(ggplot2)

# Create blank data frame

distances <- data.frame(RF_Bayesian=0, RF_EW=0, RF_IW=0, SPR_Bayesian=0, SPR_EW=0, SPR_IW=0, nodes_B =0, nodes_EW =0, nodes_IW =0, con_RFB =0, con_RFEW =0, con_RFIW = 0, NN_RFB = 0, NN_RFEW =0, NN_RFIW =0, NN_SPRB = 0, NN_SPREW = 0, NN_SPRIW = 0)
end = "Distances calculated!" 
Pro = "Processing: Run" 

# Begin forloop for 999 replicates 

for(i in 0:999) {
	cat(Pro, (sprintf("%d", i)))
	cat("\n")
	
	# read nexus tree files 
	
	readMB <- paste(i,".run1.t.nex", sep = "")
    readSim <- paste(i,"_sim.nex", sep = "")
    readTNT <- paste(i,"_POUT.tre", sep = "")
    readIW <- paste(i,"_POUT_IW.tre", sep = "")
    MB_list <- read.nexus(readMB) # iterate over every mb tree, read nexus
    TNT_list <- read.nexus(readTNT) # iterate over every mb tree, read nexus
    IW_list <- read.nexus(readIW) # iterate over every mb tree, read nexus 
    Sim_tre <- read.nexus(readSim) #  iterate over every sim tree, read nexus
    
    # unroot trees 
    
    MB_list <- unroot(MB_list)
	TNT_list <- unroot(TNT_list)
	IW_list <- unroot(IW_list)
	Sim_tre <- unroot(Sim_tre) 
	
	# produce 50% maj-rule consensus trees for Bayesian Mk, EW parsimony and IW parsimony estimate trees 
	
	con_MB <- consensus(MB_list, p= 0.5)
	con_P <- consensus(TNT_list, p= 0.5)
	con_IW <- consensus(IW_list, p= 0.5)
	
	# Calculate RF distances from each estimate tree to the simulated tree
	
    resultRF1 <- list() # an empty list to store the RF distances
    resultRF2 <- list() # an empty list to store the RF distances
    resultRF3 <- list() # an empty list to store the RF distances
    resultRF1 <- RF.dist(MB_list, Sim_tre) # For each tree in 'MB_list' the RF distances against all trees in 'Sim_tre' will be collected
    resultRF2 <- RF.dist(TNT_list, Sim_tre) # For each tree in 'TNT_list' the RF distances against all trees in 'Sim_tre' will be collected
    resultRF3 <- RF.dist(IW_list, Sim_tre) # For each tree in 'IW_list' the RF distances against all trees in 'Sim_tre' will be collected
	uresultRF1 <- unlist(resultRF1)
    uresultRF2 <- unlist(resultRF2)
    uresultRF3 <- unlist(resultRF3)
    
    # Calculate mean RF distance from estimate trees to simulated tree
    meanRF1 <- mean(uresultRF1)
    meanRF2 <- mean(uresultRF2)
    meanRF3 <- mean(uresultRF3)
    
    # Calculate nearest neighbour RF distance from estimate trees to simulated tree
    
    minRF1 <- min(uresultRF1)
    minRF2 <- min(uresultRF2)
    minRF3 <-  min(uresultRF3)
    
    # Calculate RF distance from 50% maj rule of estimate trees to simulated tree
    
    D_con_MB <- RF.dist(con_MB, Sim_tre)
    D_con_P <- RF.dist(con_P, Sim_tre) 
    D_con_IW <- RF.dist(con_IW, Sim_tre)  
    
    # Calculate approx. SPR distances from each estimate tree to the simulated tree
     
    resultSPR1 <- list() # an empty list to store the SPR distances
    resultSPR2 <- list() # an empty list to store the SPR distances
    resultSPR3 <- list() # an empty list to store the SPR distances
    resultSPR1 <- SPR.dist(MB_list, Sim_tre) # For each tree in 'MB_list' the SPR distances against all trees in 'Sim_tre' will be collected
    resultSPR2 <- SPR.dist(TNT_list, Sim_tre) # For each tree in 'TNT_list' the SPR distances against all trees in 'Sim_tre' will be collected
    resultSPR3 <- SPR.dist(IW_list, Sim_tre) # For each tree in 'IW_list' the SPR distances against all trees in 'Sim_tre' will be collected
	uresultSPR1 <- unlist(resultSPR1)
    uresultSPR2 <- unlist(resultSPR2)
    uresultSPR3 <- unlist(resultSPR3)
    
    # Calculate mean SPR distance from estimate trees to simulated tree
    
    meanSPR1 <- mean(uresultSPR1)
    meanSPR2 <- mean(uresultSPR2)
    meanSPR3 <- mean(uresultSPR3)
    
    # Calculate nearest neighbour SPR distance from estimate trees to simulated tree
    
    minSPR1 <- min(uresultSPR1)
    minSPR2 <- min(uresultSPR2)
    minSPR3 <- min(uresultSPR3)    
    
    # Create new row of data frame
      
    newrow <- data.frame(RF_Bayesian=meanRF1, RF_EW=meanRF2, RF_IW=meanRF3, SPR_Bayesian=meanSPR1, SPR_EW=meanSPR2, SPR_IW=meanSPR3, nodes_B = Nnode(con_MB), nodes_EW = Nnode(con_P), nodes_IW =Nnode(con_IW), con_RFB = D_con_MB, con_RFEW = D_con_P, con_RFIW = D_con_IW, NN_RFB = minRF1, NN_RFEW = minRF2, NN_RFIW = minRF3, NN_SPRB = minSPR1, NN_SPREW = minSPR2, NN_SPRIW =minSPR3)
    distances <- (rbind (distances, newrow)) 
    cat("...\n")}
distances <- distances[-c(1), ] 
rownames(distances) <- seq(length=nrow(distances))
write.csv(distances, file = "distances.csv")
cat(end)