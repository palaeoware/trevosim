#include "simulation.h"
#include "version.h"

#include <QRandomGenerator>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QTimer>
#include <QCoreApplication>

simulation::simulation(int runsCon, const simulationVariables *simSettingsCon, bool *error, MainWindow *theMainWindowCon, bool calculateStripUninformativeFactorRunningCon)
{
    /***** Set up GUI, and working log *****/
    theMainWindow = theMainWindowCon;
    runs = runsCon;
    simSettings = simSettingsCon;

    if ((simSettings->genomeSize < simSettings->fitnessSize) || (simSettings->genomeSize < simSettings->speciesSelectSize))
    {
        warning("Error", "Genome size is smaller than fitness or species select size, failed to initialise simulation");
        *error = true;
        qInfo("Simulation initialisation error 1");
        return;
    }

    //Setup function returns false if failed, else true
    if (!setupSaveDirectory())
    {
        warning("Error", "Can't set up save directory, failed to initialise simulation");
        *error = true;
        qInfo("Simulation initialisation error 2");
        return;
    }

    if (simSettings->writeRunningLog)
        if (!setupSaveDirectory("TREvoSim_running_log_"))
        {
            warning("Permissions issue", "Unable to set up write log folder. Will terminate.");
            *error = true;
            return;
        }

    //To check all works as expected there is a work log that writes everything to a text file
    QString workLogFilename = (QString(PRODUCTNAME) + "_working_log_");
    if (theMainWindow == nullptr) workLogFilename.append(doPadding(runs, 3));
    workLogFilename.append(".txt");
    workLogFilename.prepend(savePathDirectory.absolutePath() + QDir::separator());
    workLogFile.setFileName(workLogFilename);
    if (simSettings->workingLog)
    {
        if (!workLogFile.open(QIODevice::QIODevice::WriteOnly | QIODevice::Text))
        {
            warning("Error!", "Error opening working log file to write to.");
            *error = true;
            return;
        }
        else
        {
            workLogTextStream.setDevice(&workLogFile);
            workLogTextStream << "Run initiated\n\nSettings are : " << simSettings->printSettings() << "\n\n";
        }
    }

    //Get system max rand
    maxRand = QRandomGenerator::max();

    /***** Apply settings *****/
    //In some settings, genome size or mask number etc. will change - but this risks messing with a global. Other variables can change due to rounding from multiplying/dividing by float. Hence keep copy of the actual values and reset at end to make sure nothing changes
    runGenomeSize = simSettings->genomeSize;
    runSelectSize = simSettings->speciesSelectSize;
    runFitnessSize = simSettings->fitnessSize;
    runFitnessTarget = simSettings->fitnessTarget;
    runMaskNumber = simSettings->maskNumber;
    runEnvironmentNumber = simSettings->environmentNumber;
    runSpeciesDifference = simSettings->speciesDifference;
    runMixingProbabilityOneToZero = simSettings->mixingProbabilityOneToZero;
    runMixingProbabilityZeroToOne = simSettings->mixingProbabilityZeroToOne;

    // Initialise variables
    speciesCount = 0, iterations = 0;
    informativeCharacters = 0;

    // If stripping uninformative characters, in order to have the requested char # need to over-generate, and then subsample to requested value
    calculateStripUninformativeFactorRunning = calculateStripUninformativeFactorRunningCon;
    if (simSettings->stripUninformative && !calculateStripUninformativeFactorRunning)
    {
        double stripUninformativeFactorLocal = simSettings->stripUninformativeFactor;
        if (stripUninformativeFactorLocal < 1.5) stripUninformativeFactorLocal = 5;

        double runGenomeSizeDouble = static_cast<double>(runGenomeSize);
        runGenomeSizeDouble *= stripUninformativeFactorLocal;
        runGenomeSize = static_cast<int>(runGenomeSizeDouble);

        double runSelectSizeDouble = static_cast<double>(runSelectSize);
        runSelectSizeDouble *= stripUninformativeFactorLocal;
        runSelectSize = static_cast<int>(runSelectSizeDouble);

        double runFitnessSizeDouble = static_cast<double>(runFitnessSize);
        runFitnessSizeDouble *= stripUninformativeFactorLocal;
        runFitnessSize = static_cast<int>(runFitnessSizeDouble);

        double runFitnessTargetDouble = static_cast<double>(runFitnessTarget);
        runFitnessTargetDouble *= stripUninformativeFactorLocal;
        runFitnessTarget = static_cast<int>(runFitnessTargetDouble);

        //I suspect it makes sense to do this so it still hits expected behaviour - but note that that means we can't guaruntee species difference if stripping uninformative characters
        double runSpeciesDifferenceDouble = static_cast<double>(runSpeciesDifference);
        runSpeciesDifferenceDouble *= stripUninformativeFactorLocal;
        runSpeciesDifference = static_cast<int>(runSpeciesDifferenceDouble);
    }

    /***** Setup and populate playing field *****/
    for (int p = 0; p < simSettings->playingfieldNumber; p++) playingFields.append(new playingFieldStructure);
    //This should either be to playingfieldSize if PFs do not expand
    if (!simSettings->expandingPlayingfield)
        for (auto p : std::as_const(playingFields))
            for (int i = 0; i < simSettings->playingfieldSize; i++)
                p->playingField.append(new Organism(runGenomeSize, simSettings->stochasticLayer));
    //Or just one, if they do expand
    else
        for (auto p : std::as_const(playingFields))
            p->playingField.append(new Organism(runGenomeSize, simSettings->stochasticLayer));
    //Note organisms are populated here, but actually set during the initialisation function

    /***** Setup and populate masks *****/

    //If we need to add a mask down the line due to EE it makes sense to set it up here
    if (simSettings->ecosystemEngineersAddMask) runMaskNumber++;

    //3 vectors to allow list of bools, with multiple user defined lists per environment, then multiple environments (some structures have fourth for each playing field)
    //So this is playing field number / environment number / mask number / bits
    //This will fill the masks using random numbers as requried - if we need to match fitness peaks, we will do so below.
    if (!simSettings->matchFitnessPeaks)
        for (auto p : std::as_const(playingFields))
        {
            for (int k = 0; k < runEnvironmentNumber; k++)
            {
                p->masks.append(QVector <QVector <bool> >());
                for (int j = 0; j < runMaskNumber; j++)
                {
                    p->masks[k].append(QVector <bool>());
                    for (int i = 0; i < runFitnessSize; i++)
                    {
                        if (QRandomGenerator::global()->generate() > (maxRand / 2)) p->masks[k][j].append(bool(false));
                        else  p->masks[k][j].append(bool(true));
                    }
                }
            }
        }

    //And then if we need to make sure fitness peaks are the same height, we need to initialise with the same number of 1s in any given bit for each environment (i.e. bit zero, then bit one, and so on)
    //These can be shifted between masks, but the nature of our fitness algorithm means that the minimum will always depend on the number of ones and zeros at a given point
    //If we have multiple playing fields with independent masks, for now, let's only do this within each playing field
    else
    {
        //First let's populate them with false / 0 bits
        for (auto p : std::as_const(playingFields))
        {
            for (int k = 0; k < runEnvironmentNumber; k++)
            {
                p->masks.append(QVector <QVector <bool> >());
                for (int j = 0; j < runMaskNumber; j++)
                {
                    p->masks[k].append(QVector <bool>());
                    for (int i = 0; i < runFitnessSize; i++) p->masks[k][j].append(bool(false));
                }
            }
        }

        //Now let's do the ones
        //We need to shuffle their position between environments to allow multiple fitness peaks - keep a record
        for (auto p : std::as_const(playingFields))
        {
            QVector <QVector <bool> > used;
            for (int i = 0; i < runEnvironmentNumber; i++)
            {
                used.append(QVector <bool> ());
                for (int j = 0; j < runFitnessSize; j++) used[i].append(false);
            }

            for (int i = 0; i < runFitnessSize; i++)
            {
                //Note we want to have the number of ones to be the mask number plus one - so that e.g. if we have two masks, because we're looping through less than below, we need to have a limit of two
                int numberOnes = QRandomGenerator::global()->bounded(runMaskNumber + 1);

                for (int k = 0; k < runEnvironmentNumber; k++)
                {
                    bool flag = false;
                    int bitPosition = -1;
                    do
                    {
                        bitPosition = QRandomGenerator::global()->bounded(runFitnessSize);
                        if (used[k][bitPosition] == false) flag = true;
                    }
                    while (flag == false);

                    int count = 0;
                    used[k][bitPosition] = true;

                    while (count < numberOnes)
                    {
                        int mask =  QRandomGenerator::global()->bounded(runMaskNumber);
                        if (p->masks[k][mask][bitPosition] == false)
                        {
                            p->masks[k][mask][bitPosition] = true;
                            count++;
                        }
                    }
                }
            }
        }
    }

    //If playing field masks should start the same, copy of playing fields over
    if (simSettings->playingfieldNumber > 1 && simSettings->playingfieldMasksMode != MASKS_MODE_INDEPENDENT)
        for (int p = 1; p < simSettings->playingfieldNumber; p++)
            playingFields[p]->masks = playingFields[0]->masks;

    //If we are adding as mask for EE, we don't need to do antyhing with this yet
    if (simSettings->ecosystemEngineersAddMask) runMaskNumber--;

    /***** Set up stuff for perturbations and ecosystem engineers *****/
    perturbationStart = 0, perturbationEnd = 0, perturbationOccurring = 0;
    ecosystemEngineeringOccurring = 0;
    //Copy rate for environmental perturbation
    environmentalPerturbationCopyRate = 0;
}

