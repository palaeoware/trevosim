ptm <- proc.time()
library(phangorn)
library(ggplot2)
meanRF <- list()
distances <- data.frame(Bayesian=0,Parsimony=0)
end = "Distances calculated!" 
Pro = "Processing: Run" 
for(i in 0:999) {
	cat("[",(proc.time() - ptm), "]", Pro, (sprintf( "%d", i))) 
	readMB <- paste(i,".run1.t.nex", sep = "")
    readSim <- paste(i,"_sim.nex", sep = "")
    readTNT <- paste(i,"_POUT.tre", sep = "")
    MB_list <- read.nexus(readMB) # iterate over every mb tree, read nexus 
    Sim_tre <- read.nexus(readSim) #  iterate over every sim tree, read nexus
    MB_list <- unroot(MB_list)
    Sim_tre <- unroot(Sim_tre) 
    resultRF1 <- list() # an empty list to store the RF distances
    resultRF2 <- list() # an empty list to store the RF distances
    resultRF1 <- RF.dist(MB_list, Sim_tre) # For each tree in listOne the RF distances against all trees in listTwo will be collected
    TNT_list <- read.nexus(readTNT) # iterate over every tnt tree, read nexus
    TNT_list <- unroot(TNT_list)  
    resultRF2 <- RF.dist(TNT_list, Sim_tre) # For each tree in listOne the RF distances against all trees in listTwo will be collected
	uresultRF1 <- unlist(resultRF1)
    uresultRF2 <- unlist(resultRF2)
    meanRF1 <- mean(uresultRF1)
    meanRF2 <- mean(uresultRF2)
    newrow <- data.frame(Bayesian=meanRF1,Parsimony=meanRF2)
    distances <- (rbind (distances, newrow)) 
    cat("...\n")}
distances <- distances[-c(1), ] 
rownames(distances) <- seq(length=nrow(distances))
write.csv(distances, file = "distances.csv")
ggplot(distances, aes(x=Bayesian, y=Parsimony)) + geom_point(shape=1) + geom_smooth(method=lm , color="red", se=TRUE)  # add confidence
cat(end)