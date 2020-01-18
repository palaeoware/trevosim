ptm <- proc.time()
end = "Distances calculated!" 
Pro = "Processing: Tree" 
library(phangorn)

	listOne <- read.nexus('mol.run1.t.nex') # your trees list one 
	listTwo <- read.nexus('morph.run1.t.nex') # your trees list two
	listThree <- read.nexus('TNT_tree_xinact.nex') # your trees list two
	listFour <- read.nexus('TNT_tree_xinact_piwe.nex') # your trees list two
	listOne <- unroot(listOne)
	listTwo <- unroot(listTwo)
	listThree <- unroot(listThree) 
	listFour <- unroot(listFour) 
# RF Bayesian
	resultRF1 <- list() # an empty list to store the RF distances
	for(x in 1:length(listOne))
		{
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
    	flush.console() 
    	Sys.sleep(0.01) 
		resultRF1[[x]] <- RF.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
		cat("...\n")
		}
		
	dresultRF1 <- data.frame(resultRF1)
	molmorph <- apply(dresultRF1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultRF1, 2, FUN=min) # record min values for each column
	meanRF <- mean(molmorph)
	meanRF <- append(meanRF,(mean(morphmol)))
	RFbayes <- mean(meanRF)
# RF Parsimony
	resultRF1 <- list() # an empty list to store the RF distances
	for(x in 1:length(listOne))
		{
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
    	flush.console() 
    	Sys.sleep(0.01) 
		resultRF1[[x]] <- RF.dist(listOne[[x]], listThree) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listThree will be collected
		cat("...\n")
		}
		
	dresultRF1 <- data.frame(resultRF1)
	molmorph <- apply(dresultRF1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultRF1, 2, FUN=min) # record min values for each column
	meanRF <- mean(molmorph)
	meanRF <- append(meanRF,(mean(morphmol)))
	RFpars <- mean(meanRF)
# RF IW
	resultRF1 <- list() # an empty list to store the RF distances
	for(x in 1:length(listOne))
		{
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
    	flush.console() 
    	Sys.sleep(0.01) 
		resultRF1[[x]] <- RF.dist(listOne[[x]], listFour) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listFour will be collected
		cat("...\n")
		}
		
	dresultRF1 <- data.frame(resultRF1)
	molmorph <- apply(dresultRF1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultRF1, 2, FUN=min) # record min values for each column
	meanRF <- mean(molmorph)
	meanRF <- append(meanRF,(mean(morphmol)))
	RFIW <- mean(meanRF)
distances <- data.frame(RF_Bayesian=RFbayes, RF_Parsimony=RFpars, RF_IW=RFIW)
write.csv(distances, file = "distances_RF.csv")
cat(end)
df_res <- head(distances)
df_res