bool simulation::run()
{
    QString aliveRecordString ("Alive record is taken at the end of each iteration, so any species which have gone extinct during that iteration due to mixing or overwriting from a setling new Organism will not feature on the alive list.\nEach line lists: Iteration\tPlaying field\tSpecies alive\n\n");
    QTextStream aliveRecordTextStream(&aliveRecordString);
    Organism bestOrganism = initialise();

    //Populate playing field with clones
    quint32 notFound = static_cast<quint32>(-1);
    if (static_cast<quint32>(bestOrganism.fitness) == notFound)
    {
        warning("Oops", "This has not initialised this correctly. Please try different settings or email RJG.");
        return false;
    }
    else for (auto p : std::as_const(playingFields))
            for (auto o : std::as_const(p->playingField)) *o = bestOrganism;
    \

    if (simSettings->test == 3) return true;

    //Species list to store details of each new species
    QVector <Organism *> speciesList;

    // Above is also the first species, obvs.
    speciesList.append(new Organism(runGenomeSize, simSettings->stochasticLayer));
    *speciesList[0] = *playingFields[0]->playingField[0];
    //It is also not extinct
    extinctList.append(false);

    if (simSettings->workingLog)workLogTextStream << "\n\nMasks initiated:\n" <<  printMasks(playingFields) << "\n\n";

    //Create string to record tree in parantheses format
    QString TNTstring;

    //Need to run loop until hit species number or iteration number - track using a bool
    bool simulationComplete = false;

    //Need to track when EE are first applied in order to apply with known frequency
    int EEstart = 0;

    QElapsedTimer timer;
    timer.start();
    int lastGUIUpdate = 0;

    //If we are incrementing environment numbers, we need to start with 1
    if (simSettings->incrementEnvironments && simSettings->test != 19)runEnvironmentNumber  = 1;

    /************* Start simulation *************/
    do
    {

        //Pause if required....
        if (theMainWindow != nullptr)
            while (theMainWindow->pauseFlag == true && !theMainWindow->escapePressed)
                qApp->processEvents();

        if (theMainWindow != nullptr)
            if (theMainWindow->escapePressed)
            {
                clearVectors(playingFields, speciesList);
                return false;
            }

        if (simSettings->workingLog)
        {
            workLogTextStream << "\n\nSimulation running. Iteration: " << iterations << "\n\n" << printPlayingField(playingFields) << "\n";
            workLogTextStream << "\nCurrent tree prior to speciation:" << TNTstring << "\n\nCurrent species list:\n";
            workLogTextStream << "\nSpecies list:" << printSpeciesList(speciesList) << "\n";
            qApp->processEvents();
        }

        int playingFieldNumber = -1;
        //Do the iteration for all playing fields
        for (auto pf : std::as_const(playingFields))
        {
            /******** Sort playing field by fitness *****/
            //std::sort(playingField.begin(),playingField.end(),[](const Organism* OL, const Organism* OR){return OL->fitness<OR->fitness;});
            //qSort(playingField.begin(),playingField.end());
            //This has been removed, because sorting not strictly required - quicker option selected of going doing dice toss, then going down list, finding fittest, etc.
            //If recycled needs to be updted to work with multiple playing fields.

            //Need to keep track of which playing field we're on for calling fitness function - for now
            playingFieldNumber++;
            //If we have enough species from speciation in playing field one, no need to run this all again (and causes a crash thanks to printing to screen)
            if (simSettings->runMode == RUN_MODE_TAXON && speciesCount == simSettings->runForTaxa - 1) break;

            int select = coinToss(pf);

            //Need to duplicate one
            Organism progeny = *pf->playingField[select];
            mutateOrganism(progeny, pf);
            int parentSpecies = pf->playingField[select]->speciesID;

            //If we're using an expanding playingfield there is always one individual in playingfield, and they don't disppear until the end of a run
            //Hence instead we define extinction in functional terms - i.e. when they were last selected for replication
            if (simSettings->expandingPlayingfield)
            {
                progeny.extinct = iterations;
                //We need to do this for parent species as well as this selected individual (otherwise tree will have negative brnch lengths)
                speciesList[parentSpecies]->extinct = iterations;
            }

            //Reminder, in initialise prog genome is set equal to genome
            bool isNewSpecies = checkForSpeciation(&progeny, runSelectSize, runSpeciesDifference, simSettings->speciationMode);

            /************* New species *************/
            //Assymetry of tree changes with level of difference below, plus balance between rate of mutation of environment and Organism
            if (isNewSpecies)
            {
                if (simSettings->workingLog) workLogTextStream << "New species has appeared this iteration - species " << progeny.speciesID << " gives birth to " << speciesCount + 1
                                                                   << " at iteration " << iterations << "\n\n";

                //Update iteration counter on the parent species in the species list
                speciesList[parentSpecies]->cladogenesis = iterations;

                //New species iterates species count and does organism end of new species things
                newSpecies(progeny, *pf->playingField[select], pf);
                //Keeping species list within main object, hence below

                //Set parent to record last child species
                speciesList[parentSpecies]->children.append(speciesCount);
                //Add to species list
                speciesList.append(new Organism(runGenomeSize, simSettings->stochasticLayer));
                *speciesList[speciesCount] = progeny;

                //Update tree string, write to GUI. First sort out zero padding.
                updateTNTstring(TNTstring, progeny.parentSpeciesID, progeny.speciesID);

                //Add to the end of the pf if we are in expand mode - a new species always adds to end of PF
                if (simSettings->expandingPlayingfield) pf->playingField.append(new Organism(runGenomeSize, simSettings->stochasticLayer));
            }

            //We will place progeny in playing field at position overwrite
            int overwrite = calculateOverwrite(pf, progeny.speciesID);

            //Overwrite the thing
            *pf->playingField[overwrite] = progeny;
        }

        //Debugging code for outputting playing field
        if (simSettings->workingLog) workLogTextStream << "Playing field post placement of new organism/at end of iteration, if new organism not added:\n"  << printPlayingField(playingFields) << "\n";

        /************* Update displays etc. *************/

        //Increase iteration count
        iterations++;

        /************* Mutate environment then update fitness *************/
        mutateEnvironment();

        //Calculate fitness
        if (!simSettings->noSelection)
            for (int p = 0; p < simSettings->playingfieldNumber; p++)
                for (int i = 0; i < playingFields[p]->playingField.count(); i++)
                    playingFields[p]->playingField[i]->fitness = fitness(playingFields[p]->playingField[i], playingFields[p]->masks, runFitnessSize, runFitnessTarget, runMaskNumber, simSettings->fitnessMode);


        /************* Playing field mixing *************/

        //Implement mixing between playing fields if required - do check for extinction at same time
        if (simSettings->playingfieldNumber > 1 && simSettings->mixing) applyPlayingfieldMixing(speciesList);

        /************* Check for extinction *************/

        //Check and record species genome if it is last surviving instance, for both extinction and speciation.
        //Some redundancy here, but easier approach than waiting for extinction then recovering genome
        // For each species, Loop through playing field and count and when one instance of a species exists, overwrite genome in species list, and update display
        QHash<QString, QVector <int> > extinct = checkForExtinct(speciesList);

        for (auto s : std::as_const(extinct))
        {
            speciesExtinction(speciesList[s[0]], playingFields[s[1]]->playingField[s[2]], (iterations + 1), simSettings->genomeOnExtinction, simSettings->stochasticLayer);
            if (simSettings->workingLog) workLogTextStream << "\nFor write genome at extinction, replacing species list species " << speciesList[s[0]]->speciesID <<
                                                               " that is entry " << s[0] << " with genome of playing field " << s[1] << " " << s[2] << "\n\n";
        }

        /************* Work out halfway point *************/
        bool halfway = false;
        if (simSettings->runMode == RUN_MODE_TAXON && (speciesCount >= simSettings->runForTaxa / 2)) halfway = true;
        else if (simSettings->runMode == RUN_MODE_ITERATION && (iterations >= simSettings->runForIterations / 2)) halfway = true;

        /************* Apply perturbations *************/
        if (halfway && (simSettings->environmentalPerturbation || simSettings->mixingPerturbation) && perturbationOccurring < 2)
        {
            applyPerturbation();
        }

        /************* Apply ecosystem engineering *************/
        int iterationsSinceEEapplications = iterations - EEstart;
        if (halfway && simSettings->ecosystemEngineers && iterationsSinceEEapplications % simSettings->ecosystemEngineeringFrequency == 0)
        {
            ecosystemEngineeringOccurring++;
            if (ecosystemEngineeringOccurring == 1)
            {
                EEstart = iterations;
                if (simSettings->writeEE)
                    if (!setupSaveDirectory("Ecosystem_engineers_"))
                    {
                        warning("Permissions issue", "Unable to set up ecosystem engineer folder. Will terminate.");
                        return false;
                    }
            }
            if (ecosystemEngineeringOccurring == 1 || (ecosystemEngineeringOccurring > 1 && simSettings->ecosystemEngineersArePersistent)) applyEcosystemEngineering(speciesList, simSettings->writeEE);
        }


        /************* Increment environments, if requried *************/

        //If we are incrementing environment numbers, we need to change environment numbers alongg the way
        if (simSettings->incrementEnvironments && simSettings->test != 19)
        {
            if (simSettings->runMode == RUN_MODE_TAXON)
                if (speciesList.length() > (runEnvironmentNumber * (simSettings->runForTaxa / simSettings->environmentNumber)))
                {
                    runEnvironmentNumber++;
                    qDebug() << "Increasing environment number  - iterations " << iterations << " taxa " << speciesList.length() << " environments " << runEnvironmentNumber;
                }
            if (simSettings->runMode == RUN_MODE_ITERATION)
                if (iterations > (runEnvironmentNumber * (simSettings->runForIterations / simSettings->environmentNumber)))
                {
                    runEnvironmentNumber++;
                    qDebug() << "Increasing environment number  - iterations " << iterations << " environments " << runEnvironmentNumber;
                }
        }

        /************* Check if simulation is complete *************/
        if (simSettings->runMode == RUN_MODE_TAXON && (speciesCount + 1) == simSettings->runForTaxa) simulationComplete = true;
        else if (simSettings->runMode == RUN_MODE_ITERATION && iterations == simSettings->runForIterations) simulationComplete = true;

        /************* Write GUI if not running in parallel *************/
        if (theMainWindow != nullptr)
        {
            //Update GUI every five milliseconds
            if ((timer.elapsed() - lastGUIUpdate) > 5)
            {
                writeGUI(speciesList);
                lastGUIUpdate = timer.elapsed();
            }
        }

        /************* Write alive record *************/
        QVector <QVector <int> > speciesAlive;

        for (int n = 0; n < simSettings->playingfieldNumber; n++)
        {
            speciesAlive.append(QVector <int> ());
            for (int i = 0; i < playingFields[n]->playingField.count(); i++)
            {
                bool found = false;
                for (int j = 0; j < speciesAlive[n].length(); j++)
                    if (speciesAlive[n][j] == playingFields[n]->playingField[i]->speciesID)found = true;

                if (!found)speciesAlive[n].append(playingFields[n]->playingField[i]->speciesID);
            }
        }

        for (int i = 0; i < simSettings->playingfieldNumber; i++)
        {
            aliveRecordTextStream << iterations << "\t" << i << "\t";
            for (int j = 0; j < speciesAlive[i].length(); j++)aliveRecordTextStream << speciesAlive[i][j] << "\t";

            aliveRecordTextStream << "\n";
        }

        /************* Write running log *************/
        if (simSettings->writeRunningLog && (iterations % simSettings->runningLogFrequency == 0) && !calculateStripUninformativeFactorRunning)
        {
            //We need to do most of the text processing here, as many of the things we may want to report in a running log are local in scope to the run function
            //Text is stored in the simulation settings as runningLogString
            QString logTextOut = simSettings->runningLogString;

            if (logTextOut.contains("||Unresolvable||") || logTextOut.contains( "||Identical||"))
            {
                int unresolvableCount = 0;
                QString unresolvableTaxonGroups;
                checkForUnresolvableTaxa(speciesList, unresolvableTaxonGroups, unresolvableCount);
                logTextOut.replace("||Unresolvable||", unresolvableTaxonGroups, Qt::CaseInsensitive);
                logTextOut.replace("||Identical||", QString::number(unresolvableCount), Qt::CaseInsensitive);

            }

            if (logTextOut.contains("||Uninformative||"))
            {
                //Work out which are uninformative- create list
                QList <int> uninformativeCoding;
                QList <int> uninformativeNonCoding;

                //Test for informative
                checkForUninformative(speciesList, uninformativeCoding, uninformativeNonCoding);
                int uninformativeNumber = uninformativeCoding.length() + uninformativeNonCoding.length();
                logTextOut.replace("||Uninformative||", QString::number(uninformativeNumber), Qt::CaseInsensitive);
            }

            if (logTextOut.contains("||Alive_Record||"))
            {
                QString aliveRecordIterationString;

                for (int i = 0; i < simSettings->playingfieldNumber; i++)
                    for (int j = 0; j < speciesAlive[i].length(); j++)
                        aliveRecordIterationString.append(QString(QString::number(speciesAlive[i][j]) + "\t"));
                logTextOut.replace("||Alive_Record||", aliveRecordIterationString);
            }

            logTextOut.replace("||TNT_Tree||", TNTstring, Qt::CaseInsensitive);
            logTextOut.replace("||MrBayes_Tree||", printNewickWithBranchLengths(0, speciesList, false), Qt::CaseInsensitive);
            logTextOut.replace("||Stochastic_Matrix||", printStochasticMatrix(speciesList, simSettings->stochasticLayer), Qt::CaseInsensitive);
            logTextOut.replace("||Ecosystem_Engineers||", printEcosystemEngineers(speciesList), Qt::CaseInsensitive);
            logTextOut.replace("||Character_Number||", QString::number(runGenomeSize), Qt::CaseInsensitive);
            logTextOut.replace("||Taxon_Number||",  QString::number(speciesList.length()), Qt::CaseInsensitive);
            logTextOut.replace("||Count||", doPadding(runs, 3));
            logTextOut.replace("||Matrix||", printMatrix(speciesList), Qt::CaseInsensitive);
            logTextOut.replace("||Time||", printTime(), Qt::CaseInsensitive);
            logTextOut.replace("||Settings||", simSettings->printSettings(), Qt::CaseInsensitive);
            logTextOut.replace("||Iteration||", QString::number(iterations), Qt::CaseInsensitive);
            logTextOut.replace("||Root||", printGenomeString(&bestOrganism), Qt::CaseInsensitive);
            logTextOut.replace("||PlayingField_Number||", QString::number(simSettings->playingfieldNumber), Qt::CaseInsensitive);
            logTextOut.replace("||PlayingField_Size||", QString::number(simSettings->playingfieldSize), Qt::CaseInsensitive);
            logTextOut.replace("||PlayingField||", printPlayingField(playingFields), Qt::CaseInsensitive);
            logTextOut.replace("||PlayingField_semiconcise||", printPlayingFieldSemiconcise(playingFields), Qt::CaseInsensitive);
            logTextOut.replace("||PlayingField_concise||", printPlayingFieldConcise(playingFields), Qt::CaseInsensitive);
            logTextOut.replace("||PlayingField_genomes_concise||", printPlayingFieldGenomesConcise(playingFields), Qt::CaseInsensitive);
            logTextOut.replace("||Masks||", printMasks(playingFields), Qt::CaseInsensitive);

            bool writeRunningLogSuccess = writeRunningLog(iterations, logTextOut);

            if (theMainWindow != nullptr)
            {
                if (!writeRunningLogSuccess)
                {
                    warning("Permissions issue", QString("Failed to write running log at iteration %1 - check permissions.").arg(iterations));
                    return false;
                }
            }
        }
    }
    while (!simulationComplete);

    if (simSettings->workingLog) workLogTextStream << "\n\nMasks at end of run:\n" << printMasks(playingFields) << "\n";


    /************* Mop up writing genome for any organisms still alive under genomeOnExtinction and for extinction/branch lengths *************/
    //QVector list for each species ID
    QVector <int> alive;

    //For alive organisms need to record genome of one that is alive (rather than when it was born - in extreme case zero could not have died and have genome from beginning of run)
    //Best to record one of the fittest - hence on this occasion, for ease, going to sort playing field
    //Only works with c++ 11 thanks to lamda - hence addition of CONFIG += c++11 in .pro
    for (int p = 0; p < simSettings->playingfieldNumber; p++) std::sort(playingFields[p]->playingField.begin(), playingFields[p]->playingField.end(), [](const Organism * OL, const Organism * OR)
    {
        return OL->fitness < OR->fitness;
    });

    //Go down playing fields
    for (auto pf : std::as_const(playingFields))
        for (auto o : std::as_const(pf->playingField))
        {
            int flag = 0;
            //Check if the entry on playing field has previously been recorded/updated - if so ignore
            for (int i = 0; i < alive.length(); i++)if (alive[i] == o->speciesID)flag = 1;

            if  (flag == 0)
            {
                //Update the extinction and if required, genome.
                int species = o->speciesID;

                alive.append(species);
                if (simSettings->genomeOnExtinction)
                {
                    for (int k = 0; k < runGenomeSize; k++)speciesList[species]->genome[k] = o->genome[k];
                }
                //If expanding playingfield everything will be alive - thus extinction recorded differently
                if (!simSettings->expandingPlayingfield)speciesList[species]->extinct = iterations;
            }
        }

    //Everything is extinct now - update this prior to GUI
    for (auto &s : extinctList) s = true;
    if (theMainWindow != nullptr) writeGUI(speciesList);

    if (simSettings->workingLog)
    {
        workLogTextStream << "\n\nRun is finished. Now have some cleaning up to do.\n\n";
        workLogTextStream << "\nCurrent tree: " << TNTstring << "\n\nSpecies list at end is:\n" << printSpeciesList(speciesList) << "\n\n";
    }

    /************* Strip uninformative characters, if required *************/
    //Needed to make a decision here and arbitrarily decide how to deal with partations used for defining fitness, species, and the whole genome.
    //At present split into two partitions - that used for fitness calculation, and that not, and treat individually.
    //Don't think preserving that partition used for species definition is so important, so ignoring - but may well prove to be wrong.

    //Work out which are uninformative - create list
    QList <int> uninformativeCoding;
    QList <int> uninformativeNonCoding;

    //Test for informative characters - this is a seperate operation to actually stripping them
    checkForUninformative(speciesList, uninformativeCoding, uninformativeNonCoding);

    int uninformativeNumber = uninformativeCoding.length() + uninformativeNonCoding.length();
    informativeCharacters = runGenomeSize - uninformativeNumber;
    //If calculateStripUninformativeFactor is running, all we care about is uninformativeCharacters, so return here
    if (calculateStripUninformativeFactorRunning) return true;

    if (theMainWindow != nullptr && !simSettings->stripUninformative && uninformativeNumber > 0)
    {
        QString statusString =
            QString ("There are %1 uninformative characters in your matrix, and the software is not set to strip these out. This is not necessarily a problem, but I thought you should know.").arg(
                uninformativeNumber);
        theMainWindow->setStatus(statusString);
    }

    //Strip the characters if requried
    if (simSettings->stripUninformative)
    {
        //Check whether we have enough coding and non coding characters
        bool requiredCharacterNumber = checkForCharacterNumber(uninformativeCoding, uninformativeNonCoding);
        if (!requiredCharacterNumber && !simSettings->test)
        {
            if (theMainWindow != nullptr)
            {
                QString label = "It seems there are not enough informative characters to pull this off.\n\n"
                                "By default, TREvoSim over generates characters by a factor of 5x before trying to strip down to those that are parsimony uninformative. "
                                "Under your current settings (in which the strip uninformative factor is " + QString::number(simSettings->stripUninformativeFactor) +
                                ") TREvoSim has not managed to recover enough informative characters. It has only recovered " + QString::number(speciesList[0]->genome.size()) +
                                " characters. Choosing the menu option \'Recalculate uninformative factor for current settings\' will allow you to recalculate this factor for the current settings, and \'Set uninformative factor' will allow you to set it manually to a large number.\n\n"
                                "Alternatively, this may be a one off - you could try running a batch of 1, and the program will try repeatedly with these settings - though after ten or more repeats you may want to cancel and change the settings.";
                warning("Oops", label);
            }
            if (simSettings->workingLog) workLogTextStream << "Return at !requiredCharacterNumber\n";
            clearVectors(playingFields, speciesList);
            return false;
        }

        bool stripped = stripUninformativeCharacters(speciesList, uninformativeCoding, uninformativeNonCoding);
        if (!stripped)
        {
            clearVectors(playingFields, speciesList);
            return false;
        }
    }

    /******** Check for intrisnically unresolvable clades *****/
    int unresolvableCount = 0;
    QString unresolvableTaxonGroups;
    bool unresolvable = checkForUnresolvableTaxa(speciesList, unresolvableTaxonGroups, unresolvableCount);
    if (!unresolvable) return false;

    /************* Write files *************/
    QHash<QString, QString> outValues;
    outValues["TNTstring"] = TNTstring;
    outValues["aliveRecordString"] = aliveRecordString;
    outValues["unresolvableTaxonGroups"] = unresolvableTaxonGroups;
    outValues["MrBayes_Tree"] = printNewickWithBranchLengths(0, speciesList, false);
    outValues["Stochastic_Matrix"] = printStochasticMatrix(speciesList, simSettings->stochasticLayer);
    outValues["Ecosystem_Engineers"] = printEcosystemEngineers(speciesList);
    outValues["Uninformative"] = QString::number(uninformativeNumber);
    outValues["Identical"] = QString::number(unresolvableCount);
    outValues["Character_Number"] = QString::number(runGenomeSize);
    outValues["Taxon_Number"] = QString::number(speciesList.length());
    outValues["Count"] = doPadding(runs, 3);
    outValues["Root"] = printGenomeString(&bestOrganism);
    outValues["PlayingField_Number"] = QString::number(simSettings->playingfieldNumber);
    outValues["||PlayingField_Size||"] = QString::number(simSettings->playingfieldSize);

    if (simSettings->writeFileOne && simSettings->test == 0)
        if (!writeFile(simSettings->logFileNameBase01, simSettings->logFileExtension01, simSettings->logFileString01, outValues, speciesList))
        {
            warning("Error!", "Error opening output file 1 to write to.");
            clearVectors(playingFields, speciesList);
            return false;
        }

    if (simSettings->writeFileTwo && simSettings->test == 0)
        if (!writeFile(simSettings->logFileNameBase02, simSettings->logFileExtension02, simSettings->logFileString02, outValues, speciesList))
        {
            warning("Error!", "Error opening output file 2 to write to.");
            clearVectors(playingFields, speciesList);
            return false;
        }

    QString fileNameString03;
    fileNameString03 = simSettings->logFileNameBase03;
    fileNameString03.append(doPadding(runs, 3));
    fileNameString03.append(simSettings->logFileExtension03);
    fileNameString03.prepend(savePathDirectory.absolutePath() + QDir::separator());

    if (simSettings->writeTree  && simSettings->test == 0)
    {
        //File 03 is tree file in .nex format - without zero padding
        QFile file03(fileNameString03);
        if (!file03.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            warning("Error!", "Error opening treefile to write to.");
            clearVectors(playingFields, speciesList);
            return false;
        }

        QTextStream file03TextStream(&file03);

        QString logFileString03Tmp(simSettings->logFileString03);

        logFileString03Tmp.replace("||Time||", printTime());
        logFileString03Tmp.replace("||Settings||", simSettings->printSettings());

        file03TextStream << logFileString03Tmp;
        for (int i = 0; i < speciesList.count(); i++)
        {
            file03TextStream << (QString("%1").arg(i + 1));
            file03TextStream << "\t\t" << "Species_" << doPadding(i, paddingAmount(speciesList.length())) << ",\n";
        }

        file03TextStream << "\t\t;\n\ntree tree1 = [&R]";
        QString newickString(printNewickWithBranchLengths(0, speciesList, true));
        // Remove text for phangorn
        newickString.remove("Species_000");
        newickString.remove("Species_00");
        newickString.remove("Species_0");
        newickString.remove("Species_");

        file03TextStream << newickString << ";\n\nEND;";
        file03.close();
    }

    if (simSettings->workingLog)
    {
        workLogTextStream << "On exit, " << (QString(PRODUCTNAME)) << " thinks tree is " << printNewickWithBranchLengths(0, speciesList, false) << "\n or in TNT format: " << TNTstring;
        workLogFile.close();
    }

    //Sort memory
    if (simSettings->test == 0)
    {
        clearVectors(playingFields, speciesList);
    }

    return true;
}

