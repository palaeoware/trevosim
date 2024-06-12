#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "organism.h"
#include "version.h"
#include "simulation.h"
#include "settings.h"
#include "testinternal.h"
#include "batchdialog.h"
#include "output.h"
#include "about.h"

//#include <algorithm>
#include <QProgressDialog>
#include <QShortcut>
#include <QStandardPaths>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QFutureWatcher>
#include <QInputDialog>
#include <QRandomGenerator>
#include <QtConcurrent>


/*********** TREvoSim structure - overview ***********/
/*
 * There are four structures/classes that interact in TREvoSim, namely:
 * -- Organisms - each organism has a genome, a record of its parents genome, fitness, and can have a stochastic layer. Operators > and = are implemented.
 * -- Simulation:
 * ---- This object has a run and Fitness histogram function, local copies of important settings for the run (which change sometimes during a run)
 * -- There are lots of private functions to aid the simulation such as e.g. fitness, allowing these to tested individually
 * ---- The run function is sent a pointer to mainWindow, which is uses to communicate with the main window when a single run is occurring. When a batch is running, it is not sent this pointer, allowing batches to run in parallel
 * -- Tests are a friend class to simulation, providing under the hood access to the simulation. They are run on compile, and will throw an error if any fail
 * -- Simulation variables - this object contains the variables of the simulation. It is sent as a const to the simulation which makes local copies, and modified by the output and simulation classes. It also has functions to load, save and print setings
 * -- MainWindow - This handles the GUI and interacts with the other objects allowing user input etc.
 *
 */
