# R script to access convergence. Written by J. Keating 2018.

# Create blank data frame

cbayes_stats <- data.frame(run=0, TL_avgESS=0, alpha_avgESS=0, TL_PSRF=0, alpha_PSRF=0, Converged="n/a")

# Begin loop

for(i in 0:199) {

	# Read .pstat file

	file_pstat <- paste(i,".pstat", sep = "")
	pstat <- read.delim(file_pstat, skip=1)
	
	# If ESS >200 and PSRF is between 0.9 and 1.1, mark as converged
	
    if((2*(pstat[1, "avgESS"]) > 200) & (2*(pstat[2, "avgESS"]) > 200) & ((pstat[1, "PSRF"]) > 0.95) & ((pstat[1, "PSRF"]) < 1.05) & ((pstat[2, "PSRF"]) > 0.90) & ((pstat[2, "PSRF"]) < 1.10)){
			x <- "Y" 
			} else {x <- "N"}
			
	# Create new row of data frame
		
	newrow <- data.frame(run = i, TL_avgESS=(pstat[1, "avgESS"]), alpha_avgESS=(pstat[2, "avgESS"]), TL_PSRF=(pstat[1, "PSRF"]), alpha_PSRF=(pstat[2, "PSRF"]), Converged=x)
    newrow
	cbayes_stats <- rbind(cbayes_stats, newrow)
	}
	cbayes_stats <- cbayes_stats[-c(1), ] 
    L = cbayes_stats$Converged == "N"
    cbayes_stats[L,]
    
    # write CSV
    
    write.csv(cbayes_stats, file = "pstat_values.csv")
    write.csv(cbayes_stats[L,], file = "not_converged.csv")
    
    # cat % of runs that have converged
    
cat((100-(((sum(cbayes_stats$Converged == "N"))/(nrow(cbayes_stats)))*100)),"% of runs converged!")