ptm <- proc.time()
end = "Distances calculated!" 
Pro = "Processing: Tree" 
library(phangorn)
library(doMC)
library(foreach)
n <- detectCores(all.tests = FALSE, logical = TRUE)
registerDoMC(cores=n)
pb <- txtProgressBar(min = 0, max = 19, style = 3)				# create progress bar

res <- foreach(i=1:19) %dopar% {

readMB <- paste(i,".run1.t.nex", sep = "")
trees <- read.nexus(readMB) # your trees list one
trees <- unroot(trees) 
TN <- length(trees)-1
listOne <- trees[c(1:TN)]
resultRF1 <- list() # an empty list to store the RF distances
for(x in 1:length(listOne)){
		listTwo<- trees[c((x+1):length(trees))]
		resultRF1[[x]] <- RF.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
		}
morph <- mean(unlist(resultRF1))

readTNT <- paste(i,"_POUT.tre", sep = "")
trees <- read.nexus(readTNT) # your trees list one
trees <- unroot(trees)
if (class(trees) =="phylo") {
EW <- 0
} else {  
TN <- length(trees)-1
listOne <- trees[c(1:TN)]
resultRF1 <- list() # an empty list to store the RF distances
for(x in 1:length(listOne)){
		listTwo<- trees[c((x+1):length(trees))]
		resultRF1[[x]] <- RF.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
		}
EW <- mean(unlist(resultRF1))
}

readIW <- paste(i,"_POUT_IW.tre", sep = "")	
trees <- read.nexus(readIW) # your trees list one 
trees <- unroot(trees)
if (class(trees) =="phylo") {
IW <- 0
} else {  
TN <- length(trees)-1
listOne <- trees[c(1:TN)]
resultRF1 <- list() # an empty list to store the RF distances
for(x in 1:length(listOne)){
		listTwo<- trees[c((x+1):length(trees))]
		resultRF1[[x]] <- RF.dist(listOne[[x]], listTwo) # iterate over every tree in list One. For each tree in listOne the RF distances against all trees in listTwo will be collected
		}
IW <- mean(unlist(resultRF1))}	

newrow <- data.frame(RF_Bayesian= morph, RF_Parsimony= EW, RF_IW= IW)
distances <- (rbind (distances, newrow)) 
setTxtProgressBar(pb, i)										# update progress bar
}
distances <- distances[-c(1), ] 
rownames(distances) <- seq(length=nrow(distances))
mean_p <- data.frame(Bayesian = mean(distances$RF_Bayesian), Parsimony = mean(distances$RF_Parsimony), IW = mean(distances$RF_IW))
write.csv(mean_P, file = "mean_distances.csv")
cat(end)