/************** Simulation functions *************/

Organism simulation::initialise()
{
    /***** Initialise the playing field with an individual near a fitness peak *****/
    //Want first organism to start somewhere near a fitness peak else short runs will just be a move towards a peak, I think - and not reflect ongoing evolutionary change
    //Obviously this is *not* the case for radiations e.g. terrestrialisation. But those are relatively rarer, I guess?
    //Worth thinking about anyway...

    //Note here that if stochastic layer is enabled, initialise organism calls the mapping function from the initialisation - no need to do it here

    //Record min (best) fitness
    quint32 minimumFitness = static_cast<quint32>(-1);

    Organism bestOrganism = *playingFields[0]->playingField[0];
    int count = 0;

    if (simSettings->playingfieldNumber < 2 || simSettings->playingfieldMasksMode != MASKS_MODE_INDEPENDENT)
    {
        if (!simSettings->randomSeed)
        {
            if (!simSettings->matchFitnessPeaks)
                do
                {
                    //First organism - initialise and fill playing field with it
                    if (simSettings->stochasticLayer) playingFields[0]->playingField[0]->initialise(runGenomeSize, simSettings->stochasticMap);
                    else playingFields[0]->playingField[0]->initialise(runGenomeSize);

                    playingFields[0]->playingField[0]->fitness = fitness(playingFields[0]->playingField[0], playingFields[0]->masks, runFitnessSize, runFitnessTarget, runMaskNumber, simSettings->fitnessMode);

                    if (static_cast<quint32>(playingFields[0]->playingField[0]->fitness) < minimumFitness)
                    {
                        bestOrganism = *playingFields[0]->playingField[0];
                        minimumFitness = static_cast<quint32>(bestOrganism.fitness);
                    }
                    count++;
                }
                while (count < 200);
            //If we have match fitness peaks, we probably want our individuals to have similar fitnesses across environment
            else
            {
                quint32 minimumSumDifferences = ~0;
                do
                {
                    //First organism - initialise and fill playing field with it
                    if (simSettings->stochasticLayer) playingFields[0]->playingField[0]->initialise(runGenomeSize, simSettings->stochasticMap);
                    else playingFields[0]->playingField[0]->initialise(runGenomeSize);

                    //Work out fitnesses for all environments - to see if they are the same
                    QVector <int> fitnesses;

                    for (int i = 0; i < runEnvironmentNumber;
                            i++) fitnesses.append(fitness(playingFields[0]->playingField[0], playingFields[0]->masks, runFitnessSize, runFitnessTarget, runMaskNumber, simSettings->fitnessMode, i));
                    int sumOfDifferences = 0;
                    for (int i = 0; i < fitnesses.length() - 1; i++) sumOfDifferences += qAbs(fitnesses[i] - fitnesses[i + 1]);

                    //Then work out overall fitness
                    playingFields[0]->playingField[0]->fitness = fitness(playingFields[0]->playingField[0], playingFields[0]->masks, runFitnessSize, runFitnessTarget, runMaskNumber, simSettings->fitnessMode);

                    //We care most about having similar fitnesses - work towards getting identical fitnesses on all playingfields
                    if (static_cast<quint32>(sumOfDifferences) < minimumSumDifferences)
                    {
                        bestOrganism = *playingFields[0]->playingField[0];
                        minimumFitness = static_cast<quint32>(bestOrganism.fitness);
                        minimumSumDifferences = static_cast<quint32>(sumOfDifferences);
                    }
                    //If, however, the sum of the differences is the same (most likely zero) - then we want the smallest
                    else if ((static_cast<quint32>(sumOfDifferences) == minimumSumDifferences) && (static_cast<quint32>(playingFields[0]->playingField[0]->fitness) < minimumFitness))
                    {
                        bestOrganism = *playingFields[0]->playingField[0];
                        minimumFitness = static_cast<quint32>(bestOrganism.fitness);
                    }
                    count++;
                }
                while (count < 2000);
            }
        }
        else
        {
            if (simSettings->stochasticLayer) playingFields[0]->playingField[0]->initialise(runGenomeSize, simSettings->stochasticMap);
            else playingFields[0]->playingField[0]->initialise(runGenomeSize);
            bestOrganism = *playingFields[0]->playingField[0];
        }
    }
    //Need to initialise sensibly if there are different masks for different playing fields
    else
    {
        if (theMainWindow != nullptr) theMainWindow->setStatus("Initialialising simulation - given the non-identical masks, this will take a few seconds.");
        qApp->processEvents();
        if (theMainWindow != nullptr) theMainWindow->addProgressBar(0, 5000);

        do
        {
            //First organism - initialise and fill playing field with it
            if (simSettings->stochasticLayer) playingFields[0]->playingField[0]->initialise(runGenomeSize, simSettings->stochasticMap);
            else playingFields[0]->playingField[0]->initialise(runGenomeSize);
            playingFields[0]->playingField[0]->fitness = fitness(playingFields[0]->playingField[0], playingFields[0]->masks, runFitnessSize, runFitnessTarget, runMaskNumber, simSettings->fitnessMode);

            //Given that playing field masks are different, now we need to initialise with the best organism we can for all.
            //Currently implemented using the best mean fitness of all organisms tried
            QVector <int> fitnesses;

            for (auto p : std::as_const(playingFields))
            {
                fitnesses.append(fitness(p->playingField[0], p->masks, runFitnessSize, runFitnessTarget, runMaskNumber, simSettings->fitnessMode));
            }

            double meanFitness = 0;

            for (auto i : fitnesses) meanFitness += i;

            meanFitness = static_cast<double>(meanFitness) / static_cast<double>(fitnesses.count());

            if (static_cast<quint32>(meanFitness) < minimumFitness)
            {
                bestOrganism = *playingFields[0]->playingField[0];
                minimumFitness = static_cast<quint32>(meanFitness);
            }
            count++;
            if (count % 100 == 0 && theMainWindow != nullptr) theMainWindow->setProgressBar(count);
        }
        while (count < 5000);

        if (theMainWindow != nullptr) theMainWindow->hideProgressBar();
    }

    return bestOrganism;
}

int simulation::coinToss(const playingFieldStructure *pf)
{
    //Which organism to select
    int select = -1;

    if (simSettings->noSelection) select = QRandomGenerator::global()->bounded(pf->playingField.count());
    else
    {
        //Move down the list and select one - make it likely it is near the top, so has good fitness (i.e. near level zero)
        //Currently 50% chance it'll choose the first, and so on, a la coin toss
        int marker = 0;

        //Note that simSettings->selectionCoinToss is a double - hence all the casting below
        while (static_cast<double>(QRandomGenerator::global()->generate()) > (static_cast<double>(maxRand) / simSettings->selectionCoinToss)) if (++ marker >= pf->playingField.count())marker = 0;

        //Do this by using marker to work out which nth value from fittest we want to select, then find where this is in playing field and assign to array
        QVector <bool> ignoreList(pf->playingField.count(), false);
        QVector <int> foundPool;
        bool found = false;
        int foundCount = 0;

        while (found == false)
        {
            //Find the correct genome to duplicate, rather than sorting
            int smallest[2] {(runGenomeSize * 5), 0};
            for (int i = 0; i < pf->playingField.count(); i++)
            {
                //Go through playing field and find smallest fitness, and record how many there are of this.
                if (!ignoreList[i])
                {
                    if (pf->playingField[i]->fitness < smallest[0])
                    {
                        smallest[0] = pf->playingField[i]->fitness;
                        smallest[1] = 1;
                    }
                    else if (pf->playingField[i]->fitness == smallest[0])
                        smallest[1]++;
                }
            }

            foundCount += smallest[1];
            //If you need to get e.g. position #2, then the number you have to find is 3 - 0,1 and 2, hence >
            if (foundCount > marker)found = true;

            for (int i = 0; i < pf->playingField.count(); i++)
            {
                //If found smallest, add all organisms with best fit to environment to a list to select between
                if (found && pf->playingField[i]->fitness == smallest[0])foundPool.append(i);
                else if (!found && pf->playingField[i]->fitness == smallest[0])ignoreList[i] = true;
            }
        }

        //Sort out found pool here - create select from a random member of the found pool
        //Random number up to size of pool
        int integerPointSelect = QRandomGenerator::global()->bounded(foundPool.count());
        select = foundPool[integerPointSelect];
    }
    return select;
}

