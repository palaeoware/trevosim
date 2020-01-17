ptm <- proc.time()
end = "Distances calculated!" 
Pro = "Processing: Tree" 
library(phangorn)
library(treeman)
library(doMC)
library(foreach)
registerDoMC(cores=n)
registerDoMC(cores=n)
	listOne <- read.nexus('mol.run1.t.nex') # your mol trees list one 
	listTwo <- read.nexus('morph.run1.t.nex') # your morph trees list two
	listThree <- read.nexus('TNT_tree_xinact.nex') # your morph trees list three
	listFour <- read.nexus('TNT_tree_xinact_piwe.nex') # your morph trees list four
	listOne <- unroot(listOne)
	listTwo <- unroot(listTwo)
	listThree <- unroot(listThree) 
	listFour <- unroot(listFour)
	
	TMOne <-list()
	for(i in 1:length(listOne)){
	TMOne[[i]] <- as(listOne[[i]], 'TreeMan')   
	}
	
	TMTwo <-list()
	for(i in 1:length(listTwo)){
	TMTwo[[i]] <- as(listTwo[[i]], 'TreeMan')   
	}
	
	TMThree <-list()
	for(i in 1:length(listThree)){
	TMThree[[i]] <- as(listThree[[i]], 'TreeMan')   
	}
	
	TMFour <-list()
	for(i in 1:length(listFour)){
	TMFour[[i]] <- as(listFour[[i]], 'TreeMan')   
	}

# Trip Bayesian
	
	# LOOP
pb <- txtProgressBar(min = 0, max = length(TMOne), style = 3)				# create progress bar
res <- foreach(i=1:length(TMOne)) %dopar% {
  prt <- vector('list', length=length(TMTwo))
  for(j in 1:length(TMTwo)) {
    prt[[j]] <- calcDstRF(tree_1=TMOne[[i]],
                           tree_2=TMTwo[[j]],
                           nrmlsd=TRUE)
  }
  prt <- unlist(prt)
  prt
  setTxtProgressBar(pb, i)													# update progress bar
	}
}

# CONVERT TO DF
dresultTD1 <- data.frame(res)
molmorph <- apply(dresultTD1, 1, FUN=min)
morphmol <- apply(dresultTD1, 2, FUN=min)
meanTD <- mean(molmorph)
meanTD <- append(meanTD,(mean(morphmol)))
TDbayes <- mean(meanTD)
	
# Trip Parsimony
	
	# LOOP
pb <- txtProgressBar(min = 0, max = length(TMOne), style = 3)				# create progress bar
res <- foreach(i=1:length(TMOne)) %dopar% {
  prt <- vector('list', length=length(TMThree))
  for(j in 1:length(TMThree)) {
    prt[[j]] <- calcDstRF(tree_1=TMOne[[i]],
                           tree_2=TMThree[[j]],
                           nrmlsd=TRUE)
  }
  prt <- unlist(prt)
  prt
  setTxtProgressBar(pb, i)													# update progress bar
	}
}

# CONVERT TO DF
dresultTD1 <- data.frame(res)
molmorph <- apply(dresultTD1, 1, FUN=min)
morphmol <- apply(dresultTD1, 2, FUN=min)
meanTD <- mean(molmorph)
meanTD <- append(meanTD,(mean(morphmol)))
TDpars <- mean(meanTD)	
	

# Trip IW
	
	# LOOP
pb <- txtProgressBar(min = 0, max = length(TMOne), style = 3)				# create progress bar
res <- foreach(i=1:length(TMOne)) %dopar% {
  prt <- vector('list', length=length(TMFour))
  for(j in 1:length(TMFour)) {
    prt[[j]] <- calcDstRF(tree_1=TMOne[[i]],
                           tree_2=TMFour[[j]],
                           nrmlsd=TRUE)
  }
  prt <- unlist(prt)
  prt
  setTxtProgressBar(pb, i)													# update progress bar
	}
}

# CONVERT TO DF
dresultTD1 <- data.frame(res)
molmorph <- apply(dresultTD1, 1, FUN=min)
morphmol <- apply(dresultTD1, 2, FUN=min)
meanTD <- mean(molmorph)
meanTD <- append(meanTD,(mean(morphmol)))
TDIW <- mean(meanTD)		



distances <- data.frame(Trip_Bayesian=TDbayes, Trip_Parsimony=TDPars, Trip_IW=TDIW)
write.csv(distances, file = "distances_Trip.csv")
cat(end)
df_res <- head(distances)
df_res