#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Global define
#define MAX_RAND 65535
#define STRIP_UNINFORM_FACTOR 12

#include "about.h"
#include "organism.h"
#include "output.h"
#include "settings.h"
#include "simulation_variables.h"

#include <algorithm>

#include <QAbstractScrollArea>
#include <QAction>
#include <QActionGroup>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QtMath>
#include <QProgressBar>
#include <QPushButton>
#include <QShortcut>
#include <QStandardPaths>
#include <QStringList>
#include <QString>
#include <QTableWidget>
#include <QTextCodec>
#include <QTimer>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDateTime>

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
    void recalculateStripUniformativeFactor(bool running);
    void setStatus(QString message);
    void addProgressBar(int min, int max);
    void setProgressBar(int value);
    void hideProgressBar();
    void printGenome(const Organism *org, int row);
    void printBlank(int row);
    void resizeGrid();
    void resizeGrid(const simulationVariables &simSettings);
    void setPath(QString newPath);
    void setTreeDisplay(QString treeString);
    QString getPath();
    bool escapePressed, pauseFlag, batchRunning, calculateStripUninformativeFactorRunning;
    bool batchError;
    bool unresolvableBatch;


private:
    //GUI objects
    QAction *startButton;
    QAction *pauseButton;
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
    bool testMode;

    void finishRunGUI();
    void startRunGUI();

private slots:
    void startTriggered();
    void pauseTriggered();
    void resetTriggered();
    void runForTriggered();
    void changePathTriggered();
    void settingsTriggered();
    void outputTriggered();
    void aboutTriggered();
    void load();
    void save();
    void saveAs();
    void open();
    void escape();
    void setRandomSeed();
    void selectionHistogram();
    void setFactor();
    void setMultiplePlayingFields();
    void defaultSettings();

    //Analysis
    void countPeaks();
    void doTests();
};

#endif // MAINWINDOW_H
