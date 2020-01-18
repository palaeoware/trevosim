ptm <- proc.time()
end = "Distances calculated!" 
Pro = "Processing: Tree" 
library(phangorn)
# RF Bayesian
	listOne <- read.nexus('mol.run1.t.nex') # your trees list one 
	listTwo <- read.nexus('morph.run1.t.nex') # your trees list two
	listOne <- unroot(listOne)
	listTwo <- unroot(listTwo)
	resultRF1 <- list() # an empty list to store the RF distances
	resultRF2 <- list() # an empty list to store the RF distances 
	for(x in 1:length(listOne)){
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
	resultRF1[[x]] <- RF.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
	cat("...\n")}
	dresultRF1 <- data.frame(resultRF1)
	molmorph <- apply(dresultRF1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultRF1, 2, FUN=min) # record min values for each column
	meanRF <- mean(molmorph)
	meanRF <- append(meanRF,(mean(morphmol)))
	RFbayes <- mean(meanRF)
# SPR Bayesian
	resultRF1 <- list() # an empty list to store the RF distances
	resultRF2 <- list() # an empty list to store the RF distances 
	for(x in 1:length(listOne)){
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
	resultRF1[[x]] <- SPR.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the SPR distances against all trees in listTwo will be collected
	cat("...\n")}
	dresultRF1 <- data.frame(resultRF1)
	molmorph <- apply(dresultRF1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultRF1, 2, FUN=min) # record min values for each column
	meanSPR <- mean(molmorph)
	meanSPR <- append(meanSPR,(mean(morphmol)))
	SPRbayes <- mean(meanSPR)
# RF Parsimony
	listTwo <- read.nexus('TNT_tree_xinact.nex') # your trees list two
	listTwo <- unroot(listTwo) 
	resultRF1 <- list() # an empty list to store the RF distances
	resultRF2 <- list() # an empty list to store the RF distances 
	for(x in 1:length(listOne)){
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
	resultRF1[[x]] <- RF.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
	cat("...\n")}
	dresultRF1 <- data.frame(resultRF1)
	molmorph <- apply(dresultRF1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultRF1, 2, FUN=min) # record min values for each column
	meanRF <- mean(molmorph)
	meanRF <- append(meanRF,(mean(morphmol)))
	RFpars <- mean(meanRF)
# SPR Parsimony
	resultRF1 <- list() # an empty list to store the RF distances
	resultRF2 <- list() # an empty list to store the RF distances 
	for(x in 1:length(listOne)){
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
	resultRF1[[x]] <- SPR.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the SPR distances against all trees in listTwo will be collected
	cat("...\n")}
	dresultRF1 <- data.frame(resultRF1)
	molmorph <- apply(dresultRF1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultRF1, 2, FUN=min) # record min values for each column
	meanSPR <- mean(molmorph)
	meanSPR <- append(meanSPR,(mean(morphmol)))
	SPRpars <- mean(meanSPR)
# RF IW
	listThree <- read.nexus('TNT_tree_xinact_piwe.nex') # your trees list two
	listThree <- unroot(listTwo) 
	resultRF1 <- list() # an empty list to store the RF distances
	resultRF2 <- list() # an empty list to store the RF distances 
	for(x in 1:length(listOne)){
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
	resultRF1[[x]] <- RF.dist(listOne[[x]], listThree) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
	cat("...\n")}
	dresultRF1 <- data.frame(resultRF1)
	molmorph <- apply(dresultRF1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultRF1, 2, FUN=min) # record min values for each column
	meanRF <- mean(molmorph)
	meanRF <- append(meanRF,(mean(morphmol)))
	RFIW <- mean(meanRF)
# SPR IW
	resultRF1 <- list() # an empty list to store the RF distances
	resultRF2 <- list() # an empty list to store the RF distances 
	for(x in 1:length(listOne)){
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
	resultRF1[[x]] <- SPR.dist(listOne[[x]], listThree) # iterate over every tree in list One. For each tree in listOne the SPR distances against all trees in listTwo will be collected
	cat("...\n")}
	dresultRF1 <- data.frame(resultRF1)
	molmorph <- apply(dresultRF1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultRF1, 2, FUN=min) # record min values for each column
	meanSPR <- mean(molmorph)
	meanSPR <- append(meanSPR,(mean(morphmol)))
	SPRIW <- mean(meanSPR)
distances <- data.frame(RF_Bayesian=RFbayes,RF_Parsimony=RFpars, RF_IW=RFIW, SPR_Bayesian=SPRbayes, SPR_Parsimony=SPRpars, SPR_IW=SPRIW)
write.csv(distances, file = "distances.csv")
cat(end)
df_res <- head(distances)
df_res