#ifndef SIMULATION_VARIABLES_H
#define SIMULATION_VARIABLES_H

#include <QString>
#include <QFile>

class simulationVariables
{
public:
    simulationVariables();
    QString printSettings() const;
    bool loadSettings(QFile *settingsFile);
    void saveSettings(QFile *settingsFile);

    //Ints
    int environmentNumber;
    int fitnessSize;
    int fitnessTarget;
    int genomeSize;
    int maskNumber;
    int mixingProbabilityZeroToOne;
    int mixingProbabilityOneToZero;
    int playingfieldSize;
    int playingfieldNumber;
    int playingfieldMasksMode;
    int speciesSelectSize;
    int speciesDifference;
    int runForTaxa;
    int unresolvableCutoff;
    int stochasticDepth;
    int stochasticMap[16];
    int test;
    int runMode;
    int runForIterations;
    int ecosystemEngineeringFrequency;
    int runningLogFrequency;
    int replicates;

    //Bools
    bool discardDeleterious;
    bool environmentalPerturbation;
    bool mixing;
    bool mixingPerturbation;
    bool genomeOnExtinction;
    bool stripUninformative;
    bool workingLog;
    bool writeTree;
    bool noSelection;
    bool randomSeed;
    bool randomOverwrite;
    bool stochasticLayer;
    bool expandingPlayingfield;
    bool matchFitnessPeaks;
    bool ecosystemEngineers;
    bool ecosystemEngineersArePersistent;
    bool ecosystemEngineersAddMask;
    bool writeRunningLog;
    bool writeFileOne;
    bool writeFileTwo;
    bool writeEE;

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
    QString runningLogString;

    //And saving files
    QString savePathDirectory;
};

#endif // SIMULATION_VARIABLES_H
