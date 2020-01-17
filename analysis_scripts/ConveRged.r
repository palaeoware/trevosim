cbayes_stats <- data.frame(run=0, TL_avgESS=0, alpha_avgESS=0, TL_PSRF=0, alpha_PSRF=0, Converged="n/a")
for(i in 250:499) {
	file_pstat <- paste(i,".pstat", sep = "")
    pstat <- read.delim(file_pstat, skip=1) #  iterate over every pstat file, read pstat as data.frame
    if((2*(pstat[1, "avgESS"]) > 200) & (2*(pstat[2, "avgESS"]) > 200) & ((pstat[1, "PSRF"]) > 0.95) & ((pstat[1, "PSRF"]) < 1.05) & ((pstat[2, "PSRF"]) > 0.95) & ((pstat[2, "PSRF"]) < 1.05)){
			x <- "Y" 
			} else {x <- "N"}
		
	newrow <- data.frame(run = i, TL_avgESS=(pstat[1, "avgESS"]), alpha_avgESS=(pstat[2, "avgESS"]), TL_PSRF=(pstat[1, "PSRF"]), alpha_PSRF=(pstat[2, "PSRF"]), Converged=x)
    newrow
	cbayes_stats <- rbind(cbayes_stats, newrow)
	}
	cbayes_stats <- cbayes_stats[-c(1), ] 
    L = cbayes_stats$Converged == "N"
    cbayes_stats[L,]
    write.csv(cbayes_stats, file = "pstat_values.csv")
    write.csv(cbayes_stats[L,], file = "not_converged.csv")
cat((100-(((sum(cbayes_stats$Converged == "N"))/(nrow(cbayes_stats)))*100)),"% of runs converged!")