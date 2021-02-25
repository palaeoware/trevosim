#ifndef SIMULATION_H
#define SIMULATION_H

#include "simulation_variables.h"
#include "organism.h"
#include "mainwindow.h"
#include "version.h"

#include <QCoreApplication>

class simulation
{

public:
    simulation(MainWindow *theMainWindowCon, int runsCon, const simulationVariables *simSettingsCon);

    //Functions required for run
    void run();
    void countPeaks(MainWindow *theMainWindow);
    int returninformativeCharacters();

    friend class test;

private:
    //Variables for a run
    int runGenomeSize;
    int runFitnessTarget;
    int runSpeciesDifference;
    int informativeCharacters;
    int runs;
    int speciesCount;
    int iterations;

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
    //Vector for making cumalitve species curve if required
    QVector <int> graphing;

    //Print to string for files
    QString printNewick(int species, QVector<Organism *> &speciesList, int totalSpeciesCount);
    QString printNewickWithBranchLengths(int species, QVector<Organism *> &speciesList, bool phangornTree, int totalSpeciesCount);
    QString printTime();
    QString printMatrix(const QVector<Organism *> &speciesList);
    QString printStochasticMatrix(const QVector <Organism *> &speciesList, bool stochasticLayer);
    QString printGenomeString(const Organism *org);
    QString printGenomeInteger(quint64 genomeLocal, int genomeSizeLocal, const quint64 *lookupsLocal);
    QString printPlayingField(const QVector <playingFieldStructure *> &playingFields);
    QString printSpeciesList(const QVector <Organism *> &speciesList);
    QString printMasks(const QVector <playingFieldStructure *> &playingFields, int playingfield);
    QString printMasks(const QVector <playingFieldStructure *> &playingFields);

    //Deal with data
    void clearVectors(QVector <playingFieldStructure *> &playingFields, QVector <Organism *> &speciesList);

    //Utility functions
    bool setupSaveDirectory(QString savePath, MainWindow *theMainWindow);
    Organism initialise();
    int coinToss(const playingFieldStructure *pf);
    void mutateOrganism(Organism &progeny, const playingFieldStructure *pf);
    void mutateEnvironment();
    void newSpecies(Organism &progeny, Organism &parent, playingFieldStructure *pf);
    void updateTNTstring(QString &TNTstring, int progParentSpeciesID, int progSpeciesID);
    int calculateOverwrite(const playingFieldStructure *pf);
    void applyPerturbation();
    void testForUninformative(const QVector <Organism *> &speciesList, QList <int> &uninformativeCoding);
    bool stripUninformativeCharacters(QVector<Organism *> &speciesList, const QList <int> &uninformativeCoding);
    bool checkForUnresolvableTaxa(const QVector<Organism *> &speciesList, QString &unresolvableTaxonGroups, int &unresolvableCount);
    bool writeFile(const QString logFileNameBase, const QString logFileExtension, const QString logFileString, const QHash<QString, QString> &outValues, const QVector<Organism *> &speciesList);

    //Simulation calculations
    int fitness(const Organism *org, const QVector<QVector<QVector<bool> > > &masks, int runFitnessSize, int runFitnessTarget);
    int genomeDifference(const Organism *organismOne, const Organism *organismTwo);
    QHash<QString, QVector<int> > checkForExtinct(const QVector <Organism *> &speciesList);
    void speciesExtinction(Organism *org, const Organism *playingField, int extinctIteration, bool samsonian);
    int differenceToParent(const Organism *organismOne, int runSelectSize);
};

#endif // SIMULATION_H
