tshape <- list()
ptm <- proc.time()
library(phangorn)
library(apTreeshape)
library(ggplot2)
end = "Shapes calculated!" 
Pro = "Processing: Run" 
for(i in 0:49) {
	cat("[",(proc.time() - ptm), "]", Pro, (sprintf( "%d", i))) 
    readSim <- paste(i,"_sim.nex", sep = "")
    Sim_tre <- read.nexus(readSim) #  iterate over every sim tree, read nexus
    Sim_shape <- as.treeshape(Sim_tre)
    shapevalue <- colless(Sim_shape)
    append(tshape, shapevalue)
    cat("...\n")}
dresult <- data.frame(as.numeric(tshape))
write.csv(dresult, file="treeshape.csv")
ggplot(dresults, aes(tshape)) +
    geom_density() + xlim(0, ((Ntip(Sim_tre)-1)*(Ntip(Sim_tre)-2)/2))
cat(end)


    