/****************************************************/



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QIcon TREvoSimIcon =  QIcon (QPixmap("://resources/icon.svg"));
    TREvoSimIcon.addFile("://resources/icon.svg", QSize(16, 16));
    setWindowIcon(TREvoSimIcon);

    ui->setupUi(this);
    QString version = QString("%1.%2.%3").arg(MAJORVERSION).arg(MINORVERSION).arg(PATCHVERSION);
    setWindowTitle(QString(PRODUCTNAME) + " v" + version + " - compiled - " + __DATE__);

    //Create default settings object
    simSettings = new simulationVariables;

    //Set up toolbar...
    ui->mainToolBar->setIconSize(QSize(25, 25));
    startButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_button_green.png")), QString("Run"), this);
    pauseButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_pause_button_orange.png")), QString("Pause"), this);
    stopButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_stop_button_red.png")), QString("Stop"), this);
    resetButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_reset_button_red.png")), QString("Reset"), this);
    runForButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_n_button_green.png")), QString("Batch..."), this);
    settingsButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_settings_2_button.png")), QString("Settings"), this);
    logButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_log_button.png")), QString("Output"), this);
    testButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_test_button.png")), QString("Tests"), this);

    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);
    resetButton->setEnabled(true);
    runForButton->setEnabled(true);
    settingsButton->setEnabled(true);
    logButton->setEnabled(true);
    testButton->setEnabled(true);

    ui->mainToolBar->addAction(startButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(pauseButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(stopButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(resetButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(runForButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(settingsButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(logButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(testButton);

    //QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);

    QObject::connect(startButton, &QAction::triggered, this, &MainWindow::startTriggered);
    QObject::connect(pauseButton, &QAction::triggered, this, &MainWindow::pauseTriggered);
    QObject::connect(stopButton, &QAction::triggered, this, &MainWindow::escape);
    QObject::connect(resetButton, &QAction::triggered, this, &MainWindow::resetTriggered);
    QObject::connect(runForButton, &QAction::triggered, this, &MainWindow::runForTriggered);
    QObject::connect(settingsButton, &QAction::triggered, this, &MainWindow::settingsTriggered);
    QObject::connect(logButton, &QAction::triggered, this, &MainWindow::outputTriggered);
    QObject::connect(testButton, &QAction::triggered, this, &MainWindow::doTests);

    QObject::connect(ui->actionFitness_histogram, &QAction::triggered, this, &MainWindow::countPeaks);
    QObject::connect(ui->actionSave_settings, &QAction::triggered, this, &MainWindow::save);
    QObject::connect(ui->actionSave_settings_as, &QAction::triggered, this, &MainWindow::saveAs);
    QObject::connect(ui->actionLoad_settings_from_file, &QAction::triggered, this, &MainWindow::open);
    QObject::connect(ui->actionSet_uninformative_factor, &QAction::triggered, this, &MainWindow::setFactor);
    QObject::connect(ui->actionRun_tests, &QAction::triggered, this, &MainWindow::doTests);
    QObject::connect(ui->actionRestore_default_settings, &QAction::triggered, this, &MainWindow::defaultSettings);

    QObject::connect(ui->actionRandom_seed, &QAction::triggered, this, &MainWindow::setRandomSeed);

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escape()));
    new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_M), this, SLOT(setMultiplePlayingFields()));
    new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_X), this, SLOT(selectionHistogram()));

    QDir settingsPath;
    settingsPath.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    settingsFileString = (QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + QString(PRODUCTNAME) + "_settings.xml");

    ui->mainToolBar->addSeparator();
    QLabel *savePathLabel = new QLabel(" Save path: ", this);
    ui->mainToolBar->addWidget(savePathLabel);
    QString savePathString(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    savePathString.append("/");
    path = new QLineEdit(savePathString, this);
    connect(path, &QLineEdit::textChanged, this, &MainWindow::pathTextChanged);
    ui->mainToolBar->addWidget(path);

    //Spacer
    QWidget *empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    empty->setMaximumWidth(10);
    empty->setMaximumHeight(5);
    ui->mainToolBar->addWidget(empty);
    QPushButton *cpath = new QPushButton("&Change", this);
    ui->mainToolBar->addWidget(cpath);
    connect(cpath, &QPushButton::clicked, this, &MainWindow::changePathTriggered);

    ui->mainToolBar->addSeparator();
    aboutButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_about_button.png")), QString("About"), this);
    ui->mainToolBar->addAction(aboutButton);
    QObject::connect(aboutButton, &QAction::triggered, this, &MainWindow::aboutTriggered);

    //Formatting of table font then colour
    QFont fnt;
    fnt.setPointSize(10);
    ui->character_Display->setFont(fnt);

    //String display
    ui->treeDisplay->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    //Test text edit
    ui->testLog->document()->setDefaultStyleSheet("h1 {text-align: center;} h2 {text-align: left;} h3 {text-align: center;} p {text-align: left; font-size: small;} p.error {font color=#FF00FF;}");
    ui->testLog->setHtml("<h1>TREvoSim test log </h1><p>Below you can find the output of the TREvoSim tests. The long pieces of text - e.g. masks and playing fields - are output as MD5 checksums for space and clarity. If the text is the same, the checksum will be too. Any tests that fail will appear in bright green font with a test failed message at the front.</p>");
    ui->testLog->setVisible(false);

    //Titles
    QStringList labellist;
    for (int i = 0; i < simSettings->runForTaxa; i++)labellist << QString("Species " + QString::number(i));
    ui->character_Display->setVerticalHeaderLabels(labellist);

    //Set selectability
    ui->character_Display->setSelectionMode(QAbstractItemView::NoSelection);

    //Progress bar
    progress = new QProgressBar(ui->statusBar);
    ui->statusBar->addPermanentWidget(progress);
    progress->hide();

    //Now set variables
    escapePressed = false;
    pauseFlag = false;
    batchRunning = false;
    testMode = false;

    //Load settings if previously saved
    load();

    //Ensure grid is right size for loaded settings or defaults
    if (simSettings->runMode == RUN_MODE_TAXON) resizeGrid(simSettings->runForTaxa, simSettings->genomeSize);
    else resizeGrid(500, simSettings->genomeSize, 1);

    //Maximise window
    showMaximized();

    //Start runs at zero
    runs = 0;
}

MainWindow::~MainWindow()
{

    QDir settingsPath;
    settingsPath.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    settingsFileString = (QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + QString(PRODUCTNAME) + "_settings.xml");
    save();

    delete ui;
}

void MainWindow::setStatus(QString message)
{
    ui->statusBar->showMessage(message);
}

void MainWindow::addProgressBar(int min, int max)
{
    progress->setRange(min, max);
    progress->show();
}

void MainWindow::setProgressBar(int value)
{
    progress->setValue(value);
    qApp->processEvents();
}

void MainWindow::hideProgressBar()
{
    progress->hide();
}

void MainWindow::changePathTriggered()
{
    QString dirname = QFileDialog::getExistingDirectory(this, "Select directory in which files should be saved.");
    if (dirname.length() != 0)
    {
        dirname.append("/");
        setPath(dirname);
    }
}

void MainWindow::pathTextChanged(QString newPath)
{
    setPath(newPath);
}

void MainWindow::setPath(QString newPath)
{
    path->setText(newPath);
    simSettings->savePathDirectory = newPath;
}

void MainWindow::setTreeDisplay(QString treeString)
{
    ui->treeDisplay->setText(treeString);
    //resize as required
    int h = static_cast<int>(ui->treeDisplay->document()->size().height());
    ui->treeDisplay->setFixedHeight(h);
    ui->treeDisplay->updateGeometry();
}

void MainWindow::setRandomSeed()
{
    simSettings->randomSeed = !simSettings->randomSeed;
    if (simSettings->randomSeed)
    {
        setStatus("Simulation will initialise with random phenome");
        ui->actionRandom_seed->setChecked(true);
    }
    else
    {
        setStatus("Simulation will initialise with phenome near peak fitness");
        ui->actionRandom_seed->setChecked(false);
    }
}

void MainWindow::load()
{
    QFile settingsFile(settingsFileString);
    if (settingsFile.open(QIODevice::ReadOnly))
    {
        if (!simSettings->loadSettings(&settingsFile)) QMessageBox::warning(this, "Error", "There seems to have been an error reading in the XML file. Not all settings will have been loaded.");
        else setStatus("Loaded settings file");
        settingsFile.close();
    }
    else setStatus(QString(PRODUCTNAME) + " was unable to find a settings file to load, so will start with default values.");
    setPath(simSettings->savePathDirectory);
}

void MainWindow::open()
{
    settingsFileString = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (settingsFileString.length() < 3)
    {
        QMessageBox::warning(this, "Erk", "There seems to have been an error - no filename.");
        settingsFileString = (QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + QString(PRODUCTNAME) + "_settings.xml");
        return;
    }
    load();
    if (simSettings->runMode == RUN_MODE_TAXON) resizeGrid(simSettings->runForTaxa, simSettings->genomeSize);
    else resizeGrid(500, simSettings->genomeSize, 1);

    settingsFileString = (QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + QString(PRODUCTNAME) + "_settings.xml");
}

void MainWindow::saveAs()
{
    settingsFileString = QFileDialog::getSaveFileName(this, tr("Save File"));
    if (settingsFileString.length() < 3)
    {
        QMessageBox::warning(this, "Erk", "There seems to have been an error - no filename.");
        settingsFileString = (QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + QString(PRODUCTNAME) + "_settings.xml");
        return;
    }
    if (!settingsFileString.endsWith(".xml"))settingsFileString.append(".xml");
    save();
}

void MainWindow::save()
{
    if (settingsFileString.length() < 3)saveAs();
    QFile settingsFile(settingsFileString);
    if (!settingsFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Error!", "Error opening settings file to write to.");
        return;
    }

    simSettings->saveSettings(&settingsFile);
    settingsFile.close();

    setStatus("File saved");
}

void MainWindow::startTriggered()
{
    //First sort GUI
    startRunGUI();

    if ((simSettings->stripUninformative) && (simSettings->stripUninformativeFactorSettings != simSettings->printSettings()) && runs == 0)
        if (QMessageBox::question(this, "Hmmm",
                                  "It looks like you have not calculated the strip uninformative factor for these settings. Would you like to?<br /><br />If you have manually set the factor, you should select no here otherwise your chosen value will be overwritten.",
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) recalculateStripUniformativeFactor(true);

    bool error = false;
    //Create a new simulation object - sending it important settings.
    simulation theSimulation(runs, simSettings, &error, this);
    //Then set it running - send pointer to main window for GUI and access functions, and run number
    if (!error) theSimulation.run();

    //Clear table/gui
    finishRunGUI();
    resetTriggered();

    //Increment in case user is doing multiple runs
    runs++;
}

void MainWindow::escape()
{
    escapePressed = true;
}

void MainWindow::pauseTriggered()
{
    pauseFlag = !pauseFlag;
}


void MainWindow::resetDisplays()
{
    /******* Sort out displays *******/
// Clear table
    for (int i = 0; i < ui->character_Display->rowCount(); i++)
        for (int j = 0; j < ui->character_Display->columnCount(); j++)
        {
            QTableWidgetItem *item(ui->character_Display->item(i, j));
            item->setText(" ");
        }
}

void MainWindow::resetTriggered()
{
    /******* Sort out displays *******/
    // Clear table
    resetDisplays();
    if (simSettings->runMode == RUN_MODE_TAXON) resizeGrid(simSettings->runForTaxa, simSettings->genomeSize);
    else resizeGrid(500, simSettings->genomeSize, 1);

    //Clear tree string
    QString TNTstring(" ");
    setTreeDisplay(TNTstring);
    ui->statusBar->clearMessage();

    /******* And variables / gui if required - i.e. if hit by user. Don't really need this, but there as safety net *******/
    if (!batchRunning)
    {
        finishRunGUI();
        pauseFlag = false;
        escapePressed = false;
    }
}

void MainWindow::runForTriggered()
{
    /******** Batch mode - multiple runs *****/
    int runBatchFor = -1;
    //Use custom dialogue to allow word wrap
    batchDialog bDialogue(this, &runBatchFor);
    //Does not modify runBatchFor unless box is accepted
    bDialogue.exec();
    //Return if dialogue cancelled
    if (runBatchFor == -1) return;

    QString label =
        "It looks like you have not calculated the strip uninformative factor for these settings. Would you like to?<br /><br />If you have manually set the factor, you should select no here otherwise your chosen value will be overwritten.";
    if ((simSettings->stripUninformative) && (simSettings->stripUninformativeFactorSettings != simSettings->printSettings()))
        if (QMessageBox::question(this, "Hmmm", label, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
            recalculateStripUniformativeFactor(true);

    startRunGUI();
    batchRunning = true;

    QVector<int> runsList(runBatchFor);
    std::iota(runsList.begin(), runsList.end(), 0);

    bool errorStart = false;
    //Create a new simulation object - sending it important settings.
    simulation theSimulation(runsList[0], simSettings, &errorStart, this);
    //Then set it running - send pointer to main window for GUI and access functions, and run number
    if (!errorStart) errorStart = theSimulation.run();

    if (!errorStart)
    {
        bool stopRuns = escapePressed;
        label = "It looks like the simulation failed.\nWould you like continue?";
        if (!stopRuns)
            if (QMessageBox::question(this, "Error", label, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
                stopRuns = true;

        if (stopRuns)
        {
            batchRunning = false;
            resetTriggered();
            return;
        }
    }
    else runsList.removeFirst();

    resetTriggered();

    auto count = 0;
    do
    {
        //In a previous version RJG had used the progress bar in the status bar, but connecting the future watcher signals and slots provide very challenging
        //Hence now do this with a dialogue - first create a dialogue, then QFutureWatcher and connect signals and slots.
        QProgressDialog dialog;
        QFutureWatcher<void> futureWatcher;
        QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
        QObject::connect(&dialog, &QProgressDialog::canceled, &futureWatcher, &QFutureWatcher<void>::cancel);
        QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressRangeChanged, &dialog, &QProgressDialog::setRange);
        QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged,  &dialog, &QProgressDialog::setValue);

        //Set text
        if (count == 0) dialog.setLabelText(QString("Starting first pass of your remaining batch. Running %1 remaining simulations on %2 cores.").arg(runsList.length()).arg(QThread::idealThreadCount()));
        else dialog.setLabelText(QString("Starting pass %1 of your remaining batch. Running %2 simulations that failed in pass %3 on %4 cores.").arg(count + 1).arg(runsList.length()).arg(count).arg(
                                         QThread::idealThreadCount()));

        //Do the runs using QtConcurrent::filter which modified the sequence in place
        futureWatcher.setFuture(QtConcurrent::filter(runsList, [this](const int &run)
        {
            bool error = false;
            simulation theSimulation(run, simSettings, &error);
            if (error) return true;
            //The simulation returns false if it fails, whereas QtConcurrent::filter() retains an item if filterFunction returns true - hence the not
            return (!theSimulation.run());
        }));

        // Display the dialog and start the event loop.
        dialog.exec();
        if (futureWatcher.isCanceled()) batchRunning = false;
        count++;
    }
    //Run up to 50 times so this cannot get caught in an infinite loop
    while (runsList.count() > 0 && count < 50 && batchRunning == true);

    //Reset gui etc.
    batchRunning = false;
    resetTriggered();
}

void MainWindow::settingsTriggered()
{
    Settings *sdialogue = new Settings(this, simSettings);
    sdialogue->exec();

    resetTriggered();
    if (simSettings->runMode == RUN_MODE_TAXON) resizeGrid(simSettings->runForTaxa, simSettings->genomeSize);
    else resizeGrid(500, simSettings->genomeSize, 1);

    //if (sdialogue->recalculateStripUninformativeFactorOnClose) recalculateStripUniformativeFactor(false);

}

void MainWindow::resizeGrid(const int speciesNumber, const int genomeSize, const int hideFrom)
{
    int width = genomeSize;
    if (width > 128) width = 128;
    ui->character_Display->setColumnCount(width);
    ui->character_Display->setRowCount(speciesNumber);

    if (hideFrom > 0) for (int i = hideFrom; i < speciesNumber; i++) ui->character_Display->hideRow(i);
    else for (int i = hideFrom; i < speciesNumber; i++) ui->character_Display->showRow(i);

    for (int i = 0; i < speciesNumber; i++)
        for (int j = 0; j < width; j++)
            if (ui->character_Display->item(i, j) == nullptr) ui->character_Display->setItem(i, j, new QTableWidgetItem(" "));

    for (int i = 0; i < ui->character_Display->columnCount(); i++)ui->character_Display->setColumnWidth(i, 25);

    //Titles
    QStringList labellist;
    for (int i = 0; i < speciesNumber; i++)labellist << QString("Species " + QString::number(i));
    ui->character_Display->setVerticalHeaderLabels(labellist);

    qApp->processEvents();
}

void MainWindow::hideRow(const int rowNumber)
{
    ui->character_Display->hideRow(rowNumber);
}

void MainWindow::showRow(const int rowNumber)
{
    ui->character_Display->showRow(rowNumber);
}

int MainWindow::rowMax()
{
    return ui->character_Display->rowCount();
}

void MainWindow::outputTriggered()
{
    Output odialogue(this, simSettings);
    odialogue.exec();
}

void MainWindow::aboutTriggered()
{
    About adialogue(this, this->windowTitle());
    adialogue.exec();
}

void MainWindow::printBlank(int row)
{
    if (row > ui->character_Display->rowCount())return;
    for (int i = 0; i < ui->character_Display->columnCount(); i++)
    {
        QTableWidgetItem *item(ui->character_Display->item(row, i));
        item->setText("_");
        qApp->processEvents();
    }
}

void MainWindow::printGenome(const Organism *org, int row)
{
    for (int i = 0; i < ui->character_Display->columnCount(); i++)
    {
        QTableWidgetItem *item(ui->character_Display->item(row, i));
        if (org->genome[i] == false)item->setText("0");
        else item->setText("1");
        qApp->processEvents();
    }
}

void MainWindow::recalculateStripUniformativeFactor(bool running)
{
    bool tempStripUninformative = simSettings->stripUninformative;
    simSettings->stripUninformative = true;
    simSettings->stripUninformativeFactor = 1.0;

    double stripUninformativeFactorMean = 0.;

    startRunGUI();

    addProgressBar(0, 10);

    for (int i = 0; i < 10; i++)
    {
        setProgressBar(i);
        resetTriggered();

        bool error = false;
        //Start a simulation - last bool here tells it we are running the strip uninformative calculation, which is required for constructor
        simulation theSimulation(0, simSettings, &error, this, true);
        bool success = false;
        if (!error) success = theSimulation.run();

        if (success) stripUninformativeFactorMean += static_cast<double>(simSettings->genomeSize) / static_cast<double>(theSimulation.returninformativeCharacters());

        if (error || !success) setStatus("Error calculating strip uninformative");
    }

    //Divide by 9 here to add some extra given variability of strip ununformative factor
    simSettings->stripUninformativeFactor = (stripUninformativeFactorMean / 9.);
    if (simSettings->stripUninformativeFactor > 20.)simSettings->stripUninformativeFactor = 20.;
    if (simSettings->stripUninformativeFactor < 1.) simSettings->stripUninformativeFactor = 1.;

    hideProgressBar();

    //Reset gui etc.
    if (!running)finishRunGUI();

    simSettings->stripUninformative = tempStripUninformative;

    simSettings->stripUninformativeFactorSettings = simSettings->printSettings();

    resetTriggered();
    QString status = QString("Strip uninformative factor calculated as %1").arg(simSettings->stripUninformativeFactor);
    setStatus(status);
}

void MainWindow::startRunGUI()
{
    startButton->setEnabled(false);
    pauseButton->setEnabled(true);
    stopButton->setEnabled(true);
    resetButton->setEnabled(false);
    runForButton->setEnabled(false);
    settingsButton->setEnabled(false);
    logButton->setEnabled(false);
    testButton->setEnabled(false);
}

void MainWindow::finishRunGUI()
{
    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);
    resetButton->setEnabled(true);
    runForButton->setEnabled(true);
    settingsButton->setEnabled(true);
    logButton->setEnabled(true);
    testButton->setEnabled(true);
}

void MainWindow::selectionHistogram()
{
    ui->statusBar->clearMessage();

    addProgressBar(0, 10000);

    QVector <quint64> histogram;
    for (int i = 0; i < simSettings->playingfieldSize; i++) histogram.append(0);

    //Create histogram for the selection criteria currently used
    for (int i = 0; i < 1000000; i++)
    {
        int marker = 0;
        //Note that selectionCoinToss is a double - hence all the casting below
        while (static_cast<double>(QRandomGenerator::global()->generate()) > (static_cast<double>(QRandomGenerator::max()) / simSettings->selectionCoinToss))
            if (++marker >= simSettings->playingfieldSize) marker = 0;
        histogram[marker]++;
        if (i % 100 == 0)setProgressBar(i);
    }

    //RJG - Set up save directory
    QDir savePathDirectory(simSettings->savePathDirectory);
    if (!savePathDirectory.mkpath(QString(PRODUCTNAME) + "_output"))
    {
        QMessageBox::warning(this, "Error", "Can't create output directory. Permissions issue?");
        return;
    }
    else savePathDirectory.cd(QString(PRODUCTNAME) + "_output");

    QString histogramFileNameString = (QString(PRODUCTNAME) + "_selection_histogram.txt");
    histogramFileNameString.prepend(savePathDirectory.absolutePath() + QDir::separator());
    QFile histogramFile(histogramFileNameString);
    if (!histogramFile.open(QIODevice::Append | QIODevice::Text))QMessageBox::warning(this, "Error!", "Error opening curve file to write to.");
    QTextStream histogramTextStream(&histogramFile);

    histogramTextStream << QString(PRODUCTNAME) << " selection histogram for the settings: " << simSettings->printSettings() << "\n";

    histogramTextStream << "Each row shows the position in the playing field, then the number of times this position was selected over the course of 1000000 repeats.\n\n";
    histogramTextStream << "Position\tFrequency\n";
    for (int i = 0; i < histogram.length(); i++) histogramTextStream << i << "\t" << histogram[i] << "\n";
    histogramFile.close();

    hideProgressBar();
    setStatus("Selection histogram saved in output folder.");
}

void MainWindow::setMultiplePlayingFields()
{
    bool ok;
    int tempPlayingfieldNumber = QInputDialog::getInt(this, "Set playing field number...", "How many playing fields would you like?", simSettings->playingfieldNumber, 3, 100, 1, &ok);
    if (!ok)
    {
        return;
    }
    simSettings->playingfieldNumber = tempPlayingfieldNumber;
}

void MainWindow::setFactor()
{
    bool ok;

    double tempFactor = QInputDialog::getDouble(this, "Set factor...", "Required factor?", 1.0, 1.0, 2000.0, 2, &ok);
    if (!ok)
    {
        return;
    }
    else simSettings->stripUninformativeFactor = tempFactor;

}

void MainWindow::countPeaks()
{
    //Save settings to load at end
    save();
    startRunGUI();

    //Create a new simulation object - sending it important settings.
    bool error = false;

    int genomeSize = QInputDialog::getInt(this, "Fitness histogram...", "How many bits?", 32, 1, 64, 1, &error);
    if (!error) return;

    int repeats =  QInputDialog::getInt(this, "Fitness histogram...", "How many repeats?", 1, 1, 100000, 1, &error);
    if (!error) return;

    //RJG - resize grid otherwise the print function will make. No need to record original settings, as dealt with in main window
    simSettings->genomeSize = genomeSize;
    //Also need to reset fitness size so count peaks works when doing match peaks (masks are initialised to fitness size)
    simSettings->fitnessSize = genomeSize;
    simSettings->speciesSelectSize = genomeSize;
    //Don't want to create a rogue subfolder if writeRunningLog is enabled
    simSettings->writeRunningLog = false;
    resizeGrid(1, genomeSize);

    //Progress bar max value is 2^16 - scale to this
    quint16 pmax = static_cast<quint16>(-1);

    addProgressBar(0, pmax);

    for (int repeat = 0; repeat < repeats; repeat++)
    {
        bool simError = false;
        simulation theSimulation(0, simSettings, &simError, this);
        setStatus(QString("Repeat %1/%2").arg(repeat).arg(repeats));

        //Then set it running - send pointer to main window for GUI and access functions, and run number
        if (!simError)theSimulation.countPeaks(genomeSize, repeat);
    }

    //Load previous settings again
    load();
    if (simSettings->runMode == RUN_MODE_TAXON) resizeGrid(simSettings->runForTaxa, simSettings->genomeSize);
    else resizeGrid(1, simSettings->genomeSize);

    finishRunGUI();

    resetTriggered();

    hideProgressBar();
}


void MainWindow::doTests()
{
    if (testMode)
    {
        load();
        testMode = false;
        if (simSettings->runMode == RUN_MODE_TAXON) resizeGrid(simSettings->runForTaxa, simSettings->genomeSize);
        else resizeGrid(1, simSettings->genomeSize);
        ui->testLog->setHtml("<h1>TREvoSim test log </h1><p>Below you can find the output of the TREvoSim tests. The long pieces of text - e.g. masks and playing fields - are output as MD5 checksums for space and clarity. If the text is the same, the checksum will be too. Any tests that fail will appear in bright green font with a test failed message at the front.</p>");
        ui->testLog->setVisible(false);
        return;
    }

    testMode = true;
    ui->testLog->setVisible(true);

    //Save settings to load at end
    save();

    //Create this here so we can use QMap list of tests to populate drop downs
    testinternal testObject(this);

    int testCount = 19;
    int testStart = 0;

    QStringList items;
    items << tr("All");
    for (int i = 0; i < testCount; i++) items << QString::number(i) + " - " + testObject.testDescription(i);

    bool ok;
    QString item = QInputDialog::getItem(this, tr("TREvoSim tests"), tr("Which test?"), items, 0, false, &ok);

    setStatus("Starting tests");
    QProgressDialog progress("Doing tests...", "Cancel", 0, testCount, this);
    progress.setWindowModality(Qt::WindowModal);


    if (ok && !item.isEmpty())
    {
        if (item != "All")
        {
            testCount = item.toInt() + 1;
            testStart = item.toInt();
            progress.close();
        }
        else
        {
            progress.setValue(0);
            progress.show();
            qApp->processEvents();
        }
    }
    else
    {
        testMode = false;
        if (simSettings->runMode == RUN_MODE_TAXON) resizeGrid(simSettings->runForTaxa, simSettings->genomeSize);
        else resizeGrid(1, simSettings->genomeSize);
        ui->testLog->setVisible(false);
        setStatus("Tests cancelled");
        return;
    }

    QElapsedTimer timer;
    timer.start();

    for (int testNumber = testStart; testNumber < testCount; testNumber++)
    {
        if (item == "All")
        {
            progress.setValue(testNumber);
            if (progress.wasCanceled()) break;
            qApp->processEvents();
        }

        ui->testLog->setAlignment(Qt::AlignLeft);
        ui->testLog->append(QString("<h2>Test %1</h2>").arg(testNumber));

        QString testResult;
        if (testObject.callTest(testNumber, testResult))
        {
            testResult.prepend("<p>");
            testResult.append("</p>");
            //Replace new lines with breaks - do it here because this makes use of a load of print functions that return \n
            testResult.replace("\n", "<br />");
            ui->testLog->append(testResult);
        }
        else
        {
            testResult.prepend("<p><font color=\"Lime\">********Test Failed ********\n");
            testResult.append("</font></p>");
            testResult.replace("\n", "<br />");
            ui->testLog->append(testResult);
        }

        ui->testLog->setAlignment(Qt::AlignCenter);
        ui->testLog->append("<p>*****************************************</p>");

    }

    if (item != "All") progress.close();

    setStatus(QString("%1 tests completed in %2 seconds.").arg(testCount - testStart).arg(timer.elapsed() / 1000));
}

void MainWindow::defaultSettings()
{
    delete simSettings;
    simSettings = new simulationVariables;
    if (simSettings->runMode == RUN_MODE_TAXON) resizeGrid(simSettings->runForTaxa, simSettings->genomeSize);
    else resizeGrid(1, simSettings->genomeSize);
    setStatus("Returned settings to defaults");
    path->setText(simSettings->savePathDirectory);
}

