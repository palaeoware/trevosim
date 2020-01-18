# R script to calculate tree and data properties. Written by J. Keating 2018.

# Create blank data frame

TS_RI2 <- data.frame("Tree_no." = 0, "Treeshape" = 0, "RI" = 0, "CI" = 0, "sd_RI" = 0, "sd_CI" = 0, "N_int_nodes" =0, "T_height" =0, "N_min_BL" =0, "N_max_BL" =0, "N_mean_BL" =0, "N_mean_no" =0)
end = "RI calculated!" 
Pro = "Processing: Tree" 
library(phangorn)
library(apTreeshape)
library(ggplot2)
library(adephylo)
library(stringr)
library(modes)

# Begin loop

for(i in 0:199) {
	cat(Pro, (sprintf( "%d", i)))
	pad<-str_pad(i, 3, pad = "0")
	
	# Read data and tree file
	
	file_nex1 <- paste(i,"_sim.nex", sep = "")
	file_nex2 <- paste("F",i+1, sep = "")
	Sim_tre <- read.nexus(file_nex1)
	Sim_data <- read.nexus.data(file_nex2) 
	
	# Convert matrix to data.frame
	
	Sim_df <- as.data.frame(Sim_data)
	
	# Calculate Colless' index
	
	Sim_shape <- as.treeshape(Sim_tre)
	shapevalue <- colless(Sim_shape)	
	Sim_tre_ur <- unroot(Sim_tre)
	
	# Convert matrix to phydat fromat
	
	levelInData <- levels(as.factor(unlist(Sim_data))) 
	phyDat_format <- phyDat(Sim_data, type="USER", levels=levelInData)
	
	# Get ensemble CI and RI values and per site CI and RI values
	
	CI_value <- CI(Sim_tre_ur, phyDat_format, cost = NULL, sitewise = FALSE) # iterate over every tree. RI.	
	RI_value <- RI(Sim_tre_ur, phyDat_format, cost = NULL, sitewise = FALSE) # iterate over every tree. RI.
	CI_all <- CI(Sim_tre_ur, phyDat_format, cost = NULL, sitewise = TRUE)
	RI_all <- RI(Sim_tre_ur, phyDat_format, cost = NULL, sitewise = TRUE)
	CI_var <- sd(CI_all)
	RI_var <- sd(RI_all)
	
	# Get branch length and node height data
	
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
	
	# Create new row of data frame
	
	newrow <- data.frame("Tree_no." = i, "Treeshape" = shapevalue, "RI" = RI_value, "CI" = CI_value, "sd_RI" = RI_var, "sd_CI" = CI_var, "N_int_nodes" = No_int, "T_height" = Theight, "N_min_BL" = n_minbl, "N_max_BL" = n_maxbl, "N_mean_BL" = n_meanbl, "N_mean_no" = n_mean_no)
	TS_RI2 <- (rbind (TS_RI2, newrow)) 
	cat("...\n")
	}
TS_RI2 <- TS_RI2[-c(1), ] 
cat(end)