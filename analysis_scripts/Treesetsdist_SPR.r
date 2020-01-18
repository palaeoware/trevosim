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
# SPR Bayesian
	resultSPR1 <- list() # an empty list to store the SPR distances
	for(x in 1:length(listOne))
		{
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
    	flush.console() 
    	Sys.sleep(0.01) 
		resultSPR1[[x]] <- SPR.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the SPR distances against all trees in listTwo will be collected
		cat("...\n")
		}
		
	dresultSPR1 <- data.frame(resultSPR1)
	molmorph <- apply(dresultSPR1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultSPR1, 2, FUN=min) # record min values for each column
	meanSPR <- mean(molmorph)
	meanSPR <- append(meanSPR,(mean(morphmol)))
	SPRbayes <- mean(meanSPR)
# SPR Parsimony
	resultSPR1 <- list() # an empty list to store the SPR distances
	for(x in 1:length(listOne))
		{
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
    	flush.console() 
    	Sys.sleep(0.01) 
		resultSPR1[[x]] <- SPR.dist(listOne[[x]], listThree) # iterate over every tree in list One. For each tree in listOne the SPR distances against all trees in listThree will be collected
		cat("...\n")
		}
		
	dresultSPR1 <- data.frame(resultSPR1)
	molmorph <- apply(dresultSPR1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultSPR1, 2, FUN=min) # record min values for each column
	meanSPR <- mean(molmorph)
	meanSPR <- append(meanSPR,(mean(morphmol)))
	SPRpars <- mean(meanSPR)
# SPR IW
	resultSPR1 <- list() # an empty list to store the SPR distances
	for(x in 1:length(listOne))
		{
		cat("[",(proc.time() - ptm), "]            			", Pro, (sprintf( "%d", x))) 
    	flush.console() 
    	Sys.sleep(0.01) 
		resultSPR1[[x]] <- SPR.dist(listOne[[x]], listFour) # iterate over every tree in list One. For each tree in listOne the SPR distances against all trees in listFour will be collected
		cat("...\n")
		}
		
	dresultSPR1 <- data.frame(resultSPR1)
	molmorph <- apply(dresultSPR1, 1, FUN=min) # record min values for each row 
	morphmol <- apply(dresultSPR1, 2, FUN=min) # record min values for each column
	meanSPR <- mean(molmorph)
	meanSPR <- append(meanSPR,(mean(morphmol)))
	SPRIW <- mean(meanSPR)
distances <- data.frame(SPR_Bayesian=SPRbayes, SPR_Parsimony=SPRpars, SPR_IW=SPRIW)
write.csv(distances, file = "distances_SPR.csv")
cat(end)
df_res <- head(distances)
df_res