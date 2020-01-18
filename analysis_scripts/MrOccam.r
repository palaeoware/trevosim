library(phangorn)
library(plotly)
library(mcmcse)

ngen <- 100000
samp <- 10
morph_data <- read.nexus.data("morph.nex")
levelInData <- levels(as.factor(unlist(morph_data))) 
dataset <- phyDat(morph_data, type="USER", levels=levelInData)
tree1 <- rtree((length(dataset)), tip.label = names(dataset), rooted = FALSE, br = NULL)
tree2 <- rtree((length(dataset)), tip.label = names(dataset), rooted = FALSE, br = NULL)
tree3 <- rtree((length(dataset)), tip.label = names(dataset), rooted = FALSE, br = NULL)
tree4 <- rtree((length(dataset)), tip.label = names(dataset), rooted = FALSE, br = NULL)
tree_out<-tree1
score1<-parsimony(tree1, dataset)
score2<-parsimony(tree2, dataset)
score3<-parsimony(tree3, dataset)
score4<-parsimony(tree4, dataset)
mcmc<-data.frame(gen=0, score_c1=score1, score_c2=score2, score_c3=score3, score_c4=score4)
pb <- txtProgressBar(min = 0, max = ngen, style = 3)				# create progress bar
for(i in 1:ngen){
	x1 <- sample(0:1000000, 1)										# random number
	x2 <- sample(0:1000000, 1)
	x3 <- sample(0:1000000, 1)
	x4 <- sample(0:1000000, 1)
	x5 <- sample(0:1000000, 1)												
	tree1b <- rSPR(tree1, moves = 1)
	tree2b <- rNNI(tree2, moves = 1)
	tree3b <- rNNI(tree3, moves = 1)
	tree4b <- rNNI(tree4, moves = 1)
	score1b<-parsimony(tree1b, dataset)
	score2b<-parsimony(tree2b, dataset)
	score3b<-parsimony(tree3b, dataset)
	score4b<-parsimony(tree4b, dataset)
	score_diff1<-(score1b - score1)
	score_diff2<-(score2b - score2)
	score_diff3<-(score3b - score3)
	score_diff4<-(score4b - score4)	
	
		
	if(score_diff1 < 0){
	tree1 <- tree1b
	score1 <- score1b
	} else {
		if(x1 < (500000/((score_diff1+1)^2))){
		tree1 <- tree1b
		score1 <- score1b
   		} 
	}
	if(score_diff2 < 0){
	tree2 <- tree2b
	score2 <- score2b
	} else {
		if(x2 < (500000/((score_diff2+1)))){
		tree2 <- tree2b
		score2 <- score2b
   		} 
	}
	if(score_diff3 < 0){
	tree3 <- tree3b
	score3 <- score3b
	} else {
		if(x3 < (500000/((score_diff3+1)^(1/2)))){
		tree3 <- tree3b
		score3 <- score3b
   		} 
	}
	if(score_diff4 < 0){
	tree4 <- tree4b
	score4 <- score4b
	} else {
		if(x4 < (500000/((score_diff4+1)^(1/3)))){
		tree4 <- tree4b
		score4 <- score4b
   		} 
	}
	R1 <- sample(2:4, 1)										# swap trees and scores between cold chain and random hot chain according to scorediff5 
	score_diff5 <- (get(paste("score", R1,  sep="")) - score1)
	if(score_diff5 < 0){
		treeT <- tree1
		scoreT <- score1
		tree1 <- get(paste("tree", R1,  sep=""))
		score1 <- get(paste("score", R1,  sep=""))
		assign(paste("score", R1,  sep=""), scoreT)
		assign(paste("tree", R1,  sep=""), treeT)
	} else {
		if(x5 < (500000/((score_diff5+1)^(1/3)))){
		treeT <- tree1
		scoreT <- score1
		tree1 <- get(paste("tree", R1,  sep=""))
		score1 <- get(paste("score", R1,  sep=""))
		assign(paste("score", R1,  sep=""), scoreT)
		assign(paste("tree", R1,  sep=""), treeT)
   		} 
	}
		
	if(i%%samp==0){													# sample accordng to samp
	tree_out<-c(tree_out, tree1)
	newrow <- data.frame(gen=i, score_c1=score1, score_c2=score2, score_c3=score3, score_c4=score4)
	mcmc <- (rbind (mcmc, newrow))} 
	setTxtProgressBar(pb, i)										# update progress bar
	}
	
	
	
close(pb)
cat("Discarding 25% burnin")
burnin<-tail(tree_out, n=7500)
p <- plot_ly(mcmc, x = ~gen, y = ~score_c1, type = 'scatter', mode = 'lines', name = 'C1 - cold chain') %>%
    add_trace(y = ~score_c2, name = 'C2 - hot+',mode = 'lines') %>%
    add_trace(y = ~score_c3, name = 'C3 - hot++', mode = 'lines') %>%
    add_trace(y = ~score_c4, name = 'C4 - hot+++', mode = 'lines')
p
con_tree <- consensus(burnin, p = 0.5)
write.nexus(con_tree, file=con_tree.tre)
ess(mcmc$score_c1)