void simulation::mutateOrganism(Organism &progeny, const playingFieldStructure *pf)
{
    //Mutate
    int temporaryFitness = progeny.fitness;

    //Work out number - need 4x if stochastic layer present
    double mutations = 0.0;
    if (simSettings->stochasticLayer) mutations = (static_cast<double>(runGenomeSize * 4) / 100.) * simSettings->organismMutationRate;
    else mutations = (static_cast<double>(runGenomeSize ) / 100.) * simSettings->organismMutationRate;

    double organismMutationIntegral = static_cast<int>(mutations);
    double fractional = modf(mutations, &organismMutationIntegral);
    int mutationNumber = static_cast<int>(organismMutationIntegral);
    //Due to rounding the above always results in fewer mutations than would be expected - the -110 below ensures an average of 1 mutation per 100 base pairs
    if (static_cast<double>(QRandomGenerator::global()->generate()) < (fractional * static_cast<double>(maxRand - 110)))mutationNumber++;

    QVector <int> SNPs;

    //Apply mutation - note that this does allow multiple hits (and therefore potentially no change) as currently implemented
    for (int i = 0; i < mutationNumber; i++)
    {
        //Scale random number to genome size
        int mutationPositionInteger = 0;
        if (simSettings->stochasticLayer) mutationPositionInteger = QRandomGenerator::global()->bounded(runGenomeSize * 4);
        else mutationPositionInteger = QRandomGenerator::global()->bounded(runGenomeSize);

        SNPs.append(QRandomGenerator::global()->bounded(runGenomeSize));

        if (simSettings->stochasticLayer) progeny.stochasticGenome[mutationPositionInteger] = !progeny.stochasticGenome[mutationPositionInteger];
        else progeny.genome[mutationPositionInteger] = !progeny.genome[mutationPositionInteger];
    }

    if (simSettings->stochasticLayer)
    {
        progeny.mapFromStochastic(simSettings->stochasticMap);
        if (simSettings->workingLog && simSettings->stochasticLayer)
        {
            workLogTextStream << "Now doing stochastic mapping for selected progeny. Post mutation genome is\n" ;
            QString genomeString;

            for (auto i : std::as_const(progeny.genome))
                if (i)genomeString.append("1");
                else genomeString.append("0");

            workLogTextStream << genomeString << "\nAnd this comes from mapping the stochastic layer:\n";

            genomeString.clear();

            for (int i = 0; i < progeny.stochasticGenome.count() ; i++)
            {
                if (progeny.stochasticGenome[i])genomeString.append("1");
                else genomeString.append("0");
                if ((i + 1) % 4 == 0)genomeString.append(" ");
            }

            workLogTextStream << genomeString << "\n\n";
        }
    }

    //Update fitness
    progeny.fitness = fitness(&progeny, pf->masks, runFitnessSize, runFitnessTarget, runMaskNumber, simSettings->fitnessMode);

    //Undo mutations if discard deleterious is on, and new fitness is worse than old
    if (simSettings->discardDeleterious && (temporaryFitness < progeny.fitness))
    {
        if (simSettings->stochasticLayer)
        {
            for (int i = 0; i < SNPs.count(); i++)progeny.stochasticGenome[SNPs[i]] = !progeny.stochasticGenome[SNPs[i]];
            progeny.mapFromStochastic(simSettings->stochasticMap);
        }
        else for (int i = 0; i < SNPs.count(); i++)progeny.genome[SNPs[i]] = !progeny.genome[SNPs[i]];
    }
}

void simulation::mutateEnvironment()
{
    //Set our mutation rate
    double localMutationRate = simSettings->environmentMutationRate;
    //If we are matching peaks, we want the mutation rate to be halved because we will need to switch a zero to a one and one to a zero or vice versa.
    //So every mutation is two bit changes
    if (simSettings->matchFitnessPeaks) localMutationRate /= 2;

    //As per docs, mutations are set per 100 bits in the genome - calculate this for each environment, as environments treated separately
    //How many bits? Use run fitness size here because there are only environmental bits for the bits of the genome that contribute to fitness
    int totalBitsPerEnvironment = runFitnessSize * runMaskNumber;

    //Calculate mutation # as previously, and using same variables for ease - this is the number of mutations total for each mask
    double numberEnvironmentMutationsDouble = (static_cast<double>(totalBitsPerEnvironment) / 100.) * localMutationRate;
    //This will be used to store the integral part of the above sum - it needs to be a double as this is what is passed to the modf function
    double numberEnvironmentMutationsIntegral = numberEnvironmentMutationsDouble;

    //Sort out the probabilities of extra mutation given remainder
    double numberEnvironmentMutationsFractional = modf(numberEnvironmentMutationsDouble, &numberEnvironmentMutationsIntegral);
    int numberEnvironmentMutationsInteger = (static_cast<int>(numberEnvironmentMutationsIntegral));

    //note that due to saturation / multiple hits on one site, the number of recoreded mutations in e.g. our tests may sneak in under the expected value
    if (static_cast<double>(QRandomGenerator::global()->generate()) < (numberEnvironmentMutationsFractional * static_cast<double>(maxRand))) numberEnvironmentMutationsInteger++;

    if (simSettings->matchFitnessPeaks)
    {
        //If we are matching peaks, it's best to think of bit positions as columns. We want to shuffle around columns in the 'x' direction to achieve on swapped bit (= two bit changes, hence the half rate above - it is impossible to do this without swapping two bits)
        //The way this is organised, we want to do this within each environment
        //reminder: masks[environment #][mask #][bit #]

        for (auto pf : std::as_const(playingFields)) // Treat playing fields separately
            for (int j = 0; j < runEnvironmentNumber; j++) //Treat environments separately
            {
                //Create lists of columns separated by one bit to do swap
                QList <int> pairOne;
                QList <int> pairTwo;

                //Used to do the exhaustively, but this was massive overkill for most settings, and made the function slooooow
                //Now use heuristic approach with an appopriate escape and error message
                int count = 0;
                do
                {
                    int firstBit = QRandomGenerator::global()->bounded(runFitnessSize);
                    int secondBit = QRandomGenerator::global()->bounded(runFitnessSize);
                    if (firstBit == secondBit) continue;

                    int bitDifference = 0;
                    for (int n = 0; n < runMaskNumber; n++)
                        if (pf->masks[j][n][firstBit] != pf->masks[j][n][secondBit]) bitDifference++;

                    if (bitDifference == 1)
                        if (!pairOne.contains(firstBit) && !pairOne.contains(secondBit) && !pairTwo.contains(firstBit) && !pairTwo.contains(secondBit))
                        {
                            pairOne.append(firstBit);
                            pairTwo.append(secondBit);
                        }
                    count++;
                }
                while (pairOne.length() < numberEnvironmentMutationsInteger && count < 10000);

                //Add warning if there are not enough columns to swap: with any decent size genome, I don't expect this to happen all that much
                if (pairOne.length() < numberEnvironmentMutationsInteger)
                {
                    warning("Oops", "There has been an error at mutating the environment with matching peaks - not enough pairs. Returning with no mutations made.");
                    return;
                }
                else
                {
                    //Now apply the mutations
                    for (int x = 0; x < numberEnvironmentMutationsInteger; x++) //How many mutations?
                    {
                        //Swap one pair of columns
                        int swap1 = pairOne[x];
                        int swap2 = pairTwo[x];
                        for (int m = 0; m < runMaskNumber; m++)
                        {
                            bool storeBit = pf->masks[j][m][swap1];
                            pf->masks[j][m][swap1] = pf->masks[j][m][swap2];
                            pf->masks[j][m][swap2] = storeBit;
                        }
                    }
                }
            }
    }
    else
    {
        for (auto pf : std::as_const(playingFields)) // Treat playing fields separately
            for (int j = 0; j < runEnvironmentNumber; j++) //Treat environments separately
                for (int x = 0; x < numberEnvironmentMutationsInteger; x++) //How many mutations?
                {
                    //Select random mask and random bit
                    int mutationPosition = QRandomGenerator::global()->bounded(runFitnessSize);
                    int maskNumber = QRandomGenerator::global()->bounded(runMaskNumber);
                    pf->masks[j][maskNumber][mutationPosition] = !pf->masks[j][maskNumber][mutationPosition];
                }
    }


    //Copy between PFs if they are set to be identical
    if ( simSettings->playingfieldNumber > 1 && simSettings->playingfieldMasksMode == MASKS_MODE_IDENTICAL)
        for (int p = 1; p < simSettings->playingfieldNumber; p++)
            playingFields[p]->masks = playingFields[0]->masks;
}


void simulation::updateTNTstring(QString &TNTstring, int progParentSpeciesID, int progSpeciesID)
{
    QString progenySpeciesID;
    if (simSettings->runMode == RUN_MODE_TAXON) progenySpeciesID = doPadding(progParentSpeciesID, paddingAmount(simSettings->runForTaxa));
    else progenySpeciesID = doPadding(progParentSpeciesID, 4);

    QString speciesID;
    if (simSettings->runMode == RUN_MODE_TAXON) speciesID = doPadding(progSpeciesID, paddingAmount(simSettings->runForTaxa));
    else speciesID = doPadding(progSpeciesID, 4);

    //Then write string
    //If first iteration, write manually so as to avoid starting with excess brackets.
    if (speciesCount == 1 && simSettings->runForTaxa < 100 && simSettings->runMode == RUN_MODE_TAXON) TNTstring = "(00 01)";
    else if (speciesCount == 1 && simSettings->runForTaxa < 1000 && simSettings->runMode == RUN_MODE_TAXON)TNTstring = "(000 001)";
    else if (speciesCount == 1) TNTstring = "(0000 0001)";
    else
    {
        //Then split string at the ancestral species - first search for the progenator
        int position = TNTstring.indexOf(progenySpeciesID);

        //You should never find a bracket or not find anything
        if (position < 1) warning("Eesh", "You shouldn't see this. There's been an error writing the tree string. Please contact RJG in the hope he can sort this out.");

        // Split tree file at this point, into left, and right of position.
        QString treeLeft = TNTstring.left(position);
        QString treeRight = TNTstring.right(TNTstring.length() - (position + progenySpeciesID.length()));

        //Strip spaces if required - is not for TNT...
        //if(treeLeft.at(treeLeft.length()-1)==' ')treeLeft.remove(treeLeft.length()-1,1);
        //if(treeRight.at(0)==' ')treeRight.remove(0,1);

        //Build new string
        QString ins = QString("(%1 %2)").arg(progenySpeciesID, speciesID);
        TNTstring = treeLeft + ins + treeRight;
    }
}

//This returns the number in the playingfield that needs to be overwritten
int simulation::calculateOverwrite(const playingFieldStructure *pf, const int speciesNumber)
{
    int overwrite = -1;
    //First we can deal with expanding playingfield cases
    //In both (new species or not) we have overwrite our chosen species in the playingfield
    if (simSettings->expandingPlayingfield) overwrite = speciesNumber;
    //Then this is not the case, we can deal with the non expanding playingfield cases
    //We have the option of a random overwrite
    else if (simSettings->randomOverwrite)
    {
        overwrite = QRandomGenerator::global()->bounded(pf->playingField.count());
        if (overwrite == pf->playingField.count()) overwrite = pf->playingField.count() - 1;
    }
    //Or overwriting the least fit (or one of the least fit)
    else
    {
        //Now settle by overwriting least fit one, or if >1 lowest fitness, but overwriting a randomly chosen one of these.
        QVector <int> deletePool;
        int lowestFitness = -1;

        //Find lowest fitness in playing field
        for (auto o : pf->playingField)
            if (o->fitness > lowestFitness)
                lowestFitness = o->fitness;

        //Write list of least fit organisms
        for (int i = 0; i < pf->playingField.count(); i++)
            if (pf->playingField[i]->fitness == lowestFitness)deletePool.append(i);


        if (deletePool.count() == 1) overwrite = deletePool[0];
        else
        {
            //Scale random number to size of pool
            int deletePositionInteger = QRandomGenerator::global()->bounded(deletePool.count());
            overwrite = deletePool[deletePositionInteger];
        }
    }
    return overwrite;
}

