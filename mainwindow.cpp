#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "organism.h"
#include "version.h"
#include "simulation.h"
#include "settings.h"
#include "test.h"

#include <QProgressDialog>

/*********** TREvoSim structure - overview ***********/
/*
 * There are four structures/classes that interact in TREvoSim, namely:
 * -- Organisms - each organism has a genome, a record of its parents genome, fitness, and can have a stochastic layer. Operators > and = are implemented.
 * -- Simulation - this object has a run and fitness histogram function, local copies of important settings for the run (which change sometimes during a run), and lots of private functions to aid the simulation such as e.g. fitness
 * -- Simulation variables - this object contains the variables of the simulation. It is sent as a const to the simulation which makes local copies, and modified by the output and simulation classes. It also has functions to load, save and print setings
 * -- MainWindow - This handles the GUI and interacts with the other objects allowing user input etc.
 *
 * Simulation.run is sent a pointer to Maindwindow and uses this to communicate with the window via access functions.
 * Mainwindow has a bunch of flags such as whether a batch is running which the simulation can access.
 *
 *
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
    QString version;
    version.sprintf("%d.%d.%d", MAJORVERSION, MINORVERSION, PATCHVERSION);
    setWindowTitle(QString(PRODUCTNAME) + " v" + version + " - compiled - " + __DATE__);

    //Create default settings object
    simSettings = new simulationVariables;

    //Set up toolbar...
    ui->mainToolBar->setIconSize(QSize(25, 25));
    startButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_button_green.png")), QString("Run"), this);
    pauseButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_pause_button_orange.png")), QString("Pause"), this);
    resetButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_reset_button_red.png")), QString("Reset"), this);
    runForButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_n_button_green.png")), QString("Batch..."), this);
    settingsButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_settings_2_button.png")), QString("Settings"), this);
    logButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_log_button.png")), QString("Output"), this);
    testButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_test_button.png")), QString("Tests"), this);

    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    resetButton->setEnabled(true);
    runForButton->setEnabled(true);
    settingsButton->setEnabled(true);
    logButton->setEnabled(true);
    testButton->setEnabled(true);

    ui->mainToolBar->addAction(startButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(pauseButton);
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

    QObject::connect(startButton, SIGNAL(triggered()), this, SLOT(startTriggered()));
    QObject::connect(pauseButton, SIGNAL(triggered()), this, SLOT(pauseTriggered()));
    QObject::connect(resetButton, SIGNAL(triggered()), this, SLOT(resetTriggered()));
    QObject::connect(runForButton, SIGNAL(triggered()), this, SLOT(runForTriggered()));
    QObject::connect(settingsButton, SIGNAL(triggered()), this, SLOT(settingsTriggered()));
    QObject::connect(logButton, SIGNAL(triggered()), this, SLOT(outputTriggered()));
    QObject::connect(testButton, SIGNAL(triggered()), this, SLOT(doTests()));

    QObject::connect(ui->actionFitness_histogram, SIGNAL(triggered()), this, SLOT(countPeaks()));
    QObject::connect(ui->actionSave_current_settings, SIGNAL(triggered()), this, SLOT(save()));
    QObject::connect(ui->actionSave_settings_as, SIGNAL(triggered()), this, SLOT(saveAs()));
    QObject::connect(ui->actionLoad_settings_from_file, SIGNAL(triggered()), this, SLOT(open()));
    QObject::connect(ui->actionSet_uninformative_factor, SIGNAL(triggered()), this, SLOT(setFactor()));
    QObject::connect(ui->actionRun_tests, SIGNAL(triggered()), this, SLOT(doTests()));
    QObject::connect(ui->actionRestore_default_settings, SIGNAL(triggered()), this, SLOT(defaultSettings()));

    QObject::connect(ui->actionRandom_seed, SIGNAL(triggered()), this, SLOT(setRandomSeed()));

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escape()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_M), this, SLOT(setMultiplePlayingFields()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_X), this, SLOT(selectionHistogram()));

    QDir settingsPath;
    settingsPath.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    settingsFileString = (QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + QString(PRODUCTNAME) + "_settings.xml");

    ui->mainToolBar->addSeparator();
    QLabel *savePathLabel = new QLabel(" Save path: ", this);
    ui->mainToolBar->addWidget(savePathLabel);
    QString savePathString(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    savePathString.append("/");
    path = new QLineEdit(savePathString, this);
    ui->mainToolBar->addWidget(path);

    //Spacer
    QWidget *empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    empty->setMaximumWidth(10);
    empty->setMaximumHeight(5);
    ui->mainToolBar->addWidget(empty);
    QPushButton *cpath = new QPushButton("&Change", this);
    ui->mainToolBar->addWidget(cpath);
    connect(cpath, SIGNAL (clicked()), this, SLOT (changePathTriggered()));

    ui->mainToolBar->addSeparator();
    aboutButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_about_button.png")), QString("About"), this);
    ui->mainToolBar->addAction(aboutButton);
    QObject::connect(aboutButton, SIGNAL (triggered()), this, SLOT (aboutTriggered()));

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
    for (int i = 0; i < simSettings->taxonNumber; i++)labellist << QString("Species " + QString::number(i));
    ui->character_Display->setVerticalHeaderLabels(labellist);

    //Populate table, and then modify these items as required when needed - think this is  better as less faffing with memory that creating and deleting new ones
    for (int i = 0; i < simSettings->taxonNumber; i++)
        for (int j = 0; j < simSettings->genomeSize; j++)
            ui->character_Display->setItem(i, j, new QTableWidgetItem(" "));

    //Qt 5.12 seems to resize the columns on compile on some operating systems. This resize ensures they appear the same across all operating systems
    for (int i = 0; i < ui->character_Display->columnCount(); i++)ui->character_Display->setColumnWidth(i, 25);
    ui->character_Display->setCurrentItem(nullptr);
    ui->character_Display->setSelectionMode(QAbstractItemView::NoSelection);

    //Progress bar
    progress = new QProgressBar(ui->statusBar);
    ui->statusBar->addPermanentWidget(progress);
    progress->hide();

    //Now set variables
    escapePressed = false;
    pauseFlag = false;
    batchRunning = false;
    calculateStripUninformativeFactorRunning = false;
    batchError = false;
    unresolvableBatch = false;
    testMode = false;

    //Load settings if previously saved
    load();

    //Ensure grid is right size for loaded settings or defaults
    resizeGrid();

    //Maximise window
    showMaximized();
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

QString MainWindow::getPath()
{
    return path->text();
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
    resizeGrid();
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

void MainWindow::changePathTriggered()
{
    QString dirname = QFileDialog::getExistingDirectory(this, "Select directory in which files should be saved.");
    if (dirname.length() != 0)
    {
        dirname.append("/");
        setPath(dirname);
    }
}

void MainWindow::startTriggered()
{
    //First sort GUI
    startRunGUI();

    if (simSettings->stripUninformative && (simSettings->stripUninformativeFactorSettings != simSettings->printSettings()))
        if (QMessageBox::question(this, "Hmmm",
                                  "It looks like you have not calculated the strip uninformative factor for these settings. Would you like to?<br /><br />If you have manually set the factor, you should select no here otherwise your chosen value will be overwritten.",
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)recalculateStripUniformativeFactor(true);

    //Create a new simulation object - sending it important settings.
    simulation theSimulation(this, 0, simSettings);
    //Then set it running - send pointer to main window for GUI and access functions, and run number
    theSimulation.run();

    //Clear table/gui
    finishRunGUI();
    resetTriggered();
}

void MainWindow::escape()
{
    escapePressed = true;
}

void MainWindow::pauseTriggered()
{
    pauseFlag = !pauseFlag;
}

void MainWindow::resetTriggered()
{
    /******* Sort out displays *******/
    // Clear table
    for (int i = 0; i < ui->character_Display->rowCount(); i++)
        for (int j = 0; j < ui->character_Display->columnCount(); j++)
        {
            QTableWidgetItem *item(ui->character_Display->item(i, j));
            item->setText(" ");
        }
    //Clear tree string
    QString TNTstring(" ");
    setTreeDisplay(TNTstring);
    ui->statusBar->clearMessage();

    /******* And variables / gui if required - i.e. if hit by user. Don't really need this, but there as safety net *******/
    if (!batchRunning && !calculateStripUninformativeFactorRunning)
    {
        finishRunGUI();
        pauseFlag = false;
        escapePressed = false;
    }
}

