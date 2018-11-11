#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Global define
#define MAX_RAND 65535
#define STRIP_UNINFORM_FACTOR 12

#include "randoms.h"
#include "organism.h"
#include "settings.h"
#include "output.h"
#include "about.h"
#include "simulation_variables.h"

#include <algorithm>

#include <QMainWindow>
#include <QActionGroup>
#include <QAction>
#include <QTableWidget>
#include <QStringList>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QAbstractScrollArea>
#include <QFileDialog>
#include <QInputDialog>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QTextCodec>
#include <QTimer>
#include <QtMath>
#include <QProgressBar>
#include <QShortcut>
#include <QStandardPaths>

//Forward declaration to avoid circular dependencies in print_genome declaration
class organism;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


protected:
    Ui::MainWindow *ui;

private:
     QAction *startButton, *pauseButton, *resetButton, *runForButton, *settingsButton, *logButton, *aboutButton;
     QTableWidget *table;
     QLineEdit *path;
     int runs;

     //Simulation calculations
     int fitness(const organism *org, const QVector<QVector<bool> > &masks);
     int genome_diff(const organism *org1, const organism *org2, int run_genome_size);
     int parent_genome_diff(const organism *org1, int run_genome_size);
     void recalculate_strip_uniformative_factor();
     void gui_finish_run();
     void gui_start_run();
     void clear_vectors(QVector<organism *> &playing_field, QVector <organism*> &species_list, QVector<QVector<bool> > &masks);

     //Display
     void print_genome(const organism *org, int row);
     void print_blank(int row);
     void resize_grid();

     //Print to string for files
     QString print_newick(int species, QVector<organism *> &species_list);
     QString print_newick_bl(int species, QVector<organism *> &species_list, bool phangorn_tree);
     QString print_Time();
     QString print_Settings();
     QString print_matrix(const QVector<organism *> species_list, int run_genome_size);
     QString print_genome_string(const organism *org, int genome_size_local);
     QString print_genome_int(int genome_local, int genome_size_local, const quint64 *lookups_local);

 private slots:
    void start_triggered();
    void pause_triggered();
    void reset_triggered();
    void runfor_triggered();
    void changepath_triggered();
    void settings_triggered();
    void output_triggered();
    void about_triggered();
    void load();
    void save();
    void save_as();
    void open();
    void escape();

    //Analysis
    void count_peaks();
};

extern MainWindow *MainWin;

#endif // MAINWINDOW_H
