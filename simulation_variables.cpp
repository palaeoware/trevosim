#include "simulation_variables.h"
#include "mainwindow.h"
#include "version.h"

#include <QStandardPaths>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

simulationVariables::simulationVariables()
{
    //Set variables for simulation - values chosen to work out of the box
    //Ints
    genomeSize = 128;
    speciesSelectSize = 128;
    fitnessSize = 128;
    fitnessTarget = 50;
    playingfieldSize = 20;
    playingfieldNumber = 1;
    ecosystemEngineeringFrequency = 10;
    // 0 is identical, 1 independent, 2 identical at start
    playingfieldMasksMode = MASKS_MODE_IDENTICAL;
    speciesDifference = 4;
    unresolvableCutoff = 5;
    environmentNumber = 1;
    maskNumber = 5;
    mixingProbabilityZeroToOne = 0;
    mixingProbabilityOneToZero = 0;
    stochasticDepth = 1;
    for (auto &i : stochasticMap) i = 0;
    test = 0;
    runMode = RUN_MODE_TAXON;
    runForIterations = 1000;
    runForTaxa = 32;
    runningLogFrequency = 50;
    replicates = 25;

    //Doubles
    environmentMutationRate = 1.0;
    organismMutationRate = 2.0;
    selectionCoinToss = 10.0;
    stripUninformativeFactor = 5.;

    //Bools
    stripUninformative = false;
    sansomianSpeciation = true;
    discardDeleterious = false;
    workingLog = false;
    environmentalPerturbation = false;
    mixing = false;
    mixingPerturbation = false;
    noSelection = false;
    randomSeed = false;
    randomOverwrite = false;
    stochasticLayer = false;
    expandingPlayingfield = false;
    matchFitnessPeaks = false;
    ecosystemEngineers = false;
    ecosystemEngineersArePersistent = false;
    ecosystemEngineersAddMask = false;
    writeRunningLog = false;
    writeTree = true;
    writeFileOne = true;
    writeFileTwo = true;
    writeEE = false;

    //Strings
    QString base = (QString(PRODUCTNAME) + "_");
    stripUninformativeFactorSettings = "UNSET";
    logFileNameBase01 = base;
    logFileNameBase02 = base;
    logFileNameBase03 = base;
    logFileNameBase03.append("tree_");
    logFileExtension01 = ".nex";
    logFileExtension02 = ".tnt";
    logFileExtension03 = ".nex";
    logFileString01 =
        "#NEXUS\n[||Settings||]\n\nBEGIN DATA;\n	DIMENSIONS NTAX=||Taxon_Number|| NCHAR=||Character_Number||;\n	FORMAT SYMBOLS= \" 0 1 \" MISSING=? GAP=- ;\nMATRIX\n\n||Matrix||\n;\nend;\n\nBEGIN ASSUMPTIONS;\n 	TYPESET * UNTITLED = unord: 1-||Character_Number||;\nEND;";
    logFileString02 =
        "mxram 100;\nNSTATES nogaps;\nxread\n'Written on ||Time|| Variables: ||Settings||'\n||Character_Number|| ||Taxon_Number||\n||Matrix||\n;\npiwe-;\nkeep 0; hold 100000;\nrseed *;\nxmult = level 10; bbreak;\nexport - TREvoSim_run_||Count||_mpts.nex;\nxwipe;";
    logFileString03 = "#NEXUS\n\n[ ||Time|| ||Settings|| ] \n\n Begin trees;\nTranslate\n";
    runningLogString = "Please enter output text (options in output dialogue).";
    savePathDirectory = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QString simulationVariables::printSettings() const
{

    QString settingsString;
    QTextStream settingsTextStream(&settingsString);

    settingsTextStream << "variables : genomeSize " << genomeSize
                       << " speciesSelectSize " << speciesSelectSize
                       << " fitnessSize " << fitnessSize
                       << " runForTaxa " << runForTaxa
                       << " runForIterations " << runForIterations
                       << " playingfieldSize " << playingfieldSize
                       << " speciesDifference " << speciesDifference
                       << " environmentMutationRate " << environmentMutationRate
                       << " organismMutationRate " << organismMutationRate
                       << " unresolvableCutoff " << unresolvableCutoff
                       << " environmentNumber " << environmentNumber
                       << " maskNumber " << maskNumber
                       << " runMode " << runMode
                       << " stripUninformative " << stripUninformative
                       << " writeTree " << writeTree
                       << " writeRunningLog " << writeRunningLog
                       << " writeFileOne " << writeFileOne
                       << " writeFileTwo " << writeFileTwo
                       << " writeEE " << writeEE
                       << " noSelection " << noSelection
                       << " randomSeed " << randomSeed
                       << " sansomianSpeciation " << sansomianSpeciation
                       << " discardDeleterious " << discardDeleterious
                       << " fitnessTarget " << fitnessTarget
                       << " playingfieldNumber " << playingfieldNumber
                       << " mixing " << mixing
                       << " mixingProbabilityZeroToOne " << mixingProbabilityZeroToOne
                       << " mixingProbabilityOneToZero " << mixingProbabilityOneToZero
                       << " playingfieldMasksMode " << playingfieldMasksMode
                       << " selection " << selectionCoinToss
                       << " randomOverwrite " << randomOverwrite
                       << " ecosystemEngineers " << ecosystemEngineers
                       << " ecosystemEngineersArePersistent " <<  ecosystemEngineersArePersistent
                       << " ecosystemEngineeringFrequency " << ecosystemEngineeringFrequency
                       << " ecosystemEngineersAddMask " << ecosystemEngineersAddMask
                       << " runningLogFrequency " << runningLogFrequency
                       << " replicates " << replicates
                       << " expandingPlayingfield" << expandingPlayingfield
                       << " stochasticLayer " << stochasticLayer
                       << " stochasticDepth " << stochasticDepth
                       << " matchFitnessPeaks " << matchFitnessPeaks
                       << " stochasticMap ";

    for (int i = 0; i < 16; i++) settingsTextStream << stochasticMap[i];

    return settingsString;
}

bool simulationVariables::loadSettings(QFile *settingsFile)
{
    QXmlStreamReader settingsFileIn(settingsFile);

    while (!settingsFileIn.atEnd() && !settingsFileIn.hasError())
    {

        /* Read next element.*/
        QXmlStreamReader::TokenType token = settingsFileIn.readNext();
        /* If token is just StartDocument, we'll go to next.*/

        if (token == QXmlStreamReader::StartDocument)continue;
        if (token == QXmlStreamReader::StartElement)
        {
            //Ints
            if (settingsFileIn.name().toString() == "TREvoSim")continue;
            if (settingsFileIn.name().toString() == "genomeSize")genomeSize = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "speciesSelectSize")speciesSelectSize = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "fitnessSize")fitnessSize = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "runForTaxa")runForTaxa = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "runForIterations")runForIterations = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "playingfieldSize")playingfieldSize = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "speciesDifference")speciesDifference = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "unresolvableCutoff")unresolvableCutoff = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "environmentNumber")environmentNumber = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "maskNumber")maskNumber = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "fitnessTarget")fitnessTarget = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "playingfieldNumber")playingfieldNumber = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "mixingProbabilityOneToZero")mixingProbabilityOneToZero = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "mixingProbabilityZeroToOne")mixingProbabilityZeroToOne = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "playingfieldMasksMode")playingfieldMasksMode = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "stochasticDepth")stochasticDepth = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "runMode")runMode = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "ecosystemEngineeringFrequency")ecosystemEngineeringFrequency = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "runningLogFrequency")runningLogFrequency = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "replicates")replicates = settingsFileIn.readElementText().toInt();

            //Double
            if (settingsFileIn.name().toString() == "stripUninformativeFactor")stripUninformativeFactor = settingsFileIn.readElementText().toDouble();
            if (settingsFileIn.name().toString() == "environmentMutationRate")environmentMutationRate = settingsFileIn.readElementText().toDouble();
            if (settingsFileIn.name().toString() == "organismMutationRate")organismMutationRate = settingsFileIn.readElementText().toDouble();
            if (settingsFileIn.name().toString() == "selectionCoinToss")selectionCoinToss = settingsFileIn.readElementText().toDouble();

            //Bools
            if (settingsFileIn.name().toString() == "stripUninformative")stripUninformative = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "writeTree")writeTree = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "noSelection")noSelection = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "randomSeed")randomSeed = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "sansomianSpeciation")sansomianSpeciation = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "discardDeleterious")discardDeleterious = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "environmentalPerturbation")environmentalPerturbation = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "mixing")mixing = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "mixingPerturbation")mixingPerturbation = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "randomOverwrite")randomOverwrite = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "stochasticLayer")stochasticLayer = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "expandingPlayingfield ")expandingPlayingfield = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "matchFitnessPeaks")matchFitnessPeaks = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "ecosystemEngineers")ecosystemEngineers = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "ecosystemEngineersArePersistent")ecosystemEngineersArePersistent = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "ecosystemEngineersAddMask")ecosystemEngineersAddMask = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "writeRunningLog")writeRunningLog = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "writeFileOne")writeFileOne = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "writeFileTwo")writeFileTwo = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name().toString() == "writeEE")writeEE = settingsFileIn.readElementText().toInt();

            //Strings
            if (settingsFileIn.name().toString() == "logFileNameBase01")logFileNameBase01 = settingsFileIn.readElementText();
            if (settingsFileIn.name().toString() == "logFileNameBase02")logFileNameBase02 = settingsFileIn.readElementText();
            if (settingsFileIn.name().toString() == "logFileNameBase03")logFileNameBase03 = settingsFileIn.readElementText();
            if (settingsFileIn.name().toString() == "logFileExtension01")logFileExtension01 = settingsFileIn.readElementText();
            if (settingsFileIn.name().toString() == "logFileExtension02")logFileExtension02 = settingsFileIn.readElementText();
            if (settingsFileIn.name().toString() == "logFileString01")logFileString01 = settingsFileIn.readElementText();
            if (settingsFileIn.name().toString() == "logFileString02")logFileString02 = settingsFileIn.readElementText();
            if (settingsFileIn.name().toString() == "savePathDirectory")savePathDirectory = (settingsFileIn.readElementText());
            if (settingsFileIn.name().toString() == "stripUninformativeFactorSettings")stripUninformativeFactorSettings = settingsFileIn.readElementText();
            if (settingsFileIn.name().toString() == "runningLogString")runningLogString = settingsFileIn.readElementText();

            if (settingsFileIn.name().toString() == "stochasticMap")
            {
                QString in = settingsFileIn.readElementText();

                for (int i = 0; i < 16; i++)
                {
                    QString inMap = in.at(i);
                    stochasticMap[i] = inMap.toInt();
                }
            }
        }
    }
    // Error
    if (settingsFileIn.hasError()) return false;
    return true;
}

