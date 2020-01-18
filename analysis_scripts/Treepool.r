library(phangorn)

trees <- read.nexus('mol.run1.t.nex') # your trees list one 
TN <- length(trees)-1
listOne <- trees[c(1:TN)]
resultRF1 <- list() # an empty list to store the RF distances
pb <- txtProgressBar(min = 1, max = length(listOne), style = 3)				# create progress bar
for(x in 1:length(listOne)){
		listTwo<- trees[c((x+1):length(trees))]
		resultRF1[[x]] <- RF.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
		setTxtProgressBar(pb, x)										# update progress bar
		}
mol <- mean(unlist(resultRF1))

trees <- read.nexus('morph.run1.t.nex') # your trees list one 
TN <- length(trees)-1
listOne <- trees[c(1:TN)]
resultRF1 <- list() # an empty list to store the RF distances
pb <- txtProgressBar(min = 1, max = length(listOne), style = 3)				# create progress bar
for(x in 1:length(listOne)){
		listTwo<- trees[c((x+1):length(trees))]
		resultRF1[[x]] <- RF.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
		setTxtProgressBar(pb, x)										# update progress bar
		}
morph <- mean(unlist(resultRF1))


trees <- read.nexus('TNT_tree_xinact.nex') # your trees list one
trees <- unroot(trees)
if (class(trees) =="phylo") {
EW <- 0
} else {  
TN <- length(trees)-1
listOne <- trees[c(1:TN)]
resultRF1 <- list() # an empty list to store the RF distances
pb <- txtProgressBar(min = 1, max = length(listOne), style = 3)				# create progress bar
for(x in 1:length(listOne)){
		listTwo<- trees[c((x+1):length(trees))]
		resultRF1[[x]] <- RF.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
		setTxtProgressBar(pb, x)										# update progress bar
		}
EW <- mean(unlist(resultRF1))
}
	
trees <- read.nexus('TNT_tree_xinact.nex') # your trees list one 
trees <- unroot(trees)
if (class(trees) =="phylo") {
IW <- 0
} else {  
TN <- length(trees)-1
listOne <- trees[c(1:TN)]
resultRF1 <- list() # an empty list to store the RF distances
pb <- txtProgressBar(min = 1, max = length(listOne), style = 3)				# create progress bar
for(x in 1:length(listOne)){
		listTwo<- trees[c((x+1):length(trees))]
		resultRF1[[x]] <- RF.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
		setTxtProgressBar(pb, x)										# update progress bar
		}
IW <- mean(unlist(resultRF1))}	

df <- data.frame(Molecular = mol, Bayesian = morph, Equal_weights = EW, Implied_weights= IW)
write.csv(df, file = "precision.csv")
df





	