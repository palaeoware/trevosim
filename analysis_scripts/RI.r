end = "RI calculated!" 
Pro = "Processing: Tree" 
library(phangorn)
Results_RI <- list()
for(i in 0:999) {
	cat(Pro, (sprintf( "%d", i)))
	file_nex1 <- paste(i,"_sim.nex", sep = "")
	file_nex2 <- paste("F",i+1, sep = "")
	x <- read.nexus(file_nex1)
	y <- read.nexus.data(file_nex2) 
	levelInData <- levels(as.factor(unlist(y))) 
	phyDat_format <- phyDat(y, type="USER", levels=levelInData)
	RI(x, phyDat_format)
	RI_value <- RI(x, phyDat_format, cost = NULL, sitewise = FALSE) # iterate over every tree. RI.
	Results_RI <- append(Results_RI, RI_value)
	cat("...\n")
	}
write.csv(Results_RI, file = "RI_results.csv")
cat(end)