void MainWindow::runForTriggered()
{

    /******** Batch mode - multiple runs *****/

    bool ok;
    int runBatchFor = QInputDialog::getInt(this, "Batch...", "How many runs?", 25, 1, 10000, 1, &ok);
    if (!ok)return;

    startRunGUI();
    batchRunning = true;
    batchError = false;
    unresolvableBatch = false;

    if (simSettings->stripUninformative && (simSettings->stripUninformativeFactorSettings != simSettings->printSettings()))
        if (QMessageBox::question(this, "Hmmm",
                                  "It looks like you have not calculated the strip uninformative factor for these settings. Would you like to?<br /><br />If you have manually set the factor, you should select no here otherwise your chosen value will be overwritten.",
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)recalculateStripUniformativeFactor(true);



    //Sort out headers for cumulative species file - no need to repeat these every time, in contrast to batch analysis files
    if (simSettings->speciesCurve)
    {

        QString speciesCurveFilenameString = (QString(PRODUCTNAME) + "_species_curve.txt");

        if (!simSettings->append || !batchRunning )
        {
            speciesCurveFilenameString.append(QString("%1").arg(runs, 3, 10, QChar('0')));
            speciesCurveFilenameString.append(".txt");
        }
        else
        {
            //speciesCurveFilenameString.append(QString("batch_env_%1_masks_%2").arg(environmentNumber).arg(maskNumber));
            speciesCurveFilenameString.append(QString("batch"));
            speciesCurveFilenameString.append(".txt");
        }

        //RJG - Set up save directory
        QDir savePathDirectory(path->text());
        if (!savePathDirectory.mkpath(QString(PRODUCTNAME) + "_output"))
        {
            QMessageBox::warning(this, "Error", "Cant save output files. Permissions issue?");
            return;
        }
        else savePathDirectory.cd(QString(PRODUCTNAME) + "_output");
        speciesCurveFilenameString.prepend(savePathDirectory.absolutePath() + QDir::separator());

        QFile speciesCurveFile(speciesCurveFilenameString);

        bool errorFlagSpecies = false;

        if (!simSettings->append)
        {
            if (!speciesCurveFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox::warning(this, "Error!", "Error opening curve file to write to.");
                errorFlagSpecies = true;
            }
        }
        else
        {
            if (!speciesCurveFile.open(QIODevice::Append | QIODevice::Text))
            {
                QMessageBox::warning(this, "Error!", "Error opening curve file to write to.");
                errorFlagSpecies = true;
            }
        }

        if (errorFlagSpecies)
        {
            finishRunGUI();
            return;
        }

        QTextStream speciesCurveTextStream(&speciesCurveFile);

        speciesCurveTextStream << (QString(PRODUCTNAME) + "_") << simSettings->printSettings() << "\n";
        speciesCurveTextStream << "First row gives species ID, subsequent rows are iterations at which that species originates in each run:\n";
        for (int i = 0; i < simSettings->taxonNumber; i++)speciesCurveTextStream << i << "\t";
        speciesCurveTextStream << "\n";
        speciesCurveFile.close();
    }

    int runs = 0;

    addProgressBar(0, runBatchFor);

    do
    {
        setProgressBar(runs);
        simulation theSimulation(this, runs, simSettings);
        theSimulation.run();
        resetTriggered();
        if (!batchError && !unresolvableBatch)runs++;
    }
    while (runs < runBatchFor && !escapePressed);

    //Reset gui etc.
    batchRunning = false;
    batchError = false;
    unresolvableBatch = false;
    hideProgressBar();
    resetTriggered();
}

void MainWindow::settingsTriggered()
{
    Settings *sdialogue = new Settings(this, simSettings);
    sdialogue->exec();

    if (sdialogue->resizeGrid)
    {
        resetTriggered();
        resizeGrid();
    }

    if (sdialogue->recalculateStripUninformativeFactorOnClose) recalculateStripUniformativeFactor(false);

}

void MainWindow::resizeGrid()
{
    int width = simSettings->genomeSize;
    if (width > 128)width = 128;
    ui->character_Display->setColumnCount(width);
    ui->character_Display->setRowCount(simSettings->taxonNumber);

    //Populate table, and then modify these items as required when needed - think this is  better as less faffing with memory that creating and deleting new ones
    for (int i = 0; i < simSettings->taxonNumber; i++)
        for (int j = 0; j < simSettings->genomeSize; j++)
            if (ui->character_Display->item(i, j) == nullptr)ui->character_Display->setItem(i, j, new QTableWidgetItem(" "));

    for (int i = 0; i < ui->character_Display->columnCount(); i++)ui->character_Display->setColumnWidth(i, 25);

    //Titles
    QStringList labellist;
    for (int i = 0; i < simSettings->taxonNumber; i++)labellist << QString("Species " + QString::number(i));
    ui->character_Display->setVerticalHeaderLabels(labellist);

    qApp->processEvents();
}


void MainWindow::resizeGrid(const simulationVariables &simSettings)
{

    resetTriggered();

    int width = simSettings.genomeSize;
    if (width > 128)width = 128;
    ui->character_Display->setColumnCount(width);
    ui->character_Display->setRowCount(simSettings.taxonNumber);

    //Populate table, and then modify these items as required when needed - think this is  better as less faffing with memory that creating and deleting new ones
    for (int i = 0; i < simSettings.taxonNumber; i++)
        for (int j = 0; j < simSettings.genomeSize; j++)
            if (ui->character_Display->item(i, j) == nullptr)ui->character_Display->setItem(i, j, new QTableWidgetItem(" "));

    for (int i = 0; i < ui->character_Display->columnCount(); i++)ui->character_Display->setColumnWidth(i, 25);

    //Titles
    QStringList labellist;
    for (int i = 0; i < simSettings.taxonNumber; i++)labellist << QString("Species " + QString::number(i));
    ui->character_Display->setVerticalHeaderLabels(labellist);

    qApp->processEvents();
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
    calculateStripUninformativeFactorRunning = true;
    simSettings->stripUninformative = true;
    simSettings->stripUninformativeFactor = 1.0;

    double stripUninformativeFactorMean = 0.;

    startRunGUI();

    addProgressBar(0, 10);

    for (int i = 0; i < 10; i++)
    {
        setProgressBar(i);
        resetTriggered();

        //qDebug() << stripUninformativeFactorMean;

        simulation theSimulation(this, 0, simSettings);
        theSimulation.run();

        stripUninformativeFactorMean += static_cast<double>(simSettings->genomeSize) / static_cast<double>(theSimulation.returninformativeCharacters());
    }

    //Divide by 9 here to add some extra given variability of strip ununformative factor
    simSettings->stripUninformativeFactor = (stripUninformativeFactorMean / 9.);
    if (simSettings->stripUninformativeFactor > 20.)simSettings->stripUninformativeFactor = 20.;

    hideProgressBar();

    //Reset gui etc.
    if (!running)finishRunGUI();

    calculateStripUninformativeFactorRunning = false;
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
    QDir savePathDirectory(path->text());
    if (!savePathDirectory.mkpath(QString(PRODUCTNAME) + "_output"))
    {
        QMessageBox::warning(this, "Error", "Cant save output file. Permissions issue?");
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

    //Create a new simulation object - sending it important settings.
    simulation theSimulation(this, 0, simSettings);
    //Then set it running - send pointer to main window for GUI and access functions, and run number
    theSimulation.countPeaks(this);

    //Load previous settings again
    load();
    resizeGrid();
    resetTriggered();
}


void MainWindow::doTests()
{
    if (testMode)
    {
        load();
        testMode = false;
        resizeGrid();
        ui->testLog->setHtml("<h1>TREvoSim test log </h1><p>Below you can find the output of the TREvoSim tests. The long pieces of text - e.g. masks and playing fields - are output as MD5 checksums for space and clarity. If the text is the same, the checksum will be too. Any tests that fail will appear in bright green font with a test failed message at the front.</p>");
        ui->testLog->setVisible(false);
        return;
    }

    testMode = true;
    ui->testLog->setVisible(true);

    //Save settings to load at end
    save();

    int testCount = 17;
    int testStart = 0;

    QStringList items;
    items << tr("All");
    for (int i = 1; i < testCount; i++) items << QString::number(i);

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
        resizeGrid();
        ui->testLog->setVisible(false);
        setStatus("Tests cancelled");
        return;
    }

    test testObject(this);
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

    setStatus("Tests done.");
}

void MainWindow::defaultSettings()
{
    delete simSettings;
    simSettings = new simulationVariables;
    resizeGrid();
    setStatus("Returned settings to defaults");
    path->setText(simSettings->savePathDirectory);
}

