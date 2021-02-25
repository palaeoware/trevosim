#include "simulation_variables.h"
#include <QStandardPaths>

simulationVariables::simulationVariables()
{
    //Set variables for simulation - values chosen to work out of the box
    //Ints
    genomeSize = 128;
    fitnessTarget = 0;
    taxonNumber = 32;
    playingfieldSize = 12;
    playingfieldNumber = 1;
    // 0 is identical, 1 independent, 2 identical at start
    playingfieldMasksMode = 0;
    speciesDifference = 5;
    unresolvableCutoff = 5;
    environmentNumber = 1;
    maskNumber = 3;
    test = 0;

    //Doubles
    environmentMutationRate = 1.0;
    organismMutationRate = 1.0;
    selectionCoinToss = 2.0;
    stripUninformativeFactor = -1.;

    //Bools
    stripUninformative = false;
    sansomianSpeciation = true;
    discardDeleterious = false;
    speciesCurve = false;
    workingLog = false;
    randomSeed = false;
    randomOverwrite = false;
    append = true;
    writeTree = true;

    //Strings
    QString base = (QString(PRODUCTNAME) + "_");
    stripUninformativeFactorSettings = "UNSET";
    logFileNameBase01 = base;
    logFileNameBase02 = base;
    logFileNameBase03 = base;
    logFileNameBase03.append("tree_");
    logFileExtension01 = ".tnt";
    logFileExtension02 = ".nex";
    logFileExtension03 = ".nex";
    logFileString01 = "Please enter output text (options in output dialogue).";
    logFileString02 = "Please enter output text (options in output dialogue).";
    logFileString03 = "#NEXUS\n\n[ ||Time|| ||Settings|| ] \n\n Begin trees;\nTranslate\n";
    savePathDirectory = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QString simulationVariables::printSettings() const
{

    QString settingsString;
    QTextStream settingsTextStream(&settingsString);

    settingsTextStream << "variables : genomeSize " << genomeSize
                       //RJG - some stuff redacted for 2.0.0 - email if you're interested
                       << " taxonNumber " << taxonNumber
                       << " playingfieldSize " << playingfieldSize
                       << " speciesDifference " << speciesDifference
                       << " environmentMutationRate " << environmentMutationRate
                       << " organismMutationRate " << organismMutationRate
                       << " unresolvableCutoff " << unresolvableCutoff
                       << " environmentNumber " << environmentNumber
                       << " maskNumber " << maskNumber
                       << " stripUninformative " << stripUninformative
                       << " append " << append
                       << " writeTree " << writeTree
                       << " sansomianSpeciation " << sansomianSpeciation
                       << " discardDeleterious " << discardDeleterious
                       << " speciesCurve " << speciesCurve
                       << " fitnessTarget " << fitnessTarget
                       << " doublePlayingfield " << playingfieldNumber
                       << " playingfieldMasksMode " << playingfieldMasksMode
                       << " selection " << selectionCoinToss
                       << " randomOverwrite " << randomOverwrite;


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
            if (settingsFileIn.name() == "TREvoSim")continue;
            if (settingsFileIn.name() == "genomeSize")genomeSize = settingsFileIn.readElementText().toInt();
            //RJG - some stuff redacted for 2.0.0 - email if you're interested
            if (settingsFileIn.name() == "taxonNumber")taxonNumber = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "playingfieldSize")playingfieldSize = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "speciesDifference")speciesDifference = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "unresolvableCutoff")unresolvableCutoff = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "environmentNumber")environmentNumber = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "maskNumber")maskNumber = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "fitnessTarget")fitnessTarget = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "playingfieldNumber")playingfieldNumber = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "playingfieldMasksMode")playingfieldMasksMode = settingsFileIn.readElementText().toInt();

            //Double
            if (settingsFileIn.name() == "stripUninformativeFactor")stripUninformativeFactor = settingsFileIn.readElementText().toDouble();
            if (settingsFileIn.name() == "environmentMutationRate")environmentMutationRate = settingsFileIn.readElementText().toDouble();
            if (settingsFileIn.name() == "organismMutationRate")organismMutationRate = settingsFileIn.readElementText().toDouble();
            if (settingsFileIn.name() == "selectionCoinToss")selectionCoinToss = settingsFileIn.readElementText().toDouble();

            //Bools
            if (settingsFileIn.name() == "stripUninformative")stripUninformative = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "append")append = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "writeTree")writeTree = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "sansomianSpeciation")sansomianSpeciation = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "discardDeleterious")discardDeleterious = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "speciesCurve")speciesCurve = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "randomOverwrite")randomOverwrite = settingsFileIn.readElementText().toInt();

            //Strings
            if (settingsFileIn.name() == "logFileNameBase01")logFileNameBase01 = settingsFileIn.readElementText();
            if (settingsFileIn.name() == "logFileNameBase02")logFileNameBase02 = settingsFileIn.readElementText();
            if (settingsFileIn.name() == "logFileExtension01")logFileExtension01 = settingsFileIn.readElementText();
            if (settingsFileIn.name() == "logFileExtension02")logFileExtension02 = settingsFileIn.readElementText();
            if (settingsFileIn.name() == "logFileString01")logFileString01 = settingsFileIn.readElementText();
            if (settingsFileIn.name() == "logFileString02")logFileString02 = settingsFileIn.readElementText();
            if (settingsFileIn.name() == "savePathDirectory")savePathDirectory = (settingsFileIn.readElementText());
            if (settingsFileIn.name() == "stripUninformativeFactorSettings")stripUninformativeFactorSettings = settingsFileIn.readElementText();
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

    //RJG - some stuff redacted for 2.0.0 - email if you're interested

    settingsFileOut.writeStartElement("taxonNumber");
    settingsFileOut.writeCharacters(QString("%1").arg(taxonNumber));
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

    settingsFileOut.writeStartElement("playingfieldMasksMode");
    settingsFileOut.writeCharacters(QString("%1").arg(playingfieldMasksMode));
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

    settingsFileOut.writeStartElement("append");
    settingsFileOut.writeCharacters(QString("%1").arg(append));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("writeTree");
    settingsFileOut.writeCharacters(QString("%1").arg(writeTree));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("sansomianSpeciation");
    settingsFileOut.writeCharacters(QString("%1").arg(sansomianSpeciation));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("discardDeleterious");
    settingsFileOut.writeCharacters(QString("%1").arg(discardDeleterious));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("speciesCurve");
    settingsFileOut.writeCharacters(QString("%1").arg(speciesCurve));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("randomOverwrite");
    settingsFileOut.writeCharacters(QString("%1").arg(randomOverwrite));
    settingsFileOut.writeEndElement();

    //Strings
    settingsFileOut.writeStartElement("logFileNameBase01");
    settingsFileOut.writeCharacters(logFileNameBase01);
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("logFileNameBase02");
    settingsFileOut.writeCharacters(logFileNameBase02);
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

    settingsFileOut.writeEndDocument();
}