void simulation::applyPerturbation()
{
    //Set up environmental perturbation if required
    if (simSettings->environmentalPerturbation && perturbationOccurring == 0)
    {
        //Create copy of masks
        for (int l = 0; l < simSettings->playingfieldNumber; l++)
        {
            environmentalPerturbationMasksCopy.append(QVector <QVector <QVector <bool> > >());
            environmentalPerturbationMasksCopy[l] = playingFields[l]->masks;
            environmentalPerturbationOverwriting.append(QVector <QVector <QVector <bool> > >());
            for (int k = 0; k < runEnvironmentNumber; k++)
            {
                environmentalPerturbationOverwriting[l].append(QVector <QVector <bool> >());
                for (int j = 0; j < runMaskNumber; j++)
                {
                    environmentalPerturbationOverwriting[l][k].append(QVector <bool>());
                    for (int i = 0; i < runFitnessSize; i++)
                    {
                        environmentalPerturbationOverwriting[l][k][j].append(bool(false));
                    }
                }
            }
        }


        if (simSettings->workingLog) workLogTextStream << "\n\nAbout to set up environmental perturbation. Old masks are:\n\n" << printPlayingField(playingFields) << "\n";

        //Bork current masks to create environmental perturbation - makes sense to have these as independent and different unless pf masks set to be identical
        for (auto pf : std::as_const(playingFields))
            for (int k = 0; k < runEnvironmentNumber; k++)
                for (int j = 0; j < runMaskNumber; j++)
                    //I is reference so as to allow us to modify contents
                    for (auto &i : pf->masks[k][j])
                        if (QRandomGenerator::global()->generate() > (maxRand / 2)) i = true;
                        else i = false;
        //Copy over if identical
        if (simSettings->playingfieldMasksMode == MASKS_MODE_IDENTICAL)
            for (int i = 1; i < simSettings->playingfieldNumber; i++)
                for (int k = 0; k < runEnvironmentNumber; k++)
                    for (int j = 0; j < runMaskNumber; j++)
                        for (int l = 0; l < playingFields[i]->masks[k][j].length(); l++)
                            playingFields[i]->masks[k][j][l] = playingFields[0]->masks[k][j][l];

        if (simSettings->workingLog) workLogTextStream << "\n\nSet up environmental perturbation. New masks are:\n\n" << printPlayingField(playingFields) << "\n";
    }

    //Set up mixing perturbation if required
    if (simSettings->mixingPerturbation && perturbationOccurring == 0)
    {
        runMixingProbabilityOneToZero *= 10;
        runMixingProbabilityZeroToOne *= 10;
        if (simSettings->workingLog) workLogTextStream << "\nSet up mixing perturbation.  \n\nsimSettings->mixingProbabilityOneToZero  now " << runMixingProbabilityOneToZero <<
                                                           "\nmixingProbabilityZeroToOne  " << runMixingProbabilityZeroToOne;
    }

    //Do perturbation
    if (perturbationOccurring == 1 && simSettings->environmentalPerturbation)
    {
        int copied = 0;
        if (simSettings->workingLog)
            workLogTextStream << "\nNow applying environmental perturbation to masks. Copying over the following to the masks in addition to those (if any) highlighted in previous iterations\n";

        do
        {
            //Random number size of vectors
            int l = QRandomGenerator::global()->bounded(simSettings->playingfieldNumber);
            int k = QRandomGenerator::global()->bounded(runEnvironmentNumber);
            int j = QRandomGenerator::global()->bounded(runMaskNumber);
            int i = QRandomGenerator::global()->bounded(runFitnessSize);

            //Update
            if (environmentalPerturbationOverwriting[l][k][j][i] == false)
            {
                environmentalPerturbationOverwriting[l][k][j][i] = true;
                copied++;
                if (simSettings->workingLog) workLogTextStream << "\nPlaying field " << l << " Environment number " << k << " Mask number " << j << " Bit number " << i;
            }
        }
        while (copied < environmentalPerturbationCopyRate);

        //Copy over masks where dictated by the the overwriting record
        for (int l = 0; l < simSettings->playingfieldNumber; l++)
            for (int k = 0; k < runEnvironmentNumber; k++)
                for (int j = 0; j < runMaskNumber; j++)
                    for (int i = 0; i < runFitnessSize; i++)
                        if (environmentalPerturbationOverwriting[l][k][j][i])
                            playingFields[l]->masks[k][j][i] = environmentalPerturbationMasksCopy[l][k][j][i];

        //Copy over if identical
        if (simSettings->playingfieldMasksMode == MASKS_MODE_IDENTICAL)
            for (int i = 1; i < simSettings->playingfieldNumber; i++)
                for (int k = 0; k < runEnvironmentNumber; k++)
                    for (int j = 0; j < runMaskNumber; j++)
                        for (int l = 0; l < playingFields[i]->masks[k][j].length(); l++)
                            playingFields[i]->masks[k][j][l] = playingFields[0]->masks[k][j][l];
    }

    //End perturbations
    if (perturbationOccurring == 1 && iterations == perturbationEnd)
    {
        perturbationOccurring++;

        if (simSettings->workingLog) workLogTextStream << "\nPerturbations now ending. Masks will mutate randomly once more, if environmnetal perturbation was selected.\n";

        //End mixing as required
        if (simSettings->mixingPerturbation)
        {
            runMixingProbabilityOneToZero = simSettings->mixingProbabilityOneToZero;
            runMixingProbabilityZeroToOne = simSettings->mixingProbabilityZeroToOne;
            if (simSettings->workingLog) workLogTextStream << "\nMixing perturbation has ended. simSettings->mixingProbabilityOneToZero " << runMixingProbabilityOneToZero <<
                                                               "\nmixingProbabilityZeroToOne now " << runMixingProbabilityZeroToOne;
        }
    }

    //Sort variables for next round as required
    if (!perturbationOccurring)
    {
        perturbationStart = iterations;
        perturbationEnd = (iterations / 10) + iterations;
        perturbationOccurring++;
        if (simSettings->workingLog) workLogTextStream << "\nPerturbations have started this iteration. They will end at iteration " << perturbationEnd << "\n";
        if (simSettings->environmentalPerturbation)
        {
            //Need to copy over 90% of original masks over course of perturbation
            environmentalPerturbationCopyRate = (simSettings->playingfieldNumber * simSettings->environmentNumber * runMaskNumber * runFitnessSize) / (perturbationEnd - perturbationStart);
            environmentalPerturbationCopyRate /= 10;
            environmentalPerturbationCopyRate *= 9;
            if (simSettings->workingLog) workLogTextStream << "\nCopy rate between mask backup and masks until perturbation end will be " << environmentalPerturbationCopyRate <<
                                                               " increase copied bits per iteration.\n";
        }
    }
}

void simulation::applyPlayingfieldMixing(QVector<Organism *> &speciesList)
{
    //We shouldn't be able to get here with only one playing field
    if (playingFields.count() == 1)
    {
        qInfo() << "Error at mix playingfields - should have at least 2 playingfields";
        return;
    }
    //If we have two playing fields, then we allow asymmetrical mixing
    if (simSettings->playingfieldNumber == 2)
    {
        if (QRandomGenerator::global()->bounded(100) < runMixingProbabilityOneToZero)
        {
            int selectOverwrite =  QRandomGenerator::global()->bounded(playingFields[0]->playingField.count());
            int sourceForOverwrite = QRandomGenerator::global()->bounded(playingFields[1]->playingField.count());
            //Assuming overwriting is quicker than doing extinction check for whole playing field to find out if species will be made extinct, especially if latter large.
            //If it's about to go extinct here, then it'll actually be this iteration, not+1 as assumed above
            speciesExtinction(speciesList[playingFields[0]->playingField[selectOverwrite]->speciesID], playingFields[0]->playingField[selectOverwrite], iterations, simSettings->genomeOnExtinction,
                              simSettings->stochasticLayer);
            *playingFields[0]->playingField[selectOverwrite] = *playingFields[1]->playingField[sourceForOverwrite];
            if (simSettings->workingLog)workLogTextStream << "\nReplacing entry " << selectOverwrite << " in playing field 0 with entry " << sourceForOverwrite << " from playing field 1.";
        }

        if (QRandomGenerator::global()->bounded(100) < runMixingProbabilityZeroToOne)
        {
            int selectOverwrite =  QRandomGenerator::global()->bounded(playingFields[1]->playingField.count());
            int sourceForOverwrite = QRandomGenerator::global()->bounded(playingFields[0]->playingField.count());

            speciesExtinction(speciesList[playingFields[1]->playingField[selectOverwrite]->speciesID], playingFields[1]->playingField[selectOverwrite], iterations, simSettings->genomeOnExtinction,
                              simSettings->stochasticLayer);
            *playingFields[1]->playingField[selectOverwrite] = *playingFields[0]->playingField[sourceForOverwrite];
            if (simSettings->workingLog)workLogTextStream << "\nReplacing entry " << selectOverwrite << " in playing field 1 with entry " << sourceForOverwrite << " from playing field 0.";
        }
    }
    //Otherwise we do it symetrically - for each organism in each PF there is a user defined chance of copying one of our organisms to another PF each iteration
    else
    {
        for (int i = 0; i < playingFields.count(); i++)
        {
            //For multiple PFs other than two, we base probability off runMixingProbabilityOneToZero
            if (QRandomGenerator::global()->bounded(100) < runMixingProbabilityOneToZero)
            {
                //Create a list of playingfields we could overwrite in (i.e. all but this one)
                QList <int> overWritePlayingfields;
                for (int j = 0; j < playingFields.count(); j++)
                    if (j != i) overWritePlayingfields.append(j);

                //Select individual to overwrite
                int selectPlayingfield = overWritePlayingfields[QRandomGenerator::global()->bounded(overWritePlayingfields.length())];
                int selectOverwrite = QRandomGenerator::global()->bounded(playingFields[selectPlayingfield]->playingField.count());
                int sourceForOverwrite = QRandomGenerator::global()->bounded(playingFields[i]->playingField.count());
                //Check for extinction
                if (!simSettings->test) speciesExtinction(speciesList[playingFields[selectPlayingfield]->playingField[selectOverwrite]->speciesID], playingFields[selectPlayingfield]->playingField[selectOverwrite],
                                                              iterations, simSettings->genomeOnExtinction, simSettings->stochasticLayer);

                //OVerwerite selected with current organism
                *playingFields[selectPlayingfield]->playingField[selectOverwrite] = *playingFields[i]->playingField[sourceForOverwrite];
                if (simSettings->workingLog)workLogTextStream << "\nReplacing playingfield " << selectPlayingfield << " entry " << selectOverwrite << " with entry " << sourceForOverwrite <<
                                                                  " from playing field " << i << ".\n";
            }
        }
    }
}

void simulation::applyEcosystemEngineering(QVector <Organism *> &speciesList, bool writeEcosystemEngineers)
{
    QString outText;
    QTextStream out(&outText);

    if (writeEcosystemEngineers) out << "Applying ecosystem engineers for " << ecosystemEngineeringOccurring << " time on iteration " << iterations << "\nMasks before ecosystem engineers :\n" <<
                                         printMasks(playingFields) << "\n\nPlaying field(s):\n" << printPlayingFieldSemiconcise(playingFields) << "\n\n";

    //Select then apply an EE for the first time, halfway through run
    if (ecosystemEngineeringOccurring == 1)
    {
        if (simSettings->workingLog)workLogTextStream << "\n\nApplying ecosystem engineers on iteration " << iterations << "\nMasks before ecosystem engineers:\n" <<  printMasks(playingFields) << "\n";

        //Jul 23 - we want to select a single EE from all playing fields, then apply this across all PFs
        int selectEngineerPlayingfield = QRandomGenerator::global()->bounded(playingFields.size());
        int selectEngineerPosition = QRandomGenerator::global()->bounded(playingFields[selectEngineerPlayingfield]->playingField.size());
        playingFields[selectEngineerPlayingfield]->playingField[selectEngineerPosition]->ecosystemEngineer = true;
        speciesList[playingFields[selectEngineerPlayingfield]->playingField[selectEngineerPosition]->speciesID]->ecosystemEngineer = true;

        if (simSettings->workingLog) workLogTextStream << "Playing field " << selectEngineerPlayingfield << " organism number " << selectEngineerPosition << " selected. Genome is " <<
                                                           printGenomeString(playingFields[selectEngineerPlayingfield]->playingField[selectEngineerPosition]) << ".\n";
        if (writeEcosystemEngineers) out << "Playing field " << selectEngineerPlayingfield << " organism number " << selectEngineerPosition <<  " selected. Genome is " <<
                                             printGenomeString(playingFields[selectEngineerPlayingfield]->playingField[selectEngineerPosition]) << ".\n";

        //If EE are meant to add a mask, we should increase the mask number here
        if (simSettings->ecosystemEngineersAddMask) runMaskNumber++;

        for (auto p : std::as_const(playingFields))
        {
            //EE works either by copying over genome to a prexisting mask - thus improving fitness of the EE species - or to the mask added just above (this was initialised with the simulation, but has been ignored until now)
            //Either way, we can write over the last mask for each environment (-1 because indexing starts at zero)
            for (int environmentNumber = 0; environmentNumber < p->masks.count(); environmentNumber++)
                for (int i = 0; i < p->masks[environmentNumber][runMaskNumber - 1].length(); i++)
                    p->masks[environmentNumber][runMaskNumber - 1][i] = playingFields[selectEngineerPlayingfield]->playingField[selectEngineerPosition]->genome[i];

            //Now check for identical taxa in playingfield and mark as ecosystem engineer, as these will also benefit
            for (int i = 0; i < p->playingField.count(); i++)
            {
                if (p->playingField[i]->ecosystemEngineer) continue;
                //Check for identical individuals - including both coding and non coding regions of genome; if identical, these are also EE
                if (genomeDifference(playingFields[selectEngineerPlayingfield]->playingField[selectEngineerPosition], p->playingField[i]) == 0) p->playingField[i]->ecosystemEngineer = true;
            }
        }
        if (simSettings->workingLog)workLogTextStream << "Masks after ecosystem engineers:\n" <<  printMasks(playingFields) << "\n\n";
    }
    //Otherwise, we're beyond halfway through. Select a random EE, and then reapply to all PFs
    else if (ecosystemEngineeringOccurring > 1 && simSettings->ecosystemEngineersArePersistent)
    {

        //31st of July 2023 - code block deleted that allows us to choose the modal EE if we so wish, and use that to overwrite the masks
        //Deleted because this enforces too strong a selective pressure towads the modal genome: not what we want
        //Instead we have chosen to select a random one
        //To recover code block if this changes, just look at git history for commits early on the 31st of July

        if (simSettings->workingLog) workLogTextStream << "Now doing EE again in iteration " << iterations << ".\n";

        QList <QList <int> > organismList;
        organismList.append(QList <int>());
        organismList.append(QList <int>());

        for (int i = 0; i < playingFields.length(); i++)
            //Let's create a list oF EEs - first list pf number, second list position
            for (int j = 0; j < playingFields[i]->playingField.count(); j++)
                if (playingFields[i]->playingField[j]->ecosystemEngineer == true)
                {
                    organismList[0].append(i);
                    organismList[1].append(j);
                }

        //Don't implement if EE have died out
        if (organismList[0].length() == 0)
        {
            if (simSettings->workingLog) workLogTextStream << "EE have died out.\n";
            if (writeEcosystemEngineers) out << "EE have died out.\n";
            return;
        }

        int selectEngineer = QRandomGenerator::global()->bounded(organismList[0].length());

        int selectEngineerPlayingfield = organismList[0][selectEngineer];
        int selectEngineerPosition = organismList[1][selectEngineer];

        //playingFields[selectEngineerPlayingfield]->playingField[selectEngineerPosition]

        if (simSettings->workingLog) workLogTextStream << "Playing field " << selectEngineerPlayingfield << " organism number " << selectEngineerPosition << " selected. Genome is " <<
                                                           printGenomeString(playingFields[selectEngineerPlayingfield]->playingField[selectEngineerPosition]) << ".\n";
        if (writeEcosystemEngineers) out << "Playing field " << selectEngineerPlayingfield << " organism number " << selectEngineerPosition <<  " selected. Genome is " <<
                                             printGenomeString(playingFields[selectEngineerPlayingfield]->playingField[selectEngineerPosition]) << ".\n";

        for (auto p : std::as_const(playingFields))
            for (int environmentNumber = 0; environmentNumber < p->masks.count(); environmentNumber++)
                for (int i = 0; i < p->masks[environmentNumber][runMaskNumber - 1].length(); i++)
                    p->masks[environmentNumber][runMaskNumber - 1][i] = playingFields[selectEngineerPlayingfield]->playingField[selectEngineerPosition]->genome[i];
    }


    if (simSettings->workingLog) workLogTextStream << "Masks after ecosystem engineers :\n" <<  printMasks(playingFields) << "\n\n";

    //Either way, when we're here, we need to write playingfields if required
    if (writeEcosystemEngineers)
    {
        out << "Masks after ecosystem engineers:\n" <<  printMasks(playingFields) << "\n\nPlaying field(s) after ecosystem engineers:\n" << printPlayingFieldSemiconcise(playingFields) << "\n\n";
        writeEEFile(iterations, outText);
    }
}