void simulationVariables::saveSettings(QFile *settingsFile)
{
    QXmlStreamWriter settingsFileOut(settingsFile);
    settingsFileOut.setAutoFormatting(true);
    settingsFileOut.setAutoFormattingIndent(-2);

    settingsFileOut.writeStartDocument();

    settingsFileOut.writeStartElement("TREvoSim");

    //Ints
    settingsFileOut.writeStartElement("genomeSize");
    settingsFileOut.writeCharacters(QString("%1").arg(genomeSize));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("speciesSelectSize");
    settingsFileOut.writeCharacters(QString("%1").arg(speciesSelectSize));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("fitnessSize");
    settingsFileOut.writeCharacters(QString("%1").arg(fitnessSize));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("runForTaxa");
    settingsFileOut.writeCharacters(QString("%1").arg(runForTaxa));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("runForIterations");
    settingsFileOut.writeCharacters(QString("%1").arg(runForIterations));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("playingfieldSize");
    settingsFileOut.writeCharacters(QString("%1").arg(playingfieldSize));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("speciesDifference");
    settingsFileOut.writeCharacters(QString("%1").arg(speciesDifference));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("unresolvableCutoff");
    settingsFileOut.writeCharacters(QString("%1").arg(unresolvableCutoff));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("environmentNumber");
    settingsFileOut.writeCharacters(QString("%1").arg(environmentNumber));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("maskNumber");
    settingsFileOut.writeCharacters(QString("%1").arg(maskNumber));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("fitnessTarget");
    settingsFileOut.writeCharacters(QString("%1").arg(fitnessTarget));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("playingfieldNumber");
    settingsFileOut.writeCharacters(QString("%1").arg(playingfieldNumber));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("mixingProbabilityZeroToOne");
    settingsFileOut.writeCharacters(QString("%1").arg(mixingProbabilityZeroToOne));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("mixingProbabilityOneToZero");
    settingsFileOut.writeCharacters(QString("%1").arg(mixingProbabilityOneToZero));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("playingfieldMasksMode");
    settingsFileOut.writeCharacters(QString("%1").arg(playingfieldMasksMode));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("stochasticDepth");
    settingsFileOut.writeCharacters(QString("%1").arg(stochasticDepth));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("runMode");
    settingsFileOut.writeCharacters(QString("%1").arg(runMode));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("ecosystemEngineeringFrequency");
    settingsFileOut.writeCharacters(QString("%1").arg(ecosystemEngineeringFrequency));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("runningLogFrequency");
    settingsFileOut.writeCharacters(QString("%1").arg(runningLogFrequency));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("replicates");
    settingsFileOut.writeCharacters(QString("%1").arg(replicates));
    settingsFileOut.writeEndElement();

    //Double
    settingsFileOut.writeStartElement("stripUninformativeFactor");
    settingsFileOut.writeCharacters(QString("%1").arg(stripUninformativeFactor));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("environmentMutationRate");
    settingsFileOut.writeCharacters(QString("%1").arg(environmentMutationRate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("organismMutationRate");
    settingsFileOut.writeCharacters(QString("%1").arg(organismMutationRate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("selectionCoinToss");
    settingsFileOut.writeCharacters(QString("%1").arg(selectionCoinToss));
    settingsFileOut.writeEndElement();

    //Bools
    settingsFileOut.writeStartElement("stripUninformative");
    settingsFileOut.writeCharacters(QString("%1").arg(stripUninformative));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("writeTree");
    settingsFileOut.writeCharacters(QString("%1").arg(writeTree));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("noSelection");
    settingsFileOut.writeCharacters(QString("%1").arg(noSelection));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("randomSeed");
    settingsFileOut.writeCharacters(QString("%1").arg(randomSeed));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("sansomianSpeciation");
    settingsFileOut.writeCharacters(QString("%1").arg(sansomianSpeciation));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("discardDeleterious");
    settingsFileOut.writeCharacters(QString("%1").arg(discardDeleterious));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("environmentalPerturbation");
    settingsFileOut.writeCharacters(QString("%1").arg(environmentalPerturbation));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("mixing");
    settingsFileOut.writeCharacters(QString("%1").arg(mixing));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("mixingPerturbation");
    settingsFileOut.writeCharacters(QString("%1").arg(mixingPerturbation));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("randomOverwrite");
    settingsFileOut.writeCharacters(QString("%1").arg(randomOverwrite));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("stochasticLayer");
    settingsFileOut.writeCharacters(QString("%1").arg(stochasticLayer));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("expandingPlayingfield");
    settingsFileOut.writeCharacters(QString("%1").arg(expandingPlayingfield));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("matchFitnessPeaks");
    settingsFileOut.writeCharacters(QString("%1").arg(matchFitnessPeaks));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("ecosystemEngineers");
    settingsFileOut.writeCharacters(QString("%1").arg(ecosystemEngineers));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("ecosystemEngineersArePersistent");
    settingsFileOut.writeCharacters(QString("%1").arg(ecosystemEngineersArePersistent));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("ecosystemEngineersAddMask");
    settingsFileOut.writeCharacters(QString("%1").arg(ecosystemEngineersAddMask));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("writeRunningLog");
    settingsFileOut.writeCharacters(QString("%1").arg(writeRunningLog));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("writeFileOne");
    settingsFileOut.writeCharacters(QString("%1").arg(writeFileOne));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("writeFileTwo");
    settingsFileOut.writeCharacters(QString("%1").arg(writeFileTwo));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("writeEE");
    settingsFileOut.writeCharacters(QString("%1").arg(writeEE));
    settingsFileOut.writeEndElement();

    //Strings
    settingsFileOut.writeStartElement("logFileNameBase01");
    settingsFileOut.writeCharacters(logFileNameBase01);
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("logFileNameBase02");
    settingsFileOut.writeCharacters(logFileNameBase02);
    settingsFileOut.writeEndElement();


    settingsFileOut.writeStartElement("logFileNameBase03");
    settingsFileOut.writeCharacters(logFileNameBase03);
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("logFileExtension01");
    settingsFileOut.writeCharacters(logFileExtension01);
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("logFileExtension02");
    settingsFileOut.writeCharacters(logFileExtension02);
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("logFileString01");
    settingsFileOut.writeCharacters(logFileString01);
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("logFileString02");
    settingsFileOut.writeCharacters(logFileString02);
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("savePathDirectory");
    settingsFileOut.writeCharacters(savePathDirectory);
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("stripUninformativeFactorSettings");
    settingsFileOut.writeCharacters(stripUninformativeFactorSettings);
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("runningLogString");
    settingsFileOut.writeCharacters(runningLogString);
    settingsFileOut.writeEndElement();

    QString map;
    QTextStream out(&map);
    for (int i = 0; i < 16; i++) out << stochasticMap[i];
    settingsFileOut.writeStartElement("stochasticMap");
    settingsFileOut.writeCharacters(map);
    settingsFileOut.writeEndElement();

    settingsFileOut.writeEndElement();

    settingsFileOut.writeEndDocument();
}
