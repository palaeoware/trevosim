#ifndef SIMULATION_VARIABLES_H
#define SIMULATION_VARIABLES_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "version.h"

class simulationVariables
{
public:
    simulationVariables();
    QString printSettings() const;
    bool loadSettings(QFile *settingsFile);
    void saveSettings(QFile *settingsFile);

    //Ints
    int environmentNumber;
    int fitnessTarget;
    int genomeSize;
    int maskNumber;
    int playingfieldSize;
    int playingfieldNumber;
    int playingfieldMasksMode;
    int speciesDifference;
    int taxonNumber;
    int unresolvableCutoff;
    int test;

    //Bools
    bool append;
    bool discardDeleterious;
    bool sansomianSpeciation;
    bool speciesCurve;
    bool stripUninformative;
    bool workingLog;
    bool writeTree;
    bool randomSeed;
    bool randomOverwrite;

    //Doubles
    double environmentMutationRate;
    double organismMutationRate;
    double stripUninformativeFactor;
    double selectionCoinToss;

    //Strings - for logging
    QString logFileNameBase01;
    QString logFileNameBase02;
    QString logFileNameBase03;
    QString logFileExtension01;
    QString logFileExtension02;
    QString logFileExtension03;
    QString stripUninformativeFactorSettings;
    QString logFileString01;
    QString logFileString02;
    QString logFileString03;
    //And saving files
    QString savePathDirectory;
};

#endif // SIMULATION_VARIABLES_H
