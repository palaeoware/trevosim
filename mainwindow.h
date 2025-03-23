#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "organism.h"
#include "simulation_variables.h"

#include <QMainWindow>
#include <QTableWidget>
#include <QProgressBar>

//Mode for masks
// 0 is identical, 1 independent, 2 identical at start
#define MASKS_MODE_IDENTICAL 0
#define MASKS_MODE_INDEPENDENT 1
#define MASKS_MODE_IDENTICAL_START 2

//Run mode - i.e. continuous, until taxon number, until iteration number
#define RUN_MODE_CONTINUOUS 0
#define RUN_MODE_TAXON 1
#define RUN_MODE_ITERATION 2

//Speciation mode
#define SPECIES_MODE_ORIGIN 0
#define SPECIES_MODE_LAST_SPECIATION 1
#define SPECIES_MODE_ALL 2
#define SPECIES_MODE_MAYR 3

//Multiple environment fitness mode
#define FITNESS_MODE_MINIMUM 0
#define FITNESS_MODE_MEAN 1
#define FITNESS_MODE_VARIABLE 2

//Forward declaration to avoid circular dependencies in printGenome declaration
class Organism;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Ui::MainWindow *ui;

    //Variables for simulation
    simulationVariables *simSettings;

    //Access functions for updating GUI from simulation object
    void recalculateStripUniformativeFactor();
    void setStatus(QString message);
    void runFromCommandLine(QHash<QString, QString> parsedOptions);
    void setPath(QString newPath);
    void addProgressBar(int min, int max);
    void setProgressBar(int value);
    void hideProgressBar();
    void printGenome(const Organism *org, int row);
    void printBlank(int row);
    void resizeGrid(const int speciesNumber, const int genomeSize, const int hideFrom = 0);
    void resetDisplays();
    void setTreeDisplay(QString treeString);
    void hideRow(const int rowNumber);
    void showRow(const int rowNumber);
    int rowMax();
    bool escapePressed, pauseFlag, batchRunning, runFromCommand;

private:
    //GUI objects
    QAction *startButton;
    QAction *pauseButton;
    QAction *stopButton;
    QAction *resetButton;
    QAction *runForButton;
    QAction *settingsButton;
    QAction *logButton;
    QAction *testButton;
    QAction *aboutButton;
    QTableWidget *table;
    QLineEdit *path;
    QProgressBar *progress;
    QString settingsFileString;
    int runs;
    int countPeaksReplicatesGenomeSize;
    bool testMode;

    void finishRunGUI();
    void startRunGUI();

private slots:
    void startTriggered();
    void pauseTriggered();
    void resetTriggered();
    void runForTriggered(int runBatchFor);
    void changePathTriggered();
    void pathTextChanged(QString newPath);
    void settingsTriggered();
    void outputTriggered();
    void aboutTriggered();
    void load();
    void save();
    void saveAs();
    void open(QString fileName = QString());
    void escape();
    void selectionHistogram();
    void setFactor();

    void setMultiplePlayingFields();
    void defaultSettings();

    //Analysis
    void countPeaks();
    void doTests();
};

#endif // MAINWINDOW_H
