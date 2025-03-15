#ifndef SIMULATION_H
#define SIMULATION_H

#include "simulation_variables.h"
#include "organism.h"
#include "mainwindow.h"

#include <QDir>

class simulation
{

public:
    simulation(int runsCon, const simulationVariables *simSettingsCon, bool *error, MainWindow *theMainWindowCon = nullptr, bool calculateStripUninformativeFactorRunningCon = false);

    //Functions required for run
    bool run();
    int countPeaks(int genomeSize, int repeat = -1, int environment = -1);
    int returninformativeCharacters();

    bool calculateStripUninformativeFactorRunning = false;

    friend class testinternal;

private:
    //Variables for a run
    int runGenomeSize;
    int runSelectSize;
    int runFitnessSize;
    int runFitnessTarget;
    int runMaskNumber;
    int runSpeciesDifference;
    int runMixingProbabilityOneToZero;
    int runMixingProbabilityZeroToOne;
    int informativeCharacters;
    int runs;
    int speciesCount;
    int iterations;
    int perturbationStart, perturbationEnd, perturbationOccurring;
    int ecosystemEngineeringOccurring;
    int environmentalPerturbationCopyRate;
    //One vector to keep a copy of masks, the other to keep a copy of which elements of the original masks have been copied over the perturbation masks
    QVector <QVector <QVector <QVector <bool> > > > environmentalPerturbationMasksCopy;
    QVector <QVector <QVector <QVector <bool> > > > environmentalPerturbationOverwriting;
    //Keep track for printing
    QList <bool> extinctList;

    const simulationVariables *simSettings;

    struct playingFieldStructure
    {
        QVector <Organism *> playingField;
        QVector <QVector <QVector <bool> > > masks;
    };

    QVector <playingFieldStructure *> playingFields;

    //Other variables
    QDir savePathDirectory;
    QFile workLogFile;
    MainWindow *theMainWindow;
    QTextStream workLogTextStream;
    quint32 maxRand;

    //Print to string for files
    QString printNewick(int species, QVector<Organism *> &speciesList);
    QString printNewickWithBranchLengths(int species, QVector<Organism *> &speciesList, bool phangornTree);
    QString printTime();
    QString printMatrix(const QVector<Organism *> &speciesList);
    QString printStochasticMatrix(const QVector <Organism *> &speciesList, bool stochasticLayer);
    QString printGenomeInteger(quint64 genomeLocal, int genomeSizeLocal, const quint64 *lookupsLocal);
    QString printGenomeString(const Organism *org);
    QString printPlayingField(const QVector <playingFieldStructure *> &playingFields);
    QString printPlayingFieldSemiconcise(const QVector <playingFieldStructure *> &playingFields);
    QString printPlayingFieldConcise(const QVector <playingFieldStructure *> &playingFields);
    QString printPlayingFieldGenomesConcise(const QVector <playingFieldStructure *> &playingFields);
    QString printSpeciesList(const QVector <Organism *> &speciesList);
    QString printMasks(const QVector <playingFieldStructure *> &playingFields, int playingfield);
    QString printMasks(const QVector <playingFieldStructure *> &playingFields);
    QString printEcosystemEngineers(const QVector <Organism *> &speciesList);
    QString doPadding(int number, int significantFigures);
    int paddingAmount(int taxonNumber);
    void printCountPeaks(int genomeSize, QVector <quint64> &totals, QVector <QVector <quint64> > &genomes, int repeat);

    //Deal with data
    void clearVectors(QVector <playingFieldStructure *> &playingFields, QVector <Organism *> &speciesList);

    //Utility functions
    Organism initialise();
    int coinToss(const playingFieldStructure *pf);
    void mutateOrganism(Organism &progeny, const playingFieldStructure *pf);
    void mutateEnvironment();
    void newSpecies(Organism &progeny, Organism &parent, playingFieldStructure *pf);
    void updateTNTstring(QString &TNTstring, int progParentSpeciesID, int progSpeciesID);
    int calculateOverwrite(const playingFieldStructure *pf, const int speciesNumber);
    void applyPerturbation();
    void applyPlayingfieldMixing(QVector<Organism *> &speciesList);
    void applyEcosystemEngineering(QVector<Organism *> &speciesList, bool writeEcosystemEngineers);
    void checkForUninformative(QVector <Organism *> &speciesList, QList <int> &uninformativeCoding, QList <int> &uninformativeNonCoding);
    bool checkForCharacterNumber(QList <int> &uninformativeCoding, QList <int> &uninformativeNonCoding);
    bool stripUninformativeCharacters(QVector<Organism *> &speciesList, const QList <int> &uninformativeCoding, const QList <int> &uninformativeNonCoding);
    bool checkForUnresolvableTaxa(QVector<Organism *> &speciesList, QString &unresolvableTaxonGroups, int &unresolvableCount);
    bool writeFile(const QString logFileNameBase, const QString logFileExtension, const QString logFileString, const QHash<QString, QString> &outValues, const QVector<Organism *> &speciesList);
    bool writeRunningLog(const int iterations, const QString logFileString);
    bool writeEEFile(const int iterations, const QString logFileString);
    bool setupSaveDirectory(QString subFolder = "");
    void warning(QString header, QString message);
    void writeGUI(QVector<Organism *> &speciesList);

    //Simulation calculations
    int fitness(const Organism *org, const QVector<QVector<QVector<bool> > > &masks, int runFitnessSize, int runFitnessTarget, int runMaskNumber, int fitnessMode, int environment = -1);
    int genomeDifference(const Organism *organismOne, const Organism *organismTwo, const int selectSize = -1);
    QHash<QString, QVector<int> > checkForExtinct(const QVector <Organism *> &speciesList);
    void speciesExtinction(Organism *speciesListOrganism, const Organism *playingFieldOrganism, int extinctIteration, bool samsonian, bool stochastic, bool test = false);
    bool checkForSpeciation(const Organism *organismOne, int runSelectSize, int runSpeciesDifference, int speciationMode);
};

#endif // SIMULATION_H
