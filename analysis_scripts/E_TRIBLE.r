TS_RI <- data.frame("Tree_no." = 0, "Treeshape" = 0, "N_Treeshape" = 0, "RI" = 0, "N_int_nodes" =0, "T_height" =0, "N_min_BL" =0, "N_max_BL" =0, "N_mean_BL" =0, "N_mean_no" =0)
end = "RI calculated!" 
Pro = "Processing: Tree" 
library(phangorn)
library(phytools)
library(apTreeshape)
library(ggplot2)
library(adephylo)
mol_trees <- read.nexus("mol.run1.t.nex")
morph_data <- read.nexus.data("morph.nex")
mol_trees <- root(mol_trees, 1:2, resolve.root = TRUE)
for(i in 1:length(mol_trees)) {
	cat(Pro, (sprintf( "%d", i)))
	Tr_shape <- as.treeshape(mol_trees[[i]])
	shapevalue <- colless(Tr_shape)
	levelInData <- levels(as.factor(unlist(morph_data))) 
	phyDat_format <- phyDat(morph_data, type="USER", levels=levelInData)
	RI_value <- RI(mol_trees[[i]], phyDat_format, cost = NULL, sitewise = FALSE) # iterate over every tree. RI.
	No_int <- Nnode(mol_trees[[i]], internal.only = TRUE)
	Theight <- max(distRoot(mol_trees[[i]]))
	meanbl <- mean(mol_trees[[i]]$edge.length)
	maxbl <- max(mol_trees[[i]]$edge.length)
	minbl <- min(mol_trees[[i]]$edge.length)
	max_tshape <- (Ntip(mol_trees[[i]])-1)*(Ntip(mol_trees[[i]]))
	n_Tshape <- (shapevalue/max_tshape)*100
	n_meanbl <- (meanbl/Theight)*100
	n_maxbl <- (maxbl/Theight)*100
	n_minbl <- (minbl/Theight)*100
	No_depth <- node.depth.edgelength(mol_trees[[i]])
	No_depth <- - (No_depth - max(node.depth.edgelength(mol_trees[[i]])))
	No_depth <- as.data.frame(No_depth)
	No_depth <- tail(No_depth, n = (No_int))
	mean_no <- mean(No_depth[[1]])
	n_mean_no <- (mean_no/Theight)*100
	newrow <- data.frame("Tree_no." = i, "Treeshape" = shapevalue, "N_Treeshape" = n_Tshape, "RI" = RI_value, "N_int_nodes" = No_int, "T_height" = Theight, "N_min_BL" = n_minbl, "N_max_BL" = n_maxbl, "N_mean_BL" = n_meanbl, "N_mean_no" = n_mean_no)
	TS_RI <- (rbind (TS_RI, newrow)) 
	cat("...\n")
	}
TS_RI <- TS_RI[-c(1), ] 
write.csv(TS_RI, file = "TS_RI_results.csv")
cat(end)