void simulation::checkForUninformative(QVector <Organism *> &speciesList, QList <int> &uninformativeCoding, QList <int> &uninformativeNonCoding)
{
    for (int i = 0; i < runFitnessSize; i++)
    {
        int count = 0;
        for (int j = 0; j < speciesList.length(); j++) if (speciesList[j]->genome[i])count++;
        if (count < 2 || count > (speciesList.length() - 2)) uninformativeCoding.append(i);
    }
    if (runGenomeSize != runFitnessSize)
        for (int k = runFitnessSize; k < runGenomeSize; k++)
        {
            int count = 0;
            for (int l = 0; l < speciesList.length(); l++) if (speciesList[l]->genome[k])count++;
            if (count < 2 || count > (speciesList.length() - 2)) uninformativeNonCoding.append(k);
        }
}

bool simulation::checkForCharacterNumber(QList <int> &uninformativeCoding, QList <int> &uninformativeNonCoding)
{
    //Deal with informative v.s. uninformative characters
    int requiredNonCodingCharacterNumber = simSettings->genomeSize - simSettings->fitnessSize;
    int requiredCodingCharacterNumber = simSettings->fitnessSize;

    int recoveredNonCodingCharacterNumber = (runGenomeSize - runFitnessSize) - uninformativeNonCoding.length();
    int recoveredCodingCharacterNumber = runFitnessSize - uninformativeCoding.length();

    if (recoveredNonCodingCharacterNumber < requiredNonCodingCharacterNumber || recoveredCodingCharacterNumber < requiredCodingCharacterNumber ) return false;
    else return true;
}

bool simulation::stripUninformativeCharacters(QVector <Organism *> &speciesList, const QList <int> &uninformativeCoding, const QList <int> &uninformativeNonCoding)
{
    if (!simSettings->stripUninformative) return false;

    if (simSettings->workingLog) workLogTextStream << "Stripping uninformativeCoding characters. Prior to removal:\n" << printSpeciesList(speciesList) << "\n";

    //Keep a marker so swtich between coding and non coding is recorded when characters removed (if required).
    int codingGenomeEnd = runFitnessSize;

    //Delete uninformative characters - both coding and non
    for (int j = 0; j < speciesList.length(); j++)
    {
        //Start with noncoding, from end, and work back, to avoid numbering issues post-deletion
        if (runGenomeSize != runFitnessSize)
            for (int h = uninformativeNonCoding.size() - 1; h >= 0; h--) speciesList[j]->genome.removeAt(uninformativeNonCoding[h]);

        //Now do coding, start at end and work back to avoid numbering issues post-deletion.
        for (int i = uninformativeCoding.size() - 1; i >= 0; i--)
        {
            speciesList[j]->genome.removeAt(uninformativeCoding[i]);
            if (j == 0) codingGenomeEnd--;
        }
    }

    //Sort out variables by returning to requested size
    if (simSettings->stripUninformative)
    {
        runGenomeSize = simSettings->genomeSize;
        runSelectSize = simSettings->speciesSelectSize;
        runFitnessSize = simSettings->fitnessSize;
        runSpeciesDifference = simSettings->speciesDifference;
    }

    //Subsample characters from those which are informative to hit requested genome size - fine to just use simSettings->genomeSize of list if all coding
    //If not, need to fill partitions as required to ensure right mix of non-coding and coding - do this by lopping off end of coding genome.
    if (runGenomeSize != runFitnessSize)
    {
        //This has been checked, but let's check again
        if (codingGenomeEnd < runFitnessSize) return false;
        //Subsample coding genome here to requested size
        for (int j = 0; j < speciesList.length(); j++)
            for (int k = codingGenomeEnd; k > runFitnessSize; k--)
                speciesList[j]->genome.removeAt(k);
    }

    //This has been checked, but let's check again
    if (speciesList[0]->genome.size() < runGenomeSize) return false;

    //Now do non coding
    if (speciesList[0]->genome.size() > runGenomeSize)
    {
        for (int j = 0; j < speciesList.length(); j++)
            for (int i = speciesList[j]->genome.size() - 1; i >= runGenomeSize; i--)
                speciesList[j]->genome.removeAt(i);
    }


    if (simSettings->workingLog) workLogTextStream << "After removal:\n" << printSpeciesList(speciesList) << "\n";

    return true;
}

bool simulation::checkForUnresolvableTaxa(QVector<Organism *> &speciesList, QString &unresolvableTaxonGroups, int &unresolvableCount)
{
    bool unresolvable = false;
    unresolvableCount = 0;
    QVector <QVector <int> > unresolvableSpecies;

    if (simSettings->workingLog) workLogTextStream << "Checking for unresolvable clades";

    QString message("A heads up. There are intrinscially unresolvable taxa in this matrix (i.e. at least two taxa have identical genomes); this could affect your results. For more information, check out:\n\nBapst, D.W., 2013. When can clades be potentially resolved with morphology?. PLoS One, 8(4), p.e62312.\n\nThe taxa in question are:");

    //Compare each against each
    for (int i = 0; i < speciesList.length() - 1; i++)
        for (int j = i + 1; j < speciesList.length(); j++)
        {
            int count = 0;
            for (int k = 0; k < runGenomeSize; k++)if (speciesList[i]->genome[k] == speciesList[j]->genome[k])count++;

            //Taxa have identical genomes
            if (count == runGenomeSize)
            {
                unresolvable = true;

                //Clump them into identical clusters
                bool match = false;

                for (int l = 0; l < unresolvableSpecies.count(); l++)
                {
                    int count2 = 0;
                    for (int k = 0; k < runGenomeSize; k++)
                        if (speciesList[unresolvableSpecies[l][0]]->genome[k] == speciesList[i]->genome[k])count2++;

                    //Either add to already existing cluster
                    if (count2 == runGenomeSize && !match)
                    {
                        match = true;

                        bool match2 = false;
                        for (int m = 0; m < unresolvableSpecies[l].length(); m++)
                            if (unresolvableSpecies[l][m] == j)match2 = true;
                        if (!match2)unresolvableSpecies[l].append(j);

                    }
                }

                //Or add new one
                if (!match)
                {
                    QVector <int> newGroup;
                    newGroup.append(i);
                    newGroup.append(j);
                    unresolvableSpecies.append(newGroup);
                }

            }
        }

    //Groups is a string used to write unresolvable clades.
    for (int l = 0; l < unresolvableSpecies.count(); l++)unresolvableCount += unresolvableSpecies[l].length();

    if (unresolvable && theMainWindow != nullptr)
    {
        //Warning - just so people know.
        message.append(QString("\n\n"));

        for (int l = 0; l < unresolvableSpecies.count(); l++)
        {
            for (int m = 0; m < unresolvableSpecies[l].count(); m++) unresolvableTaxonGroups.append(QString("%1 ").arg(unresolvableSpecies[l][m]));
            unresolvableTaxonGroups.append(QString("\n"));
        }

        message.append(unresolvableTaxonGroups);

        if (unresolvableCount > simSettings->unresolvableCutoff)
        {
            message.append(QString("\n\n"));
            message.append(
                QString("In this case, you are above the cutoff of resolvable taxa - there are %1 identical terminals. If you are in batch mode, this will carry on trying until you have hit the requested number of runs. If you're not, you may want to try again (or hit batch, and then enter one run to automatically run it till one sticks).").arg(
                    unresolvableCount));
        }

        if (!simSettings->test && theMainWindow != nullptr) warning("Warning - Unresolvable clades", message);

        if (simSettings->workingLog) workLogTextStream << message;
    }
    else
    {
        unresolvableTaxonGroups = "There are no unresolvable taxa";
        if (simSettings->workingLog) workLogTextStream << "\n\nNo unresolvable clades";
    }

    if (unresolvableCount > simSettings->unresolvableCutoff)
    {
        if (simSettings->workingLog) workLogTextStream << "Return at less than unresolvable cutoff\n";
        return false;
    }
    return true;
}

void simulation::clearVectors(QVector <playingFieldStructure *> &playingFields, QVector <Organism *> &speciesList)
{
    qDeleteAll(playingFields);
    playingFields.clear();
    playingFields.squeeze();

    qDeleteAll(speciesList);
    speciesList.clear();
    speciesList.squeeze();
}

QHash<QString, QVector<int> > simulation::checkForExtinct(const QVector <Organism *> &speciesList)
{

    QHash<QString, QVector <int> > extinct;
    for (int i = 0; i < speciesList.length(); i++)
    {
        int count = 0, index = -1, pindex = -1;

        for (int p = 0; p < simSettings->playingfieldNumber; p++)
            for (int j = 0; j < playingFields[p]->playingField.count(); j++)
                if (playingFields[p]->playingField[j]->speciesID == i)
                {
                    count++;
                    index = j;
                    pindex = p;
                }

        if (count == 1)
        {
            QString label = QString("Species %1").arg(i);
            extinct[label].append(i);
            extinct[label].append(pindex);
            extinct[label].append(index);
        }

        if (count == 0) extinctList[i] = true;
    }
    return extinct;
}

void simulation::speciesExtinction(Organism *speciesListOrganism, const Organism *playingFieldOrganism, int extinctIteration, bool samsonian, bool stochastic, bool test)
{
    if (speciesListOrganism->speciesID != playingFieldOrganism->speciesID)
    {
        if (!simSettings->test) qInfo() << "There is a mismatch between species list species ID and playingfield species ID. Can't do extinction. Species list ID " << speciesListOrganism->speciesID <<
                                            " playingfield species ID " << playingFieldOrganism->speciesID << " born of " << playingFieldOrganism->parentSpeciesID << " at iteration " << playingFieldOrganism->cladogenesis << " current it " <<
                                            iterations;
        return;
    }

    //Record species extinction when we're not on expanding playingfield (if we are, then this results in ultrametric trees - rather record extinction as when last replicated)
    if (!simSettings->expandingPlayingfield)speciesListOrganism->extinct = extinctIteration;
    if (samsonian)
    {
        for (int j = 0; j < speciesListOrganism->genome.size(); j++)speciesListOrganism->genome[j] = playingFieldOrganism->genome[j];
        if (stochastic) for (int j = 0; j < speciesListOrganism->genome.size(); j++) speciesListOrganism->stochasticGenome[j] = playingFieldOrganism->stochasticGenome[j];
    }
}

//Masks passed as a const reference.
int simulation::fitness(const Organism *org, const QVector<QVector<QVector<bool> > > &masks, int runFitnessSize, int runFitnessTarget, int runMaskNumber, int fitnessMode, int environment)
{

    if (!(fitnessMode == FITNESS_MODE_MINIMUM || fitnessMode == FITNESS_MODE_MEAN))
    {
        warning("Fitness error", "Fitness mode error - contact RJG");
        return 0;
    }

    //Send mask number to function, as in some cases (i.e. ecosystem engineering), we don't want to include all masks.
    int maskNumber = runMaskNumber;
    int environmentNumber = masks.length();

    int fitness = (runFitnessSize * maskNumber);
    double doubleFitness = 0.;

    //Environment defaults to -1 (used to allow this to be called throughout simulation without defining environment number).
    //If this is the case check fitness for all environments
    if (environment == -1)
    {
        for (int h = 0; h < environmentNumber; h++)
        {
            int temporaryFitness = ~0, counts = 0;
            for (int i = 0; i < maskNumber; i++)
                for (int j = 0; j < runFitnessSize; j++)
                    if (org->genome[j] != masks[h][i][j])counts++;

            //Define fitness as the distance away from fitness target
            temporaryFitness = qAbs(counts - runFitnessTarget);
            if (fitnessMode == FITNESS_MODE_MINIMUM && temporaryFitness < fitness)fitness = temporaryFitness;
            if (fitnessMode == FITNESS_MODE_MEAN) doubleFitness += (static_cast<double>(temporaryFitness) / static_cast<double>(environmentNumber));
        }
        if (fitnessMode == FITNESS_MODE_MEAN) fitness = static_cast<int>(doubleFitness);
    }
    //Alteranatively, we can calculate fitness for a specific environment
    else
    {
        int counts = 0;
        for (int i = 0; i < maskNumber; i++)
            for (int j = 0; j < runFitnessSize; j++)
                if (org->genome[j] != masks[environment][i][j]) counts++;
        fitness = qAbs(counts - runFitnessTarget);
    }

    return fitness;
}

//Selectsize defaults to -1; currently only used to check for identical organisms in EE algorithm
int simulation::genomeDifference(const Organism *organismOne, const Organism *organismTwo, const int selectSize)
{
    int diff = 0;
    if (selectSize == -1)
    {
        for (int j = 0; j < organismOne->genome.length(); j++)
            if (organismOne->genome[j] != organismTwo->genome[j])diff++;
    }
    else
    {
        for (int j = 0; j < selectSize; j++)
            if (organismOne->genome[j] != organismTwo->genome[j])diff++;
    }
    return diff;
}

