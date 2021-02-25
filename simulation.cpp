#include "simulation.h"

simulation::simulation(MainWindow *theMainWindowCon, int runsCon, const simulationVariables *simSettingsCon)
{
    /***** Set up GUI, and workling log *****/
    theMainWindow = theMainWindowCon;
    runs = runsCon;
    simSettings = simSettingsCon;

    //Setup function returns false if failed, else true
    if (!setupSaveDirectory(theMainWindow->getPath(), theMainWindow)) return;

    //To check all works as expected there is a work log that writes everything to a text file
    QString workLogFilename = (QString(PRODUCTNAME) + "_working_log");
    if (theMainWindow->batchRunning) workLogFilename.append(QString("_%1").arg(runs, 3, 10, QChar('0')));
    workLogFilename.append(".txt");
    workLogFilename.prepend(savePathDirectory.absolutePath() + QDir::separator());
    workLogFile.setFileName(workLogFilename);
    if (simSettings->workingLog)
    {
        if (!workLogFile.open(QIODevice::QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(theMainWindow, "Error!", "Error opening working log file to write to.");
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
    //In some settings, genome size will change - but this risks messing with a global. Other variables can change due to rounding from multiplying/dividing by float. Hence keep copy of the actual values and reset at end to make sure nothing changes
    runGenomeSize = simSettings->genomeSize;
    runFitnessTarget = simSettings->fitnessTarget;
    runSpeciesDifference = simSettings->speciesDifference;
    // Initialise variables
    speciesCount = 0, iterations = 0;
    informativeCharacters = 0;
    //Vector for making cumalitve species curve if required
    if (simSettings->speciesCurve)graphing.append(0);

    // If stripping uninformative characters, in order to have the requested char # need to over-generate, and then subsample to requested value
    if (simSettings->stripUninformative  && !theMainWindow->calculateStripUninformativeFactorRunning)
    {

        double runGenomeSizeDouble = static_cast<double>(runGenomeSize);
        runGenomeSizeDouble *= simSettings->stripUninformativeFactor;
        runGenomeSize = static_cast<int>(runGenomeSizeDouble);

        //RJG - some stuff redacted for 2.0.0 - email if you're interested

        double runFitnessTargetDouble = static_cast<double>(runFitnessTarget);
        runFitnessTargetDouble *= simSettings->stripUninformativeFactor;
        runFitnessTarget = static_cast<int>(runFitnessTargetDouble);

        //I suspect it makes sense to do this so it still hits expected behaviour - but note that that means we can't guaruntee species difference if stripping uninformative characters
        double runSpeciesDifferenceDouble = static_cast<double>(runSpeciesDifference);
        runSpeciesDifferenceDouble *= simSettings->stripUninformativeFactor;
        runSpeciesDifference = static_cast<int>(runSpeciesDifferenceDouble);
    }

    /***** Setup and populate playing field *****/
    for (int p = 0; p < simSettings->playingfieldNumber; p++) playingFields.append(new playingFieldStructure);
    for (auto p : playingFields)
        for (int i = 0; i < simSettings->playingfieldSize; i++)
            p->playingField.append(new Organism(runGenomeSize));


    /***** Setup and populate masks *****/
    //3 vectors to allow list of bools, with multiple user defined lists per environment, then multiple environments (some structures have fourth for each playing field)
    //So this is [playing field number /] environment number / mask number / bits
    //This will fill the masks using random numbers as requried
    for (auto p : playingFields)
    {
        for (int k = 0; k < simSettings->environmentNumber; k++)
        {
            p->masks.append(QVector <QVector <bool> >());
            for (int j = 0; j < simSettings->maskNumber; j++)
            {
                p->masks[k].append(QVector <bool>());
                for (int i = 0; i < runGenomeSize; i++)
                {
                    if (QRandomGenerator::global()->generate() > (maxRand / 2)) p->masks[k][j].append(bool(false));
                    else  p->masks[k][j].append(bool(true));
                }
            }
        }
    }

    //If they should start the same, copy of playing fields over
    if ( simSettings->playingfieldNumber > 1 && simSettings->playingfieldMasksMode != 1)
        for (int p = 1; p < simSettings->playingfieldNumber; p++)
            playingFields[p]->masks = playingFields[0]->masks;
}

void simulation::run()
{

    QString aliveRecordString ("Alive record is taken at the end of each iteration, so any species which have gone extinct during that iteration due to mixing or overwriting from a setling new Organism will not feature on the alive list.\nEach line lists: Iteration\tPlaying field\tSpecies alive\n\n");
    QTextStream aliveRecordTextStream(&aliveRecordString);
    Organism bestOrganism = initialise();

    //Populate playing field with clones
    quint32 notFound = static_cast<quint32>(-1);
    if (static_cast<quint32>(bestOrganism.fitness) == notFound)
    {
        QMessageBox::warning(theMainWindow, "Oops", "This has not initialised this correctly. Please try different settings or email RJG.");
        return;
    }
    else for (auto p : playingFields)
            for (auto o : p->playingField) *o = bestOrganism;

    if (simSettings->test == 3) return;

    //Species list to store details of each new species
    QVector <Organism *> speciesList;

    // Above is also the first species, obvs.
    speciesList.append(new Organism(runGenomeSize));
    *speciesList[0] = *playingFields[0]->playingField[0];

    if (simSettings->workingLog)workLogTextStream << "\n\nMasks initiated:\n" <<  printMasks(playingFields) << "\n\n";

    if (simSettings->sansomianSpeciation)theMainWindow->printBlank(speciesCount);
    else theMainWindow->printGenome(speciesList[speciesCount], speciesCount);

    //Create string to record tree in parantheses format
    QString TNTstring;

    // Zero padding
    if (simSettings->taxonNumber > 1000)
    {
        //Not actually an issue because of spin box limits. But still...
        QMessageBox::warning(theMainWindow, "Well", "I guess you were pitching for more than 1000 terminals. Best email RJG.");
        TNTstring = "This isn't going to work, I'm afraid.";
    }

    /************* Start simulation *************/
    do
    {

        //Pause if required....
        while (theMainWindow->pauseFlag == true && !theMainWindow->escapePressed)qApp->processEvents();

        if (theMainWindow->escapePressed)
        {
            clearVectors(playingFields, speciesList);
            return;
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
        for (auto pf : playingFields)
        {
            /******** Sort playing field by fitness *****/
            //std::sort(playingField.begin(),playingField.end(),[](const Organism* OL, const Organism* OR){return OL->fitness<OR->fitness;});
            //qSort(playingField.begin(),playingField.end());
            //This has been removed, because sorting not strictly required - quicker option selected of going doing dice toss, then going down list, finding fittest, etc.
            //If recycled needs to be updted to work with multiple playing fields.

            //Need to keep track of which playing field we're on for calling fitness function - for now
            playingFieldNumber++;
            //If we have enough species from speciation in playing field one, no need to run this all again (and causes a crash thanks to printing to screen)
            if (speciesCount == simSettings->taxonNumber - 1) break;

            int select = coinToss(pf);

            //Need to duplicate one
            Organism progeny = *pf->playingField[select];
            mutateOrganism(progeny, pf);

            //Reminder, in initialise prog genome is set equal to genome
            int diff = differenceToParent(&progeny, runGenomeSize);

            /************* New species *************/
            //Assymetry of tree changes with level of difference below, plus balance between rate of mutation of environment and Organism
            if (diff >= runSpeciesDifference)
            {

                if (simSettings->workingLog) workLogTextStream << "New species has appeared this iteration - species " << progeny.speciesID << " gives birth to " << speciesCount + 1
                                                                   << " at iteration " << iterations << "\n\n";

                //Update iteration counter on the parent species in the species list
                int parentSpecies = pf->playingField[select]->speciesID;
                speciesList[parentSpecies]->cladogenesis = iterations;

                //New species iterates species count and does organism end of new species things
                newSpecies(progeny, *pf->playingField[select], pf);
                //Keeping species list within main object, hence below

                //Set parent to record last child species
                speciesList[parentSpecies]->children.append(speciesCount);
                //Add to species list
                speciesList.append(new Organism(runGenomeSize));
                *speciesList[speciesCount] = progeny;

                //Only print if not Sansomian speciation
                if (simSettings->sansomianSpeciation)theMainWindow->printBlank(speciesCount);
                else theMainWindow->printGenome(speciesList[speciesCount], speciesCount);

                //Update tree string, write to GUI. First sort out zero padding.
                updateTNTstring(TNTstring, progeny.parentSpeciesID, progeny.speciesID);

                //Update display
                theMainWindow->setTreeDisplay(TNTstring);
            }

            //Place progeny in playing field
            int overwrite = calculateOverwrite(pf);

            //Overwrite the thing
            *pf->playingField[overwrite] = progeny;
        }

        //Debugging code for outputting playing field
        if (simSettings->workingLog) workLogTextStream << "Playing field post placement of new organism/at end of iteration, if new organism not added:\n"  << printPlayingField(playingFields) << "\n";

        /************* Update displays etc. *************/

        //Increase iteration count
        iterations++;

        QString status = QString("Iteration: %1").arg(iterations);
        if (theMainWindow->calculateStripUninformativeFactorRunning)status.prepend(QString("Calculating strip uninformative factor. "));
        else if (theMainWindow->batchRunning)status.prepend(QString("Run number: %1; ").arg(runs));
        theMainWindow->setStatus(status);

        //Update gui
        qApp->processEvents();

        /************* Mutate environment then update fitness *************/
        mutateEnvironment();

        //Calculate fitness
        for (int p = 0; p < simSettings->playingfieldNumber; p++)
            for (int i = 0; i < simSettings->playingfieldSize; i++)
                playingFields[p]->playingField[i]->fitness = fitness(playingFields[p]->playingField[i], playingFields[p]->masks, runGenomeSize, runFitnessTarget);



        /************* Check for extinction *************/
        // For each species, Loop through playing field and count and when one instance of a species exists, overwrite genome in species list, and update display

        //Call here
        QHash<QString, QVector <int> > extinct = checkForExtinct(speciesList);

        for (auto s : extinct)
        {
            speciesExtinction(speciesList[s[0]], playingFields[s[1]]->playingField[s[2]], (iterations + 1), simSettings->sansomianSpeciation);
            if (simSettings->workingLog) workLogTextStream << "\nFor write genome at extinction, replacing species list species " << speciesList[s[0]]->speciesID <<
                                                               " that is entry " << s[0] << " with genome of playing field " << s[1] << " " << s[2] << "\n\n";
        }

        /************* Write alive record *************/
        QVector <QVector <int> > speciesAlive;

        for (int n = 0; n < simSettings->playingfieldNumber; n++)
        {
            speciesAlive.append(QVector <int> ());
            for (int i = 0; i < simSettings->playingfieldSize; i++)
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

    }
    while (speciesCount < simSettings->taxonNumber - 1);

    if (simSettings->workingLog) workLogTextStream << "\n\nMasks at end of run:\n" << printMasks(playingFields) << "\n";


    /************* Mop up writing genome for any organisms still alive under Sansomian speciation and for extinction/branch lengths *************/
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
    for (auto pf : playingFields)
        for (auto o : pf->playingField)
        {
            int flag = 0;
            //Check if the entry on playing field has previously been recorded/updated - if so ignore
            for (int i = 0; i < alive.length(); i++)if (alive[i] == o->speciesID)flag = 1;

            if  (flag == 0)
            {
                //Update the extinction and if required, genome.
                int species = o->speciesID;

                alive.append(species);
                if (simSettings->sansomianSpeciation)
                {
                    for (int k = 0; k < runGenomeSize; k++)speciesList[species]->genome[k] = o->genome[k];
                    theMainWindow->printGenome(speciesList[species], species);
                }
                speciesList[species]->extinct = iterations;
            }
        }

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

//Test for informative
    testForUninformative(speciesList, uninformativeCoding);

    int uninformativeNumber = uninformativeCoding.length() + uninformativeNonCoding.length();
    QString statusString =
        QString ("There are %1 uninformative characters in your matrix, and the software is not set to strip these out. This is not necessarily a problem, but I thought you should know.").arg(
            uninformativeNumber);

    if (!simSettings->stripUninformative && uninformativeNumber > 0)theMainWindow->setStatus(statusString);

//Strip the characters if requried
    if (simSettings->stripUninformative)
    {
        bool stripped = stripUninformativeCharacters(speciesList, uninformativeCoding);
        if (!stripped)
        {
            clearVectors(playingFields, speciesList);
            return;
        }
    }

    /******** Check for intrisnically unresolvable clades *****/
    int unresolvableCount = 0;
    QString unresolvableTaxonGroups;
    bool unresolvable = checkForUnresolvableTaxa(speciesList, unresolvableTaxonGroups, unresolvableCount);
    if (!unresolvable) return;

    /************* Write files *************/


    QHash<QString, QString> outValues;
    outValues["TNTstring"] = TNTstring;
    outValues["aliveRecordString"] = aliveRecordString;
    outValues["unresolvableTaxonGroups"] = unresolvableTaxonGroups;
    outValues["MrBayes_Tree"] = printNewickWithBranchLengths(0, speciesList, false, simSettings->taxonNumber);
    QString ni;
    outValues["Uninformative"] = ni.number(uninformativeNumber);
    QString identical;
    outValues["Identical"] = identical.number(unresolvableCount);
    QString cnumber;
    outValues["Character_Number"] = cnumber.number(runGenomeSize);
    QString tnumber;
    outValues["Taxon_Number"] = tnumber.number(simSettings->taxonNumber);
    QString sruns;
    outValues["Count"] = sruns.number(runs);


    if (!writeFile(simSettings->logFileNameBase01, simSettings->logFileExtension01, simSettings->logFileString01, outValues, speciesList))
    {
        QMessageBox::warning(theMainWindow, "Error!", "Error opening output file to write to.");
        clearVectors(playingFields, speciesList);
        return;
    }

    if (!writeFile(simSettings->logFileNameBase02, simSettings->logFileExtension02, simSettings->logFileString02, outValues, speciesList))
    {
        QMessageBox::warning(theMainWindow, "Error!", "Error opening output file to write to.");
        clearVectors(playingFields, speciesList);
        return;
    }

    QString fileNameString03;
    fileNameString03 = simSettings->logFileNameBase03;
    fileNameString03.append(QString("%1").arg(runs, 3, 10, QChar('0')));
    fileNameString03.append(simSettings->logFileExtension03);
    fileNameString03.prepend(savePathDirectory.absolutePath() + QDir::separator());

    if (simSettings->writeTree)
    {
        //File 03 is tree file in .nex format - withour zero padding
        QFile file03(fileNameString03);
        if (!file03.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(theMainWindow, "Error!", "Error opening treefile to write to.");
            clearVectors(playingFields, speciesList);
            return;
        }

        QTextStream file03TextStream(&file03);

        QString logFileString03Tmp(simSettings->logFileString03);

        logFileString03Tmp.replace("||Time||", printTime());
        logFileString03Tmp.replace("||Settings||", simSettings->printSettings());

        file03TextStream << logFileString03Tmp;

        for (int i = 0; i < simSettings->taxonNumber; i++)
        {
            file03TextStream << (QString("%1").arg(i + 1));
            //if (simSettings->taxonNumber < 100)file03TextStream << (QString("%1").arg(i + 1));
            //else file03TextStream << (QString("S_%1").arg(i + 1, 3, 10, QChar('0')));

            file03TextStream << "\t\t" << "Species_" << i << ",\n";
        }

        file03TextStream << "\t\t;\n\ntree tree1 = [&U]";
        QString newickString(printNewickWithBranchLengths(0, speciesList, true, simSettings->taxonNumber));
        // Remove text for phangorn
        newickString.remove("S_00");
        newickString.remove("S_0");
        newickString.remove("S_");

        file03TextStream << newickString << ";\n\nEND;";
        file03.close();
    }

//Write species curve if required
    if (simSettings->speciesCurve)
    {

        QString speciesCurveFilenameString = (QString(PRODUCTNAME) + "_species_curve.txt");

        if (!simSettings->append || !theMainWindow->batchRunning )
        {
            speciesCurveFilenameString.append(QString("%1").arg(runs, 3, 10, QChar('0')));
            speciesCurveFilenameString.append(".txt");
        }
        else
        {
            speciesCurveFilenameString.append(QString("batch_env_%1_masks_%2").arg(simSettings->environmentNumber).arg(simSettings->maskNumber));
            speciesCurveFilenameString.append(".txt");
        }

        speciesCurveFilenameString.prepend(savePathDirectory.absolutePath() + QDir::separator());

        QFile speciesCurveFile(speciesCurveFilenameString);

        if (!simSettings->append)
        {
            if (!speciesCurveFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox::warning(theMainWindow, "Error!", "Error opening curve file to write to.");
                clearVectors(playingFields, speciesList);
                return;
            }
        }
        else
        {
            if (!speciesCurveFile.open(QIODevice::Append | QIODevice::Text))
            {
                QMessageBox::warning(theMainWindow, "Error!", "Error opening curve file to write to.");
                clearVectors(playingFields, speciesList);
                return;
            }
        }

        QTextStream speciesCurveTextStream(&speciesCurveFile);

        //Writer headers
        if (!simSettings->append || !theMainWindow->batchRunning)
        {
            speciesCurveTextStream << (QString(PRODUCTNAME) + "_") << simSettings->printSettings() << "\n";
            speciesCurveTextStream << "First row gives species ID, subsequent rows are iterations at which that species originates in each run:\n";
            for (int i = 0; i < simSettings->taxonNumber; i++)speciesCurveTextStream << i << "\t";
            speciesCurveTextStream << "\n";
        }
        for (int i = 0; i < simSettings->taxonNumber; i++)
            speciesCurveTextStream << graphing[i] << "\t";

        speciesCurveTextStream << "\n";

        speciesCurveFile.close();

    }

    if (simSettings->workingLog)
    {
        workLogTextStream << "On exit, " << (QString(PRODUCTNAME)) << " thinks tree is " <<
                          printNewickWithBranchLengths(0, speciesList, false, simSettings->taxonNumber) << "\n or in TNT format: " << TNTstring;
        workLogFile.close();
    }

//If we've made it this far, all good - can set these back to false
    theMainWindow->batchError = false;
    theMainWindow->unresolvableBatch = false;

//Sort memory
    if (simSettings->test == 0)
    {
        clearVectors(playingFields, speciesList);
        return;
    }
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

    if (simSettings->playingfieldNumber < 2 || simSettings->playingfieldMasksMode != 1)
    {
        if (!simSettings->randomSeed)
        {
            do
            {
                //First organism - initialise and fill playing field with it
                playingFields[0]->playingField[0]->initialise(runGenomeSize);
                playingFields[0]->playingField[0]->fitness = fitness(playingFields[0]->playingField[0], playingFields[0]->masks, runGenomeSize, runFitnessTarget);
                if (static_cast<quint32>(playingFields[0]->playingField[0]->fitness) < minimumFitness)
                {
                    bestOrganism = *playingFields[0]->playingField[0];
                    minimumFitness = static_cast<quint32>(bestOrganism.fitness);
                }
                count++;
            }
            while (count < 200);
        }
        else
        {
            playingFields[0]->playingField[0]->initialise(runGenomeSize);
            bestOrganism = *playingFields[0]->playingField[0];
            minimumFitness = static_cast<quint32>(playingFields[0]->playingField[0]->fitness);
        }
    }
    //Need to initialise sensibly if there are different masks for different playing fields
    else
    {
        theMainWindow->setStatus("Initialialising simulation - given the non-identical masks, this will take a few seconds.");
        qApp->processEvents();
        theMainWindow->addProgressBar(0, 5000);

        do
        {
            //First organism - initialise and fill playing field with it
            playingFields[0]->playingField[0]->initialise(runGenomeSize);
            playingFields[0]->playingField[0]->fitness = fitness(playingFields[0]->playingField[0], playingFields[0]->masks, runGenomeSize, runFitnessTarget);

            //Given that playing field masks are different, now we need to initialise with the best organism we can for all.
            //Currently implemented using the best mean fitness of all organisms tried
            QVector <int> fitnesses;

            for (auto p : playingFields)
            {
                fitnesses.append(fitness(p->playingField[0], p->masks, runGenomeSize, runFitnessTarget));
            }

            int meanFitness = 0;

            for (auto i : fitnesses) meanFitness += i;

            meanFitness = meanFitness / fitnesses.count();

            if (static_cast<quint32>(meanFitness) < minimumFitness)
            {
                bestOrganism = *playingFields[0]->playingField[0];
                minimumFitness = static_cast<quint32>(meanFitness);
            }
            count++;
            if (count % 100 == 0)theMainWindow->setProgressBar(count);
        }
        while (count < 5000);

        theMainWindow->hideProgressBar();
    }

    return bestOrganism;
}

int simulation::coinToss(const playingFieldStructure *pf)
{
    //Which organism to select
    int select = -1;

    //Move down the list and select one - make it likely it is near the top, so has good fitness (i.e. near level zero)
    //Currently 50% chance it'll choose the first, and so on, a la coin toss
    int marker = 0;

    //Note that simSettings->selectionCoinToss is a double - hence all the casting below
    while (static_cast<double>(QRandomGenerator::global()->generate()) > (static_cast<double>(maxRand) / simSettings->selectionCoinToss)) if (++ marker >= simSettings->playingfieldSize)marker = 0;

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

    return select;
}

void simulation::mutateOrganism(Organism &progeny, const playingFieldStructure *pf)
{
    //Mutate
    int temporaryFitness = progeny.fitness;

    //Work out number - need 4x if stochastic layer present
    double mutations = 0.0;
    mutations = (static_cast<double>(runGenomeSize ) / 100.) * simSettings->organismMutationRate;

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
        mutationPositionInteger = QRandomGenerator::global()->bounded(runGenomeSize);

        SNPs.append(QRandomGenerator::global()->bounded(runGenomeSize));

        progeny.genome[mutationPositionInteger] = !progeny.genome[mutationPositionInteger];
    }

    //Update fitness
    progeny.fitness = fitness(&progeny, pf->masks, runGenomeSize, runFitnessTarget);

    if (simSettings->discardDeleterious && (temporaryFitness < progeny.fitness))
        for (int i = 0; i < SNPs.count(); i++)progeny.genome[SNPs[i]] = !progeny.genome[SNPs[i]];

}

void simulation::newSpecies(Organism &progeny, Organism &parent, playingFieldStructure *pf)
{

    if (progeny.speciesID != parent.speciesID) QMessageBox::warning(theMainWindow, "Eesh", "Speciation error. Please contact RJG in the hope he can sort this out.");

    //Iterate species count
    speciesCount++;

    //Update cladogenesis counter for each - use this to work out terminal branch lengths later.
    progeny.cladogenesis = iterations;

    //Record parent ID
    int parentSpecies = parent.speciesID;

    //Reset timer on all of this species to avoid clustering of speciation events as multiple individuals hit n mutations
    //Do this independently for each playing field (i.e. only in this playing field this time) - this obviously has implications if after mixing the species are very different in different playing fields - many more speciations, lower diversity in any given PF.
    for (auto o : pf->playingField)
        if (o->speciesID == parentSpecies)
            for (int j = 0; j < runGenomeSize; j++)o->parentGenome[j] = progeny.genome[j];

    parent.cladogenesis = iterations;

    //Update parent, which will be sister group to new one in tree.
    progeny.parentSpeciesID = parent.speciesID;
    //Reset last child as no children yet

    //Species curve - record speciation
    if (simSettings->speciesCurve)graphing.append(iterations);

    //Update species ID, born interation
    progeny.speciesID = speciesCount;
    progeny.born = iterations;

    //Genome → progenator genome
    for (int j = 0; j < runGenomeSize; j++)progeny.parentGenome[j] = progeny.genome[j];

}

void simulation::updateTNTstring(QString &TNTstring, int progParentSpeciesID, int progSpeciesID)
{
    QString progenySpeciesID;
    if (simSettings->taxonNumber < 100) progenySpeciesID = QString("%1").arg(progParentSpeciesID, 2, 10, QChar('0'));
    else progenySpeciesID = QString("%1").arg(progParentSpeciesID, 3, 10, QChar('0'));

    QString speciesID;
    if (simSettings->taxonNumber < 100) speciesID = QString("%1").arg(progSpeciesID, 2, 10, QChar('0'));
    else speciesID = QString("%1").arg(progSpeciesID, 3, 10, QChar('0'));

    //Then write string
    //If first iteration, write manually so as to avoid starting with excess brackets.
    if (speciesCount == 1 && simSettings->taxonNumber < 100)TNTstring = "(00 01)";
    else if (speciesCount == 1 && simSettings->taxonNumber >= 100)TNTstring = "(000 001)";
    else
    {
        //Then split string at the ancestral species - first search for the progenator
        int position = TNTstring.indexOf(progenySpeciesID);

        //You should never find a bracket or not find anything
        if (position < 1) QMessageBox::warning(theMainWindow, "Eesh", "You shouldn't see this. There's been an error writing the tree string. Please contact RJG in the hope he can sort this out.");

        // Split tree file at this point, into left, and right of position.
        QString treeLeft = TNTstring.left(position);
        QString treeRight = TNTstring.right(TNTstring.length() - (position + progenySpeciesID.length()));

        //Strip spaces if required - is not for TNT...
        //if(treeLeft.at(treeLeft.length()-1)==' ')treeLeft.remove(treeLeft.length()-1,1);
        //if(treeRight.at(0)==' ')treeRight.remove(0,1);

        //Build new string
        QString ins = QString("(%1 %2)").arg(progenySpeciesID).arg(speciesID);
        TNTstring = treeLeft + ins + treeRight;
    }
}

int simulation::calculateOverwrite(const playingFieldStructure *pf)
{
    int overwrite = -1;
    if (simSettings->randomOverwrite)
    {
        overwrite = QRandomGenerator::global()->bounded(simSettings->playingfieldSize);
        if (overwrite == simSettings->playingfieldSize) overwrite = simSettings->playingfieldSize - 1;
    }
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

void simulation::mutateEnvironment()
{
    //Calculate mutation # as previously, and using same variables for ease - this is the number of mutations total for each mask
    double numberEnvironmentMutationsDouble = (static_cast<double>(runGenomeSize) / 100.) * simSettings->environmentMutationRate;
    double numberEnvironmentMutationsIntegral = static_cast<int>(numberEnvironmentMutationsDouble);

    //Sort out the probabilities of extra mutation given remainder
    double numberEnvironmentMutationsFractional = modf(numberEnvironmentMutationsDouble, &numberEnvironmentMutationsIntegral);
    int numberEnvironmentMutationsInteger = (static_cast<int>(numberEnvironmentMutationsIntegral));
    //Due to rounding the above always results in fewer mutations than would be expected - the -150 below equates to an average of 1 mutation per 100 base pairs
    if (static_cast<double>(QRandomGenerator::global()->generate()) < (numberEnvironmentMutationsFractional * static_cast<double>(maxRand - 150))) numberEnvironmentMutationsInteger++;

    //Mutate irrespective of playing field mode settings if there are multiple ones
    for (auto pf : playingFields)
        for (int k = 0; k < numberEnvironmentMutationsInteger; k++)
            for (int j = 0; j < simSettings->environmentNumber; j++)
                for (int i = 0; i < simSettings->maskNumber; i++)
                {
                    //Scale random number to genome size
                    int mutationPosition = QRandomGenerator::global()->bounded(runGenomeSize);
                    pf->masks[j][i][mutationPosition] = !pf->masks[j][i][mutationPosition];
                }

    //Copy between PFs if they are set to be identical
    if ( simSettings->playingfieldNumber > 1 && simSettings->playingfieldMasksMode == 0)
        for (int p = 1; p < simSettings->playingfieldNumber; p++)
            playingFields[p]->masks = playingFields[0]->masks;
}

void simulation::testForUninformative(const QVector <Organism *> &speciesList, QList <int> &uninformativeCoding)
{
    for (int i = 0; i < runGenomeSize; i++)
    {
        int count = 0;
        for (int j = 0; j < simSettings->taxonNumber; j++) if (speciesList[j]->genome[i])count++;
        if (count < 2 || count > (simSettings->taxonNumber - 2)) uninformativeCoding.append(i);
    }
}

bool simulation::stripUninformativeCharacters(QVector <Organism *> &speciesList, const QList <int> &uninformativeCoding)
{
    if (simSettings->workingLog) workLogTextStream << "Stripping uninformativeCoding characters. Prior to removal:\n" << printSpeciesList(speciesList) << "\n";

    //Keep a marker so swtich between coding and non coding is recorded when characters removed (if required).
    int codingGenomeEnd = runGenomeSize;

    //Delete uninformativeCoding characters
    for (int j = 0; j < simSettings->taxonNumber; j++)
    {
        for (int i = uninformativeCoding.size() - 1; i >= 0; i--)
        {
            speciesList[j]->genome.remove(uninformativeCoding[i]);
            // Start at end and work back to avoid numbering issues post - deletion.
            if (j == 0)codingGenomeEnd--;
        }
    }

    //Sort out variables
    if (simSettings->stripUninformative)
    {
        runGenomeSize = simSettings->genomeSize;
        //RJG - some stuff redacted for 2.0.0 - email if you're interested
        runSpeciesDifference = simSettings->speciesDifference;
    }

    if (theMainWindow->calculateStripUninformativeFactorRunning)
    {
        //Record how many characters here, no need to do anything more
        informativeCharacters = speciesList[0]->genome.size();
        return false;
    }

    bool requiredCharacterNumber = true;

    if (simSettings->stripUninformative)
    {
        if (speciesList[0]->genome.size() < runGenomeSize) requiredCharacterNumber = false;
        if (codingGenomeEnd < runGenomeSize || speciesList[0]->genome.size() < runGenomeSize) requiredCharacterNumber = false;
    }

    if (!requiredCharacterNumber && !simSettings->test)
    {
        if (theMainWindow->batchError == false)
        {
            if (theMainWindow->batchRunning)theMainWindow->batchError = true;
            QMessageBox *warningBox = new QMessageBox;
            //Delete when closed so no memory leak
            warningBox->setAttribute(Qt::WA_DeleteOnClose, true);
            warningBox->setWindowTitle("Oops");
            warningBox->setText("It seems there are not enough informative characters to pull this off. Best either try different settings, or email RJG and he can sort. If you're running a batch, the program will keep on trying (and this is the sole error message you'll see, which will time out after ten minutes) - but if it's not got anywhere after half an hour or so, maybe quit, restart and change settings (e.g. species difference) to try and have more luck.");
            warningBox->show();
            //Close after three minutes.
            QTimer::singleShot(180000, warningBox, SLOT(close()));
        }

        if (simSettings->workingLog) workLogTextStream << "Return at !requiredCharacterNumber\n";
        return false;
    }

    if (simSettings->workingLog) workLogTextStream << "After removal:\n" << printSpeciesList(speciesList) << "\n";

    return true;
}

bool simulation::checkForUnresolvableTaxa(const QVector<Organism *> &speciesList, QString &unresolvableTaxonGroups, int &unresolvableCount)
{
    bool unresolvable = false;
    unresolvableCount = 0;
    QVector <QVector <int> > unresolvableSpecies;

    if (simSettings->workingLog) workLogTextStream << "Checking for unresolvable clades";

    QString message("A heads up. There are intrinscially unresolvable taxa in this matrix (i.e. at least two taxa have identical genomes); this could affect your results. For more information, check out:\n\nBapst, D.W., 2013. When can clades be potentially resolved with morphology?. PLoS One, 8(4), p.e62312.\n\nThe taxa in question are:");

    //Compare each against each
    for (int i = 0; i < simSettings->taxonNumber - 1; i++)
        for (int j = i + 1; j < simSettings->taxonNumber; j++)
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

    if (unresolvable && theMainWindow->unresolvableBatch == false)
    {
        if (theMainWindow->batchRunning)theMainWindow->unresolvableBatch = true;

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
                QString("In this case, you are above the cutoff of resolvable taxa - there are %1 identical terminals. If you are in batch mode, this will carry on trying until you have hit the requested number of runs. If you're not, you may want to try again (or hit run for, and then enter one run to automatically run it till one sticks).").arg(
                    unresolvableCount));
        }

        if (!simSettings->test)
        {
            QMessageBox *warningBox2 = new QMessageBox;
            //Delete when closed so no memory leak
            warningBox2->setAttribute(Qt::WA_DeleteOnClose, true);
            warningBox2->setWindowTitle("Warning");
            warningBox2->setText(message);
            warningBox2->show();
            //Close after three minutes.
            QTimer::singleShot(180000, warningBox2, SLOT(close()));
        }

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
            for (int j = 0; j < simSettings->playingfieldSize; j++)
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

        //Print if has gone extinct
        if (count == 0)theMainWindow->printGenome(speciesList[i], i);
    }
    return extinct;
}

void simulation::speciesExtinction(Organism *org, const Organism *playingField, int extinctIteration, bool samsonian)
{
    org->extinct = extinctIteration;
    if (samsonian)
        for (int j = 0; j < org->genome.size(); j++)org->genome[j] = playingField->genome[j];

}

//Masks passed as a const reference.
int simulation::fitness(const Organism *org, const QVector<QVector<QVector<bool> > > &masks, int runGenomeSize, int runFitnessTarget)
{

    int maskNumber = masks[0].length();
    int environmentNumber = masks.length();

    int minimumFitness = (runGenomeSize * maskNumber);

    for (int h = 0; h < environmentNumber; h++)
    {
        int temporaryFitness = -1, counts = 0;
        for (int i = 0; i < maskNumber; i++)
            for (int j = 0; j < runGenomeSize; j++)
                if (org->genome[j] != masks[h][i][j])counts++;

        //Define fitness as the distance away from having ~50% of genome different from all masks in environment. 0 highest, fitness size/2 lowest.
        temporaryFitness = qAbs(counts - runFitnessTarget);
        //temporaryFitness = counts;
        //temporaryFitness = qAbs(counts-(simSettings->maskNumber*(runGenomeSize/2)));
        if (temporaryFitness < minimumFitness)minimumFitness = temporaryFitness;
    }

    return minimumFitness;
}

int simulation::genomeDifference(const Organism *organismOne, const Organism *organismTwo)
{
    int diff = 0;
    for (int j = 0; j < organismOne->genome.length(); j++)
        if (organismOne->genome[j] != organismTwo->genome[j])diff++;
    return diff;
}

int simulation::differenceToParent(const Organism *organismOne, int runGenomeSize)
{
    int diff = 0;
    // Loop to select size to allow decoupling of species definition from genome size.
    for (int j = 0; j < runGenomeSize; j++)
        if (organismOne->genome[j] != organismOne->parentGenome[j])diff++;
    return diff;
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
        for (auto o : pf->playingField)
        {
            out << "\nPlayingfield pos: " << cnt << " \nSpecies ID: " << o->speciesID << "\nGenome:\t";
            for (auto i : o->genome) i ? out << 1 : out << 0 ;
            out << "\nParent genome:\t";
            for (auto i : o->parentGenome) i ? out << 1 : out << 0 ;

            out << "\nFitness:\t" << o->fitness;
            out << "\n";
            cnt++;
        }
    }
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
        int environmentNumber = 0;
        for (auto e : p->masks)
        {
            int maskNumber = 0;
            out << "Environment " << environmentNumber << "\n";
            for (auto m : p->masks[environmentNumber])
            {
                out << "Mask number " << maskNumber << " :\t";
                for (auto i : p->masks[environmentNumber][maskNumber]) i ? out << 1 : out << 0 ;
                out << "\n";
                maskNumber++;
            }
            environmentNumber ++;
        }
        playingfield++;
    }
    return maskText;
}

QString simulation::printMasks(const QVector <playingFieldStructure *> &playingFields, int playingfield)
{
    QString maskText;
    QTextStream out(&maskText);

    int environmentNumber = 0;
    for (auto e : playingFields[playingfield]->masks)
    {
        int maskNumber = 0;
        out << "Environment " << environmentNumber << "\n";
        for (auto m : playingFields[playingfield]->masks[environmentNumber])
        {
            out << "Mask number " << maskNumber << " :\t";
            for (auto i : playingFields[playingfield]->masks[environmentNumber][maskNumber]) i ? out << 1 : out << 0 ;
            out << "\n";
            maskNumber++;
        }
        environmentNumber ++;
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
        for (auto i : o->genome) i ? out << 1 : out << 0 ;
        out << "\tBorn: " << o->born << "\tExtinct: " << o->extinct << "\n";
    }

    return slText;
}

QString simulation::printMatrix(const QVector <Organism *> &speciesList)
{
    QString matrixString;
    QTextStream matrixTextStream(&matrixString);

    for (int i = 0; i < speciesList.length(); i++)
    {
        matrixTextStream << "Species_" << i << "\t";
        for (auto j : speciesList[i]->genome) j ? matrixTextStream << 1 : matrixTextStream << 0 ;
        matrixTextStream << "\n";
    }

    return matrixString;
}

QString simulation::printStochasticMatrix(const QVector <Organism *> &speciesList, bool stochasticLayer)
{
    if (!stochasticLayer)return ("Sorry - stochastic layer is not currently enabled\n");

    QString matrixString;
    QTextStream matrixTextStream(&matrixString);

    for (int i = 0; i < speciesList.length(); i++)
    {
        matrixTextStream << "Species_" << i << "\t";
        for (auto j : speciesList[i]->stochasticGenome) j ? matrixTextStream << 1 : matrixTextStream << 0 ;
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
QString simulation::printNewick(int species, QVector <Organism *> &speciesList, int totalSpeciesCount)
{
    QString treeString;
    QTextStream treeTextStream(&treeString);

    int offspring = speciesList[species]->children.length();

    //Zero padding
    QString speciesID;
    if (totalSpeciesCount < 100) speciesID = QString("%1").arg(speciesList[species]->speciesID, 2, 10, QChar('0'));
    else speciesID = QString("%1").arg(speciesList[species]->speciesID, 3, 10, QChar('0'));

    if (offspring == 0)
    {
        treeTextStream << speciesID << ",";
        return treeString;
    }

    for (int i = 0; i < offspring; i++)treeTextStream << "(" << printNewick(speciesList[species]->children[i], speciesList, totalSpeciesCount);

    treeTextStream << speciesID;

    for  (int i = 0; i < offspring; i++)treeTextStream << ")";

    if (species > 0)treeTextStream << ",";

    return treeString;
}

//Recursive function for writing trees with branch lengths in Newick format
QString simulation::printNewickWithBranchLengths(int species, QVector <Organism *> &speciesList, bool phangornTree, int totalSpeciesCount)
{
    QString treeString;
    QTextStream treeTextStream(&treeString);

    int offspring = speciesList[species]->children.length();

    //Zero padding
    QString speciesID;
    if (phangornTree)
    {
        if (totalSpeciesCount < 100) speciesID = QString("S_%1").arg(speciesList[species]->speciesID + 1, 2, 10, QChar('0'));
        else speciesID = QString("S_%1").arg(speciesList[species]->speciesID + 1, 3, 10, QChar('0'));
    }
    else
    {
        if (totalSpeciesCount < 100) speciesID = QString("S_%1").arg(speciesList[species]->speciesID, 2, 10, QChar('0'));
        else speciesID = QString("S_%1").arg(speciesList[species]->speciesID, 3, 10, QChar('0'));
    }
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
    for (int i = 0; i < offspring; i++)treeTextStream << "(" << printNewickWithBranchLengths(speciesList[species]->children[i], speciesList, phangornTree, totalSpeciesCount);

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

/************** Utility functions *************/

bool simulation::setupSaveDirectory(QString savePath, MainWindow *theMainWindow)
{
//Sort out path
    savePathDirectory = savePath;

    if (!savePathDirectory.exists())
    {
        if (QMessageBox::warning(theMainWindow, "Error!", "The program doesn't think the save directory exists, so is going to default back to the direcctory in which the executable is.",
                                 QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel) return false;
        QString savePathString(QCoreApplication::applicationDirPath());
        savePathString.append(QDir::separator());
        theMainWindow->setPath(savePathString);
    }

    //RJG - Set up save directory
    if (!savePathDirectory.mkpath(QString(PRODUCTNAME) + "_output"))
    {
        QMessageBox::warning(theMainWindow, "Error", "Cant save output files. Permissions issue?");
        return false;
    }
    else savePathDirectory.cd(QString(PRODUCTNAME) + "_output");
    return true;
}

bool simulation::writeFile(const QString logFileNameBase, const QString logFileExtension, const QString logFileString, const QHash<QString, QString> &outValues, const QVector<Organism *> &speciesList)
{
    //File  - if needed this can be expanded and run from a loop to include more files...
    QString fileNameString;
    if (!simSettings->append || !theMainWindow->batchRunning )
    {
        fileNameString = logFileNameBase;
        fileNameString.append(QString("%1").arg(runs, 3, 10, QChar('0')));
        fileNameString.append(logFileExtension);
    }
    else
    {
        fileNameString = logFileNameBase;
        fileNameString.append("batch");
        fileNameString.append(logFileExtension);
    }

    fileNameString.prepend(savePathDirectory.absolutePath() + QDir::separator());

    QFile file(fileNameString);
    if (!simSettings->append)
    {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    }
    else if (!file.open(QIODevice::Append | QIODevice::Text)) return false;

    QTextStream fileTextStream(&file);
    QString fileStringWrite = logFileString;

    fileStringWrite.replace("||Matrix||", printMatrix(speciesList));
    fileStringWrite.replace("||Stochastic_Matrix||", outValues["Stochastic_Matrix"]);
    fileStringWrite.replace("||TNT_Tree||", outValues["TNTstring"]);
    fileStringWrite.replace("||MrBayes_Tree||", outValues["MrBayes_Tree"]);
    fileStringWrite.replace("||Time||", printTime());
    fileStringWrite.replace("||Settings||", simSettings->printSettings());
    fileStringWrite.replace("||Unresolvable||", outValues["unresolvableTaxonGroups"]);
    fileStringWrite.replace("||Uninformative||", outValues["uninformativeNumber"]);
    fileStringWrite.replace("||Identical||", outValues["unresolvableCount"]);
    fileStringWrite.replace("||Alive_Record||", outValues["aliveRecordString"]);
    fileStringWrite.replace("||Character_Number||", outValues["Character_Number"]);
    fileStringWrite.replace("||Taxon_Number||",  outValues["Taxon_Number"]);
    fileStringWrite.replace("||Count||", outValues["Count"]);

    fileTextStream << fileStringWrite;

    file.close();
    return true;
}

void simulation::countPeaks(MainWindow *theMainWindow)
{
    bool ok;
    int genomeSize = QInputDialog::getInt(theMainWindow, "Fitness histogram...", "How many bits?", 32, 1, 64, 1, &ok);
    if (!ok) return;

    theMainWindow->simSettings->genomeSize = genomeSize;

    int maskNumber =  theMainWindow->simSettings->maskNumber;
    int fitnessTarget = theMainWindow->simSettings->fitnessTarget;
    int environmentNumber = theMainWindow->simSettings->environmentNumber;

    // Load masks
    QVector <QVector <QVector <bool> > > masks;
    for (int k = 0; k < environmentNumber; k++)
    {
        masks.append(QVector <QVector <bool> >());
        for (int j = 0; j < maskNumber; j++)
        {
            masks[k].append(QVector <bool>());
            for (int i = 0; i < genomeSize; i++)
            {
                if (QRandomGenerator::global()->generate() > (QRandomGenerator::max() / 2)) masks[k][j].append(bool(false));
                else  masks[k][j].append(bool(true));
            }
        }
    }

    theMainWindow->resizeGrid();

    Organism org(genomeSize);
    QVector <quint64> totals;
    for (int i = 0; i < genomeSize * maskNumber; i++)totals.append(0);

    //Lookups for printing genomes
    quint64 lookups[64];
    lookups[0] = 1;
    for (int i = 1; i < 64; i++)lookups[i] = lookups[i - 1] * 2;

    quint64 max = static_cast<quint64>(pow(2., static_cast<double>(genomeSize)));

    //Progress bar max value is 2^16 - scale to this
    quint16 pmax = static_cast<quint16>(-1);

    theMainWindow->addProgressBar(0, pmax);

    QVector <QVector <quint64> > genomes;
    bool recordGenomes = false;
    if (genomeSize < 21) recordGenomes = true;

    if (recordGenomes)for (int i = 0; i < genomeSize * maskNumber; i++)genomes.append(QVector <quint64 >());

    for (quint64 x = 0; x < max; x++)
    {

        //Create genome from number
        for (int i = 0; i < genomeSize; i++)
            if (lookups[i] & x)org.genome[i] = true;
            else org.genome[i] = false;

        //Update GUI every now and then to show not crashed
        if ((x % 9999) == 0)theMainWindow->printGenome(&org, 0);
        if ((x % 1000) == 0)
        {
            double prog = (static_cast<double>(x) / static_cast<double>(max)) * pmax;
            theMainWindow->setProgressBar(static_cast<int>(prog));
        }

        org.fitness = fitness(&org, masks, genomeSize, fitnessTarget);

        totals[org.fitness]++;
        if (recordGenomes)genomes[org.fitness].append(x);
    }

    //RJG - Set up save directory
    if (!setupSaveDirectory(theMainWindow->getPath(), theMainWindow)) return;

    QString peaksFileNameString = (QString(PRODUCTNAME) + "_fitness_histogram.txt");
    peaksFileNameString.prepend(savePathDirectory.absolutePath() + QDir::separator());
    QFile peaksFile(peaksFileNameString);
    if (!peaksFile.open(QIODevice::Append | QIODevice::Text))QMessageBox::warning(theMainWindow, "Error!", "Error opening curve file to write to.");
    QTextStream peaksTextStream(&peaksFile);

    peaksTextStream << QString(PRODUCTNAME) << " peak count for " << environmentNumber << " environment(s), " << maskNumber << " masks, ";
    peaksTextStream << genomeSize << " bits in genome, and a fitness target of " << fitnessTarget << "\n";
    peaksTextStream << "Full " <<  QString(PRODUCTNAME) << " settings: ";
    peaksTextStream << theMainWindow->simSettings->printSettings();
    peaksTextStream << "\n";

    for (int i = 0; i < environmentNumber; i++)
    {
        peaksTextStream << "\nEnvironment number " << i;
        for (int j = 0; j < maskNumber; j++)
        {
            peaksTextStream << "\nMask: " << j << ": ";
            for (int k = 0; k < genomeSize; k++)peaksTextStream << masks[i][j][k];
        }
    }

    peaksTextStream << "\n\nGenomes tested: " << max << ", distribution:\n";
    for (int i = 0; i < genomeSize * maskNumber; i++)peaksTextStream << "Fit to environment: " << i << " Number of genomes: " << totals[i] << "\n";

    peaksTextStream << "\n\nGenome fit to environment as follows:\n";
    for (int i = 0; i < genomes.length(); i++)
        if (!genomes[i].empty())
        {
            peaksTextStream << "Fit to environment " << i << "\n";
            for (int j = 0; j < genomes[i].length(); j++)
                peaksTextStream << printGenomeInteger(genomes[i][j], genomeSize, lookups) << "\n";
        }
    peaksTextStream << "\n";
    peaksFile.close();

    theMainWindow->hideProgressBar();
}


