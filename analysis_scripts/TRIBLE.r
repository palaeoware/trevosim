TS_RI <- data.frame("Tree_no." = 0, "Treeshape" = 0, "RI" = 0, "N_int_nodes" =0, "T_height" =0, "N_min_BL" =0, "N_max_BL" =0, "N_mean_BL" =0, "N_mean_no" =0, "mean_gpc" = 0, "bimodality_coefficient" =0)
end = "RI calculated!" 
Pro = "Processing: Tree" 
library(phangorn)
library(apTreeshape)
library(ggplot2)
library(adephylo)
library(stringr)
library(modes)
for(i in 0:199) {
	cat(Pro, (sprintf( "%d", i)))
	pad<-str_pad(i, 3, pad = "0")
	file_nex1 <- paste("TreeSim_tree_",pad,".nex", sep = "")
	file_nex2 <- paste("F",i+1, sep = "")
	Sim_tre <- read.nexus(file_nex1)
	Sim_data <- read.nexus.data(file_nex2) 
	Sim_df <- as.data.frame(Sim_data)
	Sim_shape <- as.treeshape(Sim_tre)
	shapevalue <- colless(Sim_shape)
	Sim_tre_ur <- unroot(Sim_tre)
	levelInData <- levels(as.factor(unlist(Sim_data))) 
	phyDat_format <- phyDat(Sim_data, type="USER", levels=levelInData)
	RI_value <- RI(Sim_tre_ur, phyDat_format, cost = NULL, sitewise = FALSE) # iterate over every tree. RI.
	No_int <- Nnode(Sim_tre, internal.only = TRUE)
	Theight <- max(distRoot(Sim_tre))
	meanbl <- mean(Sim_tre$edge.length)
	maxbl <- max(Sim_tre$edge.length)
	minbl <- min(Sim_tre$edge.length)
	n_meanbl <- (meanbl/Theight)*100
	n_maxbl <- (maxbl/Theight)*100
	n_minbl <- (minbl/Theight)*100
	No_depth <- node.depth.edgelength(Sim_tre)
	No_depth <- - (No_depth - max(node.depth.edgelength(Sim_tre)))
	No_depth <- as.data.frame(No_depth)
	No_depth <- tail(No_depth, n = (No_int))
	mean_no <- mean(No_depth[[1]])
	n_mean_no <- (mean_no/Theight)*100
	count_1 <- list()
	for(row in 1:nrow(Sim_df)) { 
		x <- sum(Sim_df[row,] == "1")
		count_1 <- c(count_1, x)}
		count_1 <- unlist(count_1)
		count_0 <- (count_1 - 32)*-1
		gpc <- c(count_1, count_0)
		mgpc <- mean(gpc)
		BC <- bimodality_coefficient(gpc, finite = TRUE)
	newrow <- data.frame("Tree_no." = i, "Treeshape" = shapevalue, "RI" = RI_value, "N_int_nodes" = No_int, "T_height" = Theight, "N_min_BL" = n_minbl, "N_max_BL" = n_maxbl, "N_mean_BL" = n_meanbl, "N_mean_no" = n_mean_no, "mean_gpc" = mgpc, "bimodality_coefficient" = BC)
	TS_RI <- (rbind (TS_RI, newrow)) 
	cat("...\n")
	}
TS_RI <- TS_RI[-c(1), ] 
write.csv(TS_RI, file = "TS_RI_results.csv")
p_tshape <- ggplot(TS_RI, aes(Treeshape)) +
	geom_density() + xlim(0, ((Ntip(Sim_tre)-1)*(Ntip(Sim_tre)-2)/2))
p_RI <- ggplot(TS_RI, aes(RI)) +
	geom_density() + xlim(0, 1)
p_BL <- ggplot(TS_RI, aes(N_mean_BL)) +
	geom_density() + xlim(0, 50)
ggsave("p_tshape.png", p_tshape)
ggsave("p_BL.png", p_BL)
ggsave("p_RI.png", p_RI)

cat(end)