bool simulation::checkForSpeciation(const Organism *organismOne, int runSelectSize, int runSpeciesDifference, int speciationMode)
{
    int difference = 0;

    //Speciation modes are defined as follows
    //SPECIES_MODE_ORIGIN 0
    //SPECIES_MODE_LAST_SPECIATION 1
    //SPECIES_MODE_ALL 2
    //SPECIES_MODE_MAYR 3 - not currently implemented

    int genomeCount = organismOne->parentGenomes.count();
    if (speciationMode == SPECIES_MODE_ORIGIN)
    {
        //Loop to select size to allow decoupling of species definition from genome size: also can't just use the count difference function above as this operates on organisms, not their genomes.
        for (int j = 0; j < runSelectSize; j++)
            if (organismOne->genome[j] != organismOne->parentGenomes[0][j])
                difference++;
    }
    else if (speciationMode == SPECIES_MODE_LAST_SPECIATION)
    {
        for (int j = 0; j < runSelectSize; j++)
            if (organismOne->genome[j] != organismOne->parentGenomes[genomeCount - 1][j])
                difference++;
    }
    else if (speciationMode == SPECIES_MODE_ALL)
    {
        //We need to record the maximum level of difference, as the biggest is going to be closest to species difference
        difference = 0;
        for (int i = 0; i < genomeCount; i++)
        {
            int tempDiff = 0;
            for (int j = 0; j < runSelectSize; j++)
                if (organismOne->genome[j] != organismOne->parentGenomes[i][j])
                    tempDiff++;
            if (tempDiff > difference) difference = tempDiff;
        }
    }

    if (difference >= runSpeciesDifference)
        return true;
    else
        return false;
}


void simulation::newSpecies(Organism &progeny, Organism &parent, playingFieldStructure *pf)
{

    if ((progeny.speciesID != parent.speciesID) || (runGenomeSize != progeny.genome.size()))
        warning("Eesh", "Speciation error. Please contact RJG in the hope he can sort this out.");

    //Iterate species count
    speciesCount++;

    //Update cladogenesis counter for each - use this to work out terminal branch lengths later.
    progeny.cladogenesis = iterations;

    //Record parent ID
    int parentSpecies = parent.speciesID;

    //Add this species to list of children species for all members of this species currently alive in the simulation
    //Do this independently for each playing field (i.e. only in this playing field this time) - this obviously has implications if after mixing the species are very different in different playing fields - many more speciations, lower diversity in any given PF.
    //To do - revisit this decision //
    QList <bool> newSpeciesGenome;
    for (auto b : std::as_const(progeny.genome))newSpeciesGenome.append(b);
    for (auto o : std::as_const(pf->playingField))
        if (o->speciesID == parentSpecies) o->parentGenomes.append(newSpeciesGenome);

    parent.cladogenesis = iterations;

    //Update parent, which will be sister group to new one in tree.
    progeny.parentSpeciesID = parent.speciesID;
    //Reset last child as no children yet

    //Update species ID, born interation
    progeny.speciesID = speciesCount;
    progeny.born = iterations;

    //Genome → parent genome
    for (int j = 0; j < runGenomeSize; j++)progeny.parentGenomes[0][j] = progeny.genome[j];

    //Pass on eccosystem engineer status
    progeny.ecosystemEngineer = parent.ecosystemEngineer;

    //Keep a track of extinction for printing
    extinctList.append(false);
}

int simulation::returninformativeCharacters()
{
    return informativeCharacters;
}

/************** Print functions *************/

QString simulation::printPlayingField(const QVector <playingFieldStructure *> &playingFields)
{
    int p = 0;

    QString pfText;
    QTextStream out(&pfText);

    for (auto pf : playingFields)
    {
        out << "Playing field: " << p << "\n";
        p++;
        int cnt = 0;
        for (auto o : std::as_const(pf->playingField))
        {
            out << "\nPlayingfield pos: " << cnt << " \nSpecies ID: " << o->speciesID << "\nGenome:\t";
            for (auto i : std::as_const(o->genome)) i ? out << 1 : out << 0 ;
            out << "\nParent genomes:\n";
            int count = 0;
            for (auto g : std::as_const(o->parentGenomes))
            {
                out << "Genome " << count++ << "\t";
                for (auto i : std::as_const(g))
                    i ? out << 1 : out << 0 ;
                out << "\n";
            }

            out << "\nFitness:\t" << o->fitness;
            out << "\nEcosystem engineer:\t" << o->ecosystemEngineer;
            out << "\n";
            cnt++;
        }
    }
    return pfText;
}

QString simulation::printPlayingFieldSemiconcise(const QVector <playingFieldStructure *> &playingFields)
{
    int p = 0;

    QString pfText;
    QTextStream out(&pfText);

    for (auto pf : playingFields)
    {
        int cnt = 0;
        out << "\nPlaying field number,Playingfield position,Species ID,Ecosystem engineer,Genome\n";
        for (auto o : std::as_const(pf->playingField))
        {
            out << p << "," << cnt << "," << o->speciesID << "," << o->ecosystemEngineer << ",";
            for (auto i : std::as_const(o->genome)) i ? out << 1 : out << 0 ;
            out << "\n";
            cnt++;
        }
        p++;
    }
    return pfText;
}

QString simulation::printPlayingFieldConcise(const QVector <playingFieldStructure *> &playingFields)
{
    int p = 0;

    QString pfText;
    QTextStream out(&pfText);

    for (auto pf : playingFields)
    {
        int cnt = 0;
        out << "\nPlaying field number,Playingfield position,Species ID,Ecosystem engineer\n";
        for (auto o : std::as_const(pf->playingField))
        {
            out << p << "," << cnt << "," << o->speciesID << "," << o->ecosystemEngineer << "\n";
            cnt++;
        }
        p++;
    }
    return pfText;
}

QString simulation::printPlayingFieldGenomesConcise(const QVector <playingFieldStructure *> &playingFields)
{
    QString pfText;
    QTextStream out(&pfText);

    for (auto pf : playingFields)
        for (auto o : std::as_const(pf->playingField))
            for (auto i : std::as_const(o->genome)) i ? out << 1 : out << 0 ;

    return pfText;
}


QString simulation::printMasks(const QVector <playingFieldStructure *> &playingFields)
{
    int playingfield = 0;

    QString maskText;
    QTextStream out(&maskText);

    for (auto p : playingFields)
    {
        out << "Playingfield " << playingfield << "\n";
        for (int environmentNumber = 0; environmentNumber < p->masks.count(); environmentNumber++)
        {
            out << "Environment " << environmentNumber << "\n";
            for (int maskNumber = 0; maskNumber < runMaskNumber; maskNumber++)
            {
                out << "Mask number " << maskNumber << " :\t";
                for (auto i : std::as_const(p->masks[environmentNumber][maskNumber])) i ? out << 1 : out << 0 ;
                out << "\n";

            }
        }
        playingfield++;
    }
    return maskText;
}

QString simulation::printMasks(const QVector <playingFieldStructure *> &playingFields, int playingfield)
{
    QString maskText;
    QTextStream out(&maskText);

    for (int environmentNumber = 0; environmentNumber < playingFields[playingfield]->masks.count(); environmentNumber++)
    {
        out << "Environment " << environmentNumber << "\n";
        for (int maskNumber = 0; maskNumber < runMaskNumber; maskNumber++)
        {
            out << "Mask number " << maskNumber << " :\t";
            for (auto i : std::as_const(playingFields[playingfield]->masks[environmentNumber][maskNumber])) i ? out << 1 : out << 0 ;
            out << "\n";
        }
    }

    return maskText;
}


QString simulation::printSpeciesList(const QVector <Organism *> &speciesList)
{
    int cnt = 0;

    QString slText;
    QTextStream out(&slText);

    for (auto o : speciesList)
    {
        out << "Entry " << cnt << " is species " << o->speciesID << " ";
        cnt++;
        for (auto i : std::as_const(o->genome)) i ? out << 1 : out << 0 ;
        out << "\tBorn: " << o->born << "\tExtinct: " << o->extinct << "\n";
    }

    return slText;
}

QString simulation::printMatrix(const QVector <Organism *> &speciesList)
{
    QString matrixString;
    QTextStream matrixTextStream(&matrixString);

    int totalSpeciesCount = speciesList.length();
    for (int i = 0; i < totalSpeciesCount; i++)
    {
        matrixTextStream << "Species_" << doPadding(i, totalSpeciesCount) << "\t";
        for (auto j : std::as_const(speciesList[i]->genome)) j ? matrixTextStream << 1 : matrixTextStream << 0 ;
        matrixTextStream << "\n";
    }

    return matrixString;
}

QString simulation::printStochasticMatrix(const QVector <Organism *> &speciesList, bool stochasticLayer)
{
    if (!stochasticLayer)return ("Sorry - stochastic layer is not currently enabled\n");

    QString matrixString;
    QTextStream matrixTextStream(&matrixString);
    int totalSpeciesCount = speciesList.length();
    for (int i = 0; i < totalSpeciesCount; i++)
    {
        matrixTextStream << "Species_" << doPadding(i, paddingAmount(totalSpeciesCount)) << "\t";
        for (auto j : std::as_const(speciesList[i]->stochasticGenome)) j ? matrixTextStream << 1 : matrixTextStream << 0 ;
        matrixTextStream << "\n";
    }
    return matrixString;
}

QString simulation::printTime()
{
    //Create a new simulation object - sending it important settings.
    //Timestamp
    QDateTime date = QDateTime::currentDateTime();
    return date.toString();
}

//Recursive function for writing tree in Newick format, no branch lengths
QString simulation::printNewick(int species, QVector <Organism *> &speciesList)
{
    QString treeString;
    QTextStream treeTextStream(&treeString);

    int offspring = speciesList[species]->children.length();

    int totalSpeciesCount = speciesList.length();

    //Zero padding
    QString speciesID = doPadding(speciesList[species]->speciesID, paddingAmount(totalSpeciesCount));

    if (offspring == 0)
    {
        treeTextStream << speciesID << ",";
        return treeString;
    }

    for (int i = 0; i < offspring; i++)treeTextStream << "(" << printNewick(speciesList[species]->children[i], speciesList);

    treeTextStream << speciesID;

    for  (int i = 0; i < offspring; i++)treeTextStream << ")";

    if (species > 0)treeTextStream << ",";

    return treeString;
}

//Recursive function for writing trees with branch lengths in Newick format
QString simulation::printNewickWithBranchLengths(int species, QVector <Organism *> &speciesList, bool phangornTree)
{
    QString treeString;
    QTextStream treeTextStream(&treeString);

    int offspring = speciesList[species]->children.length();
    int totalSpeciesCount = speciesList.length();

    //Zero padding
    QString speciesID;
    if (phangornTree) speciesID = doPadding(speciesList[species]->speciesID + 1, paddingAmount(totalSpeciesCount));
    else speciesID = doPadding(speciesList[species]->speciesID, paddingAmount(totalSpeciesCount));

    //For terminal cases (reused for branches to nodes below)
    int branchLength = speciesList[species]->extinct - speciesList[species]->cladogenesis;

    //Return recursion and print terminal taxa, and without offspring
    if (offspring == 0)
    {
        //Comma here for terminal duos (e.g. XX,XY) and ladders (e.g. XX,(XY)) - else trees are not correctly parsed
        treeTextStream << speciesID << ":" << branchLength << ",";
        return treeString;
    }

    //Then sort where taxa have offpsring - recurse
    for (int i = 0; i < offspring; i++)treeTextStream << "(" << printNewickWithBranchLengths(speciesList[species]->children[i], speciesList, phangornTree);

    //Write offspring
    treeTextStream << speciesID << ":" << branchLength;

    //Close all brackets and sort out branch lengths
    for  (int i = offspring - 1; i >= 0; i--)
    {
        int child = speciesList[species]->children[i];

        if (i > 0)
        {
            int olderSibling = speciesList[species]->children[i - 1];
            branchLength = speciesList[child]->born - speciesList[olderSibling]->born;
        }
        else branchLength = speciesList[child]->born - speciesList[species]->born;
        treeTextStream << "):" << branchLength;
    }

    //Comma here for internal clade boundaries - e.g. ),(
    if (species > 0)treeTextStream << ",";

    return treeString;
}

QString simulation::printGenomeString(const Organism *org)
{
    QString genome;
    for (auto i : org->genome) i ? genome.append("1") : genome.append("0") ;
    return genome;
}

QString simulation::printGenomeInteger(quint64  genomeLocal, int genomeSizeLocal, const quint64 *lookupsLocal)
{
    QString genome;
    for (int i = 0; i < genomeSizeLocal; i++)
        if (lookupsLocal[i] & genomeLocal)genome.append("1");
        else genome.append("0");

    return genome;
}


QString simulation::printEcosystemEngineers(const QVector <Organism *> &speciesList)
{
    QString ecosystemEngineersList;
    QTextStream ecosystemEngineersTextStream(&ecosystemEngineersList);
    ecosystemEngineersTextStream << "Species,Ecosystem engineer\n";
    int totalSpeciesNumber = speciesList.length();
    for (int i = 0; i < totalSpeciesNumber; i++) ecosystemEngineersTextStream << "Species_" << doPadding(i, paddingAmount(totalSpeciesNumber)) << "," << speciesList[i]->ecosystemEngineer << "\n";

    return ecosystemEngineersList;
}



/************** Utility functions *************/


void simulation::warning(QString header, QString message)
{
    if (theMainWindow != nullptr && !theMainWindow->runFromCommand) QMessageBox::warning(theMainWindow, header, message);
    if (theMainWindow != nullptr && theMainWindow->runFromCommand) qInfo().noquote() << "\n****Warning - message****: " << message;
}

bool simulation::setupSaveDirectory(QString subFolder)
{
    //Sort out path
    savePathDirectory.setPath(simSettings->savePathDirectory);

    if (!savePathDirectory.exists())
    {
        if (theMainWindow != nullptr) warning("Error!", "The program doesn't think the save directory exists, so is going to default back to the direcctory in which the executable is.");
        QString savePathString(QCoreApplication::applicationDirPath());
        savePathString.append(QDir::separator());
        if (theMainWindow != nullptr) theMainWindow->setPath(savePathString);
        savePathDirectory.setPath(savePathString);
    }

    //RJG - Set up save directory
    if (!savePathDirectory.mkpath(QString(PRODUCTNAME) + "_output"))
    {
        warning("Error", "Cant save output files. Permissions issue?");
        return false;
    }
    else savePathDirectory.cd(QString(PRODUCTNAME) + "_output");

    QStringList basenames = {simSettings->logFileNameBase01, simSettings->logFileNameBase02, simSettings->logFileNameBase03};
    for (auto const &basename : basenames)
        if (basename.contains("\\"))
        {
            warning("Error", "Filename includes a backslash. Use forward slashes as separators in path and you should be all good.");
            return false;
        }
        else if (basename.contains("/"))
            if (!savePathDirectory.mkpath(basename.left(basename.lastIndexOf("/"))))
            {
                warning("Error", "Cant create subFolder " + subFolder + ". This could be a permissions issue.");
                return false;
            }

    if (subFolder.length() > 0)
    {
        subFolder.append(doPadding(runs, 3));

        if (!savePathDirectory.mkpath(subFolder))
        {
            warning("Error", "Cant create subFolder. Permissions issue?");
            return false;
        }
        else
        {
            bool flag = false;
            QString directoryPath = savePathDirectory.path();
            directoryPath.append(QDir::separator());
            directoryPath.append(subFolder);
            QDir directory(directoryPath);
            directory.setNameFilters(QStringList() << "*.*");
            directory.setFilter(QDir::Files);
            for (auto &fileToDelete : directory.entryList())
            {
                directory.remove(fileToDelete);
                flag = true;
            }
            if (flag && theMainWindow != nullptr)
            {
                //Add a warning, but I don't want a pop up here - rather just put it in status bar, and then add a slight delay.
                theMainWindow->setStatus("Heads up - there were prexisting files in a subfolder (e.g. Ecosystem engineers outputs) from a previous run. These have been deleted.");
                QTime dieTime = QTime::currentTime().addSecs(2);
                while (QTime::currentTime() < dieTime) QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            }
        }
    }

    return true;
}

bool simulation::writeFile(const QString logFileNameBase, const QString logFileExtension, const QString logFileString, const QHash<QString, QString> &outValues, const QVector<Organism *> &speciesList)
{
    QString fileNameString;
    fileNameString = logFileNameBase;
    fileNameString.append(doPadding(runs, 3));
    fileNameString.append(logFileExtension);
    fileNameString.prepend(savePathDirectory.absolutePath() + QDir::separator());

    QFile file(fileNameString);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream fileTextStream(&file);
    QString fileStringWrite = logFileString;

    fileStringWrite.replace("||Matrix||", printMatrix(speciesList), Qt::CaseInsensitive);
    fileStringWrite.replace("||Stochastic_Matrix||", outValues["Stochastic_Matrix"], Qt::CaseInsensitive);
    fileStringWrite.replace("||Ecosystem_Engineers||", outValues["Ecosystem_Engineers"], Qt::CaseInsensitive);
    fileStringWrite.replace("||TNT_Tree||", outValues["TNTstring"], Qt::CaseInsensitive);
    fileStringWrite.replace("||MrBayes_Tree||", outValues["MrBayes_Tree"], Qt::CaseInsensitive);
    fileStringWrite.replace("||Time||", printTime(), Qt::CaseInsensitive);
    fileStringWrite.replace("||Settings||", simSettings->printSettings(), Qt::CaseInsensitive);
    fileStringWrite.replace("||Unresolvable||", outValues["unresolvableTaxonGroups"], Qt::CaseInsensitive);
    fileStringWrite.replace("||Uninformative||", outValues["uninformativeNumber"], Qt::CaseInsensitive);
    fileStringWrite.replace("||Identical||", outValues["unresolvableCount"], Qt::CaseInsensitive);
    fileStringWrite.replace("||Alive_Record||", outValues["aliveRecordString"], Qt::CaseInsensitive);
    fileStringWrite.replace("||Character_Number||", outValues["Character_Number"], Qt::CaseInsensitive);
    fileStringWrite.replace("||Taxon_Number||",  outValues["Taxon_Number"], Qt::CaseInsensitive);
    fileStringWrite.replace("||Count||", outValues["Count"], Qt::CaseInsensitive);
    fileStringWrite.replace("||Root||", outValues["Root"], Qt::CaseInsensitive);
    fileStringWrite.replace("||PlayingField_Size||", outValues["PlayingField_Size"], Qt::CaseInsensitive);
    fileStringWrite.replace("||PlayingField_Number||", outValues["PlayingField_Number"], Qt::CaseInsensitive);
    fileStringWrite.replace("||PlayingField||", printPlayingField(playingFields), Qt::CaseInsensitive);
    fileStringWrite.replace("||PlayingField_semiconcise||", printPlayingFieldSemiconcise(playingFields), Qt::CaseInsensitive);
    fileStringWrite.replace("||PlayingField_concise||", printPlayingFieldConcise(playingFields), Qt::CaseInsensitive);
    fileStringWrite.replace("||PlayingField_genomes_concise||", printPlayingFieldGenomesConcise(playingFields), Qt::CaseInsensitive);
    fileStringWrite.replace("||Masks||", printMasks(playingFields), Qt::CaseInsensitive);

    fileTextStream << fileStringWrite;

    file.close();
    return true;
}


bool simulation::writeRunningLog(const int iterations, const QString logFileString)
{
    //File  - if needed this can be expanded and run from a loop to include more files...
    QString fileNameString("TrevoSim_running_log_iteration_");
    fileNameString.append(doPadding(iterations, 4));
    fileNameString.append(".txt");

    QString folderName("TREvoSim_running_log_");
    folderName.append(doPadding(runs, 3));
    fileNameString.prepend(savePathDirectory.absolutePath() + QDir::separator() + folderName + QDir::separator());

    QFile file(fileNameString);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream fileTextStream(&file);
    fileTextStream << logFileString;
    file.close();
    return true;
}

bool simulation::writeEEFile(const int iterations, const QString logFileString)
{
    //File  - if needed this can be expanded and run from a loop to include more files...
    QString fileNameString("TrevoSim_EE_iteration_");
    fileNameString.append(doPadding(iterations, 3));
    fileNameString.append(".csv");

    QString folderName("Ecosystem_engineers_");
    folderName.append(doPadding(runs, 3));
    fileNameString.prepend(savePathDirectory.absolutePath() + QDir::separator() + folderName + QDir::separator());

    QFile file(fileNameString);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream fileTextStream(&file);
    fileTextStream << logFileString;

    file.close();
    return true;
}

void simulation::writeGUI(QVector<Organism *> &speciesList)
{
    if (simSettings->runMode != RUN_MODE_TAXON)
    {
        if (theMainWindow->rowMax() < speciesList.count())
            theMainWindow->resizeGrid(speciesList.count() + 100, simSettings->genomeSize);

    }

    for (int i = 0; i < speciesList.count(); i++)
    {
        if (simSettings->runMode != RUN_MODE_TAXON) theMainWindow->showRow(i);
        if (simSettings->genomeOnExtinction && !extinctList[i]) theMainWindow->printBlank(i);
        else theMainWindow->printGenome(speciesList[i], i);
    }

    if (simSettings->runMode != RUN_MODE_TAXON)
        for (int j = speciesList.count(); j < theMainWindow->rowMax(); j++)
            theMainWindow->hideRow(j);

    theMainWindow->setTreeDisplay(printNewick(0, speciesList));

    QString status = QString("Iteration: %1").arg(iterations);
    if (calculateStripUninformativeFactorRunning)status.prepend(QString("Calculating strip uninformative factor. "));
    else if (theMainWindow->batchRunning)status.prepend(QString("Run number: %1; ").arg(runs));
    theMainWindow->setStatus(status);

    qApp->processEvents();
}

int simulation::paddingAmount(int taxonNumber)
{
    if (taxonNumber < 100) return 2;
    else if (taxonNumber < 1000) return 3;
    else return 4;
}

QString simulation::doPadding(int number, int significantFigures)
{
    return QString("%1").arg(number, significantFigures, 10, QChar('0'));
}

//Count peaks returns best fitness level, for matching fitness peaks, if called with repeats -1, then it should also be sent an environment
//If called with repeats (from main window) it prints the histogram file
int simulation::countPeaks(int genomeSize, int repeat, int environment)
{
    QVector <quint64> totals;
    for (int i = 0; i < ((genomeSize * simSettings->maskNumber) + 1); i++) totals.append(0);
    QVector <QVector <quint64> > genomes;

    Organism org(genomeSize, false);

    bool recordGenomes = false;
    if (genomeSize < 21 && repeat > 0) recordGenomes = true;

    //Lookups for printing genomes
    quint64 lookups[64];
    lookups[0] = 1;
    for (int i = 1; i < 64; i++)lookups[i] = lookups[i - 1] * 2;

    quint64 max;
    if (recordGenomes) max = static_cast<quint64>(pow(2., static_cast<double>(genomeSize)));
    else max = 1000000;
    //Progress bar max value is 2^16 - scale to this
    quint16 pmax = static_cast<quint16>(~0);

    QList <quint64> toTest(1000000);
    //This fills with randoms of quint64, but this doesn't matter because below, when setting genome, we only take the first gnomeSize bits, which will also be random!
    if (!recordGenomes) QRandomGenerator::global()->fillRange(toTest.data(), toTest.size());
    else for (int i = 0; i < ((genomeSize * simSettings->maskNumber) + 1); i++) genomes.append(QVector <quint64 >());

    for (quint64 x = 0; x < max; x++)
    {
        //If we do this every 1000, the GUI remains responsive to pause and cancel
        if (x % 1000 == 0 && theMainWindow != nullptr)
        {
            while (theMainWindow->pauseFlag == true && !theMainWindow->escapePressed) qApp->processEvents();
            if (theMainWindow->escapePressed) return -1;
        }

        //Create genome from number
        for (int i = 0; i < genomeSize; i++)
            if (recordGenomes)
            {
                if (lookups[i] & x) org.genome[i] = true;
                else org.genome[i] = false;
            }
            else
            {
                if (lookups[i] & toTest[x]) org.genome[i] = true;
                else org.genome[i] = false;
            }

        //Update GUI every now and then to show not crashed
        if ((x % 9999) == 0 && theMainWindow != nullptr)
        {
            theMainWindow->printGenome(&org, 0);
            qApp->processEvents();
        }
        if ((x % 1000) == 0 && theMainWindow != nullptr && repeat >= 0)
        {
            double prog = (static_cast<double>(x) / static_cast<double>(max)) * pmax;
            theMainWindow->setProgressBar(static_cast<int>(prog));
        }

        //For now, let's just do this for the first playing field - we expect each playingfield to have the same properties in terms of peaks
        //If repeat is not -1 we have called this from the main window, and we want to use all environments
        if (repeat != -1) org.fitness = fitness(&org, playingFields[0]->masks, genomeSize, simSettings->fitnessTarget, runMaskNumber, simSettings->fitnessMode);
        //Otherwise we need to specify which environment we want
        else org.fitness = fitness(&org, playingFields[0]->masks, genomeSize, simSettings->fitnessTarget, runMaskNumber, simSettings->fitnessMode, environment);

        totals[org.fitness]++;
        if (recordGenomes) genomes[org.fitness].append(x);
    }

    //Output - if called with a repeat, this came from the slot in main window
    if (repeat != -1)
    {
        printCountPeaks(genomeSize, totals, genomes, repeat);
        return 0;
    }
    else
    {
        for (int i = 0; i < totals.length(); i++) if (totals[i] > 0) return i;

        //return -1 in case of error
        return -1;
    }

}

void simulation::printCountPeaks(int genomeSize, QVector <quint64> &totals, QVector <QVector <quint64> > &genomes, int repeat)
{
    //RJG - Set up save directory
    if (!setupSaveDirectory()) warning("Error!", "Error opening peaks file to write to - error 1.");

    QString peaksFileNameString = (QString(PRODUCTNAME) + "_fitness_histogram_" + doPadding(repeat, 4));
    peaksFileNameString.prepend(savePathDirectory.absolutePath() + QDir::separator());
    QFile peaksFile(peaksFileNameString);
    if (!peaksFile.open(QIODeviceBase::WriteOnly | QIODevice::Text))warning("Error!", "Error opening peaks file to write to - error 2.");
    QTextStream peaksTextStream(&peaksFile);

    peaksTextStream << QString(PRODUCTNAME) << " peak count for " << simSettings->environmentNumber << " environment(s), " << simSettings->maskNumber << " masks, ";
    peaksTextStream << genomeSize << " bits in genome, and a fitness target of " << simSettings->fitnessTarget << "\n";
    peaksTextStream << "Full " <<  QString(PRODUCTNAME) << " settings: ";
    peaksTextStream << simSettings->printSettings();
    peaksTextStream << "\n";

    //Remove this for now - if important, I can add an access function to simulation
    for (int i = 0; i < simSettings->environmentNumber; i++)
    {
        peaksTextStream << "\nEnvironment number " << i;
        for (int j = 0; j < simSettings->maskNumber; j++)
        {
            peaksTextStream << "\nMask: " << j << ": ";
            for (int k = 0; k < genomeSize; k++)peaksTextStream << playingFields[0]->masks[i][j][k];
        }
    }

    quint64 max = static_cast<quint64>(pow(2., static_cast<double>(genomeSize)));
    //Lookups for printing genomes
    quint64 lookups[64];
    lookups[0] = 1;
    for (int i = 1; i < 64; i++)lookups[i] = lookups[i - 1] * 2;

    peaksTextStream << "\n\nGenomes tested: " << max << ", distribution:\n";
    peaksTextStream << "Fitness, Number of genomes\n";
    for (int i = 0; i < genomeSize * simSettings->maskNumber + 1; i++)peaksTextStream << i << "," << totals[i] << "\n";

    bool sizeFlag = false;
    peaksTextStream << "\n\nGenomes associated with each fitness are as follows:\n";
    for (int i = 0; i < genomes.length(); i++)
        if (!genomes[i].empty())
        {
            if (!sizeFlag)
            {
                peaksTextStream << "Fitness " << i << "\n";
                for (int j = 0; j < genomes[i].length(); j++)
                    peaksTextStream << printGenomeInteger(genomes[i][j], genomeSize, lookups) << "\n";
            }
            if (genomeSize > 20) sizeFlag = true;
        }
    peaksTextStream << "\n";
    peaksFile.close();
}

