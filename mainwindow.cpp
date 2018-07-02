#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "organism.h"
#include "version.h"

MainWindow *MainWin;
randoms *simulation_randoms;

/***********
 *
 * Thanks for checking out the TREvoSim source code. The software was written by Russell Garwood, athough it employs a GUI designed by Alan Spencer.
 * Many of the underlying concepts are shared with REvoSim, which was coded by Mark Sutton, Alan Spencer and Russell Garwood.
 *
 * I hope the software is useful to you: if there are modifications you would like to suggest to use this in your work, please feel free to email me:
 *
 * russell.garwood@gmail.com
 *
 * and I'll happily look into these.
 *
 ***********/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowIcon(QIcon ("://resources/icon.png"));
    ui->setupUi(this);
    MainWin=this;
    QString version;
    version.sprintf("%d.%d.%d",MAJORVERSION,MINORVERSION,PATCHVERSION);
    setWindowTitle(QString(PRODUCTNAME)+" v"+version+" - compiled - "+__DATE__);

    //Set global(ish) variables for simulation
    genome_size=128;
    taxon_number=32;
    pfield_size=12;
    species_difference=5;
    environment_mutation=1.0;
    organism_mutation=1.0;
    runs=0;
    unresolvable_cutoff=5;
    mask_number=3;
    strip_uninformative_factor=-1.;
    informative_characters=0;

    batch_running=false;
    error_batch=false;
    unresolvable_batch =false;
    pause_flag=false;
    strip_uninformative=false;
    sansomian=true;
    no_deleterious=false;
    calc_factor_running=false;
    factor_settings_batch=false;
    escape_pressed=false;
    work_log=false;

    factor_settings="UNSET";

    //And for file saving etc.
    append=true;
    write_tree=true;
    QString base = (QString(PRODUCTNAME)+"_");
    base_01=base;
    base_02=base;
    base_03=base;
    base_03.append("tree_");
    ext_01=".tnt";
    ext_02=".nex";
    ext_03=".nex";
    filestring_01="Please enter output text (options in output dialogue).";
    filestring_02="Please enter output test (options in output dialogue).";
    filestring_03="#NEXUS\n\n[ ||Time|| ||Settings|| ] \n\n Begin trees;\nTranslate\n";

    //Set up toolbar...
    ui->mainToolBar->setIconSize(QSize(25,25));
    startButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_button_green.png")), QString("Run"), this);
    pauseButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_pause_button_orange.png")), QString("Pause"), this);
    resetButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_reset_button_red.png")), QString("Reset"), this);
    runForButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_n_button_green.png")), QString("Batch..."), this);
    settingsButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_settings_2_button.png")), QString("Settings"), this);
    logButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_log_button.png")), QString("Output"), this);

    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    resetButton->setEnabled(true);
    runForButton->setEnabled(true);
    settingsButton->setEnabled(true);
    logButton->setEnabled(true);

    ui->mainToolBar->addAction(startButton);ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(pauseButton);ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(resetButton);ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(runForButton);ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(settingsButton);ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(logButton);

    QObject::connect(startButton, SIGNAL(triggered()), this, SLOT(start_triggered()));
    QObject::connect(pauseButton, SIGNAL(triggered()), this, SLOT(pause_triggered()));
    QObject::connect(resetButton, SIGNAL(triggered()), this, SLOT(reset_triggered()));
    QObject::connect(runForButton, SIGNAL(triggered()), this, SLOT(runfor_triggered()));
    QObject::connect(settingsButton, SIGNAL(triggered()), this, SLOT(settings_triggered()));
    QObject::connect(logButton, SIGNAL(triggered()), this, SLOT(output_triggered()));

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escape()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(save()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S), this, SLOT(save_as()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this, SLOT(open()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_P), this, SLOT(count_peaks()));

    settings_filename=(QString(PRODUCTNAME)+"_settings.xml");

    ui->mainToolBar->addSeparator();
    QLabel *spath = new QLabel("Save path: ", this);
    ui->mainToolBar->addWidget(spath);
    QString program_path(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    program_path.append("/");
    path = new QLineEdit(program_path,this);
    ui->mainToolBar->addWidget(path);

    //Spacer
    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    empty->setMaximumWidth(10);
    empty->setMaximumHeight(5);
    ui->mainToolBar->addWidget(empty);
    QPushButton *cpath = new QPushButton("&Change", this);
    ui->mainToolBar->addWidget(cpath);
    connect(cpath, SIGNAL (clicked()), this, SLOT (changepath_triggered()));

    ui->mainToolBar->addSeparator();
    aboutButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_about_button.png")), QString("About"), this);
    ui->mainToolBar->addAction(aboutButton);
    QObject::connect(aboutButton, SIGNAL (triggered()), this, SLOT (about_triggered()));
    //ui->mainToolBar->addSeparator();

    //Formatting of table font then colour
    QFont fnt;
    fnt.setPointSize(10);
    fnt.setFamily("Arial");
    ui->character_Display->setFont(fnt);
    QPalette table_palette;
    table_palette.setColor(QPalette::Text, Qt::black);
    ui->character_Display->setPalette(table_palette);

    //String display
     ui->tree_Display->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    //Titles
    QStringList labellist;
    for(int i=0;i<taxon_number;i++)labellist<<QString("Species " + QString::number(i));
    ui->character_Display->setVerticalHeaderLabels(labellist);

    //Populate table, and then modify these items as required when needed - think this is  better as less faffing with memory that creating and deleting new ones
    for (int i=0;i<taxon_number;i++)
        for(int j=0;j<genome_size;j++)
                ui->character_Display->setItem(i, j, new QTableWidgetItem(" "));

    //Load random numbers
    simulation_randoms = new randoms();

    //Load settings if previously saved
    load();

    //Ensure grid is right size for loaded settings or defaults
    resize_grid();

    //Maximise window
    showMaximized();
}

MainWindow::~MainWindow()
{

    save();

    delete simulation_randoms;
    delete ui;
}

void MainWindow::load()
{
    QFile settings_file(settings_filename);
    if(settings_file.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader settings_file_in(&settings_file);

        while (!settings_file_in.atEnd()&& !settings_file_in.hasError())
            {

             /* Read next element.*/
             QXmlStreamReader::TokenType token = settings_file_in.readNext();
             /* If token is just StartDocument, we'll go to next.*/

             if(token == QXmlStreamReader::StartDocument)continue;
             if(token == QXmlStreamReader::StartElement)
                 {
                    //Ints
                    if(settings_file_in.name() == "Tree_Sim")continue;
                    if(settings_file_in.name() == "genome_size")genome_size=settings_file_in.readElementText().toInt();
                    if(settings_file_in.name() == "taxon_number")taxon_number=settings_file_in.readElementText().toInt();
                    if(settings_file_in.name() == "pfield_size")pfield_size=settings_file_in.readElementText().toInt();
                    if(settings_file_in.name() == "species_difference")species_difference=settings_file_in.readElementText().toInt();
                    if(settings_file_in.name() == "unresolvable_cutoff")unresolvable_cutoff=settings_file_in.readElementText().toInt();
                    if(settings_file_in.name() == "mask_number")mask_number=settings_file_in.readElementText().toInt();

                    //Double
                    if(settings_file_in.name() == "strip_uninformative_factor")strip_uninformative_factor=settings_file_in.readElementText().toDouble();
                    if(settings_file_in.name() == "environment_mutation")environment_mutation=settings_file_in.readElementText().toDouble();
                    if(settings_file_in.name() == "organism_mutation")organism_mutation=settings_file_in.readElementText().toDouble();

                    //Bools
                    if(settings_file_in.name() == "strip_uninformative")strip_uninformative=settings_file_in.readElementText().toInt();
                    if(settings_file_in.name() == "append")append=settings_file_in.readElementText().toInt();
                    if(settings_file_in.name() == "write_tree")write_tree=settings_file_in.readElementText().toInt();
                    if(settings_file_in.name() == "sansomian")sansomian=settings_file_in.readElementText().toInt();
                    if(settings_file_in.name() == "no_deleterious")no_deleterious=settings_file_in.readElementText().toInt();

                    //Strings
                    if(settings_file_in.name() == "base_01")base_01=settings_file_in.readElementText();
                    if(settings_file_in.name() == "base_02")base_02=settings_file_in.readElementText();
                    if(settings_file_in.name() == "ext_01")ext_01=settings_file_in.readElementText();
                    if(settings_file_in.name() == "ext_02")ext_02=settings_file_in.readElementText();
                    if(settings_file_in.name() == "filestring_01")filestring_01=settings_file_in.readElementText();
                    if(settings_file_in.name() == "filestring_02")filestring_02=settings_file_in.readElementText();
                    if(settings_file_in.name() == "save_path")path->setText(settings_file_in.readElementText());
                    if(settings_file_in.name() == "factor_settings")factor_settings=settings_file_in.readElementText();
                  }
            }
        // Error
        if(settings_file_in.hasError()) QMessageBox::warning(0,"Erk","There seems to have been an error reading in the XML file. Not all settings will have been loaded.");
        settings_file.close();
        ui->statusBar->showMessage("Loaded settings file");
    }
    else ui->statusBar->showMessage(QString(PRODUCTNAME)+" was unable to find a settings file to load, so will start with default values.");
}

void MainWindow::open()
{
    settings_filename=QFileDialog::getOpenFileName(this, tr("Open File"));
    if(settings_filename.length()<3)
            {
            QMessageBox::warning(0,"Erk","There seems to have been an error - no filename.");
            settings_filename=(QString(PRODUCTNAME)+"_settings.xml");
            return;
            }
    load();
    resize_grid();
    settings_filename=(QString(PRODUCTNAME)+"_settings.xml");

}

void MainWindow::save_as()
{
     settings_filename=QFileDialog::getSaveFileName(this, tr("Save File"));
     if(settings_filename.length()<3)
             {
             QMessageBox::warning(0,"Erk","There seems to have been an error - no filename.");
             settings_filename=(QString(PRODUCTNAME)+"_settings.xml");
             return;
             }
     if(!settings_filename.endsWith(".xml"))settings_filename.append(".xml");
     save();
     settings_filename=(QString(PRODUCTNAME)+"_settings.xml");
}

void MainWindow::save()
{
    QFile settings_file(settings_filename);
    if(!settings_file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::warning(0, "Error!", "Error opening settings file to write to.");
        return;
    }

    QXmlStreamWriter settings_file_out(&settings_file);
    settings_file_out.setAutoFormatting(true);
    settings_file_out.setAutoFormattingIndent(-2);

    settings_file_out.writeStartDocument();

    settings_file_out.writeStartElement("Tree_Sim");

    //Ints
    settings_file_out.writeStartElement("genome_size");
    settings_file_out.writeCharacters(QString("%1").arg(genome_size));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("taxon_number");
    settings_file_out.writeCharacters(QString("%1").arg(taxon_number));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("pfield_size");
    settings_file_out.writeCharacters(QString("%1").arg(pfield_size));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("species_difference");
    settings_file_out.writeCharacters(QString("%1").arg(species_difference));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("unresolvable_cutoff");
    settings_file_out.writeCharacters(QString("%1").arg(unresolvable_cutoff));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("mask_number");
    settings_file_out.writeCharacters(QString("%1").arg(mask_number));
    settings_file_out.writeEndElement();

    //Double
    settings_file_out.writeStartElement("strip_uninformative_factor");
    settings_file_out.writeCharacters(QString("%1").arg(strip_uninformative_factor));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("environment_mutation");
    settings_file_out.writeCharacters(QString("%1").arg(environment_mutation));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("organism_mutation");
    settings_file_out.writeCharacters(QString("%1").arg(organism_mutation));
    settings_file_out.writeEndElement();

    //Bools
    settings_file_out.writeStartElement("strip_uninformative");
    settings_file_out.writeCharacters(QString("%1").arg(strip_uninformative));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("append");
    settings_file_out.writeCharacters(QString("%1").arg(append));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("write_tree");
    settings_file_out.writeCharacters(QString("%1").arg(write_tree));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("sansomian");
    settings_file_out.writeCharacters(QString("%1").arg(sansomian));
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("no_deleterious");
    settings_file_out.writeCharacters(QString("%1").arg(no_deleterious));
    settings_file_out.writeEndElement();

    //Strings
    settings_file_out.writeStartElement("base_01");
    settings_file_out.writeCharacters(base_01);
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("base_02");
    settings_file_out.writeCharacters(base_02);
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("ext_01");
    settings_file_out.writeCharacters(ext_01);
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("ext_02");
    settings_file_out.writeCharacters(ext_02);
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("filestring_01");
    settings_file_out.writeCharacters(filestring_01);
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("filestring_02");
    settings_file_out.writeCharacters(filestring_02);
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("save_path");
    settings_file_out.writeCharacters(path->text());
    settings_file_out.writeEndElement();

    settings_file_out.writeStartElement("factor_settings");
    settings_file_out.writeCharacters(factor_settings);
    settings_file_out.writeEndElement();

    settings_file_out.writeEndElement();

    settings_file_out.writeEndDocument();

    settings_file.close();

    ui->statusBar->showMessage("File saved");
}

void MainWindow::changepath_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(this,"Select directory in which files should be saved.");
    if (dirname.length()!=0)
    {
        dirname.append("/");
        path->setText(dirname);
    }

}

void MainWindow::start_triggered()
{
    //Clear table/gui
    reset_triggered();

    //Sort out path
    QDir save_path(path->text());

    if (!save_path.exists())
    {
         QMessageBox::warning(0, "Error!", "The program doesn't think the save directory exists, so is going to default back to the direcctory in which the executable is.");
         QString program_path(QCoreApplication::applicationDirPath());
         program_path.append(QDir::separator());
         path->setText(program_path);
    }
    //RJG - Set up save directory
    if(!save_path.mkpath(QString(PRODUCTNAME)+"_output")){QMessageBox::warning(this,"Error","Cant save output files. Permissions issue?");return;}
    else save_path.cd(QString(PRODUCTNAME)+"_output");

    //To check all works as expected there is a work log that writes everything to a text file
    QString work_log_filename=(QString(PRODUCTNAME)+"_working_log.txt");
    work_log_filename.prepend(save_path.absolutePath()+QDir::separator());
    QFile work_log_file(work_log_filename);

    QTextStream work_out;

    QString alive_record ("Each line lists: Iteration\tPlaying field\tSpecies alive\n");
    QTextStream alive_record_stream(&alive_record);

    if(work_log)
    {
        if(!work_log_file.open(QIODevice::QIODevice::WriteOnly|QIODevice::Text))QMessageBox::warning(0, "Error!", "Error opening working log file to write to.");
        else work_out.setDevice(&work_log_file);
    }

    //Sort GUI
    if(!batch_running && !calc_factor_running) gui_start_run();

    /************* SET UP SIMULATION *************/
     //In some settings, genome size will change - but this risks messing with a global. Other variables can change due to rounding from multiplying/dividing by float. Hence keep copy of the actual values and reset at end to make sure nothing changes
    int run_genome_size = genome_size;
    int run_species_difference=species_difference;

    // If stripping uninformative characters, in order to have the requested char # need to over-generate, and then subsample to requested value
    if(strip_uninformative)
    {

        if(((factor_settings!=print_Settings()) || strip_uninformative_factor<1.)
                        && !calc_factor_running && !factor_settings_batch)
            {
            if (QMessageBox::question(0, "Hmmm", "It looks like you have not calculated the strip uninformative factor for these settings. Would you like to?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)recalculate_strip_uniformative_factor();
                else if (strip_uninformative_factor<1.)strip_uninformative_factor=2.;
            if(batch_running)factor_settings_batch=true;
            }

        if(strip_uninformative_factor>20)strip_uninformative_factor=20.;

        run_genome_size *=strip_uninformative_factor;
        //I suspect it makes sense to do this so it still hits expected behaviour - but note that that means we can't guaruntee species difference if stripping uninformative characters
        //Based on experimentation, doesn't need to be scaled by quite so much - 1/3 seems fine most of time
        run_species_difference*=(strip_uninformative_factor/3.);
    }

    // Load masks
    //2 vectors to allow list of bools, with multiple user defined lists per environment
    QVector<QVector<bool> > masks;

    //This will fill the playing field(s) with random numbers as requried
    for (int j=0;j<mask_number;j++)
            {
                masks.append(QVector <bool>());
                for (int i=0;i<run_genome_size;i++)
                    {
                    if (simulation_randoms->gen_random() > (MAX_RAND/2)) masks[j].append(bool(false));
                    else  masks[j].append(bool(true));
                    }
            }

    if(work_log)
    {
       work_out<<"\n\nMasks initiated:\n";
       for (int j=0;j<mask_number;j++)
               {
                   work_out<<"Mask:"<<j<<"\t";
                   for (int i=0;i<run_genome_size;i++)
                       {
                       if (masks[j][i])work_out<<"1";
                       else  work_out<<"0";;
                       }
                work_out<<"\n";
                }
     }

    // Initialise variables
    int species_count=0, iterations=0;
    //Groups is a string used to write unresolvable clades.
    groups.clear();

    // Playing field of living organisms... All set to zero in constructor
    QVector <organism*> playing_field;

    for (int i=0; i<pfield_size;i++) playing_field.append(new organism(run_genome_size));

    //Want first organism to start somewhere near a fitness peak else short runs will just be a move towards a peak, I think - and not reflect ongoing evolutionary change
    //Obviously this is *not* the case for radiations e.g. terrestrialisation. But those are relatively rarer, I guess?
    //Worth thinking about anyway...

    //Record min (best) fitness
    quint32 min_fitness=-1;
    organism best_org=*playing_field[0];
    int count=0;
    do{
        //First organism - initialise and fill playing field with it
        playing_field[0]->initialise(run_genome_size);
        playing_field[0]->fitness=fitness(playing_field[0],masks);
        if(playing_field[0]->fitness<min_fitness)
                {
                best_org=*playing_field[0];
                min_fitness=best_org.fitness;
                }
        count++;
     }while(count<200);


    //Populate playing field with clones
    quint32 not_found=-1;
    if (min_fitness==not_found){QMessageBox::warning(0,"Oops","I've not managed to initialise this correctly. Please try different settings or email RJG.");gui_finish_run();return;}
    for (int i=0;i<pfield_size;i++)*playing_field[i]=best_org;

    //Species list to store details of each new species
    QVector <organism*> species_list;

    // Above is also the first species, obvs.
    species_list.append(new organism(run_genome_size));
    *species_list[0]=*playing_field[0];

    if(sansomian)print_blank(species_count);
    else print_genome(species_list[species_count], species_count);

    //Create string to record tree in parantheses format
    QString tnt_string;

    // Zero padding
    if (taxon_number>1000)
        {
        //Not actually an issue because of spin box limits. But still...
        QMessageBox::warning(0,"Well","I guess you were pitching for more than 1000 terminals. Best email RJG.");
        tnt_string="This isn't going to work, I'm afraid.";
        }

    //In case user has hit escape whilst not running loop
    escape_pressed=false;

    /************* Start simulation *************/
    do{
            //Pause if required....
            while(pause_flag==true && !escape_pressed)qApp->processEvents();

            if(escape_pressed)
            {
            if(!batch_running)escape_pressed=false;
            clear_vectors(playing_field, species_list, masks);
            reset_triggered();
            return;
            }

             if(work_log)
             {
                work_out<<"\n\nSimulation running. Iteration: "<<iterations;

                //Debugging code for outputting playing field
                for (int j=0;j<pfield_size;j++)
                    {
                    work_out<<"Playingfield pos: "<<j<<" Species ID: "<<playing_field[j]->species_id<<"\tGenome:\t";
                    for(int i=0;i<run_genome_size;i++)
                        {
                         if(playing_field[j]->genome[i]==false)work_out<<"0";
                         else work_out<<"1";
                         qApp->processEvents();
                        }
                    work_out<<"\tParent genome:\t";
                    for(int i=0;i<run_genome_size;i++)
                        {
                         if(playing_field[j]->parent_genome[i]==false)work_out<<"0";
                         else work_out<<"1";
                         qApp->processEvents();
                        }

                    work_out<<"\tFitness:\t"<<playing_field[j]->fitness;

                    work_out<<"\n";
                    }

                work_out<<"\nCurrent tree prior to speciation:"<<tnt_string<<"\n\nCurrent species list:\n";

                for (int i=0;i<species_list.length();i++){
                                                            work_out<<"Entry "<<i<<" is species "<<species_list[i]->species_id<<" ";
                                                            for(int k=0;k<run_genome_size;k++)
                                                            {
                                                             if(species_list[i]->genome[k]==false)work_out<<"0";
                                                             else work_out<<"1";
                                                             qApp->processEvents();
                                                            }
                                                             work_out<<"\tBorn: "<<species_list[i]->born<<"\tExtinct: "<<species_list[i]->extinct<<"\n";
                                                           }

                   work_out<<"\n";
              }

             /******** Equivalent of sort playing field by fitness *****/
            //Move down the list and select one - make it likely it is near the top, so has good fitness (i.e. near level zero)
            //Currently 50% chance it'll choose the first, and so on, a la coin toss
            int marker=0;

            while(simulation_randoms->gen_random()>(MAX_RAND/2))if(marker++ >=pfield_size)marker=0;

             //Do this by using marker to work out which nth value from fittest we want to select, then find where this is in playing field and assign to array
            QVector <bool> ignore_list;
            QVector <int> found_pool;
            for (int i=0;i<playing_field.count();i++)ignore_list.append(false);
            bool found=false;
            int found_count=0;

            while(found==false)
            {
            //Find the correct genome to duplicate, rather than sorting
            int smallest[2]{(run_genome_size*5),0};
            for (int i=0;i<playing_field.count();i++)
                    {
                    //Go through playing field and find smallest fitness, and record how many there are of this.
                    if(!ignore_list[i])
                        {
                        if(playing_field[i]->fitness<smallest[0])
                                                {
                                                    smallest[0]=playing_field[i]->fitness;
                                                    smallest[1]=1;
                                                }
                        else if (playing_field[i]->fitness==smallest[0])
                                                 smallest[1]++;
                        }
                    }

            found_count+=smallest[1];
            if (found_count>=marker)found=true;

            for (int i=0;i<playing_field.count();i++)
                    {
                    //If found smallest, add all organisms with best fit to environment to a list to select between
                    if(found && playing_field[i]->fitness==smallest[0])found_pool.append(i);
                    else if (!found && playing_field[i]->fitness==smallest[0])ignore_list[i]=true;
                    }
            }

            //sort out found pool here - create select from a random member of the found pool
            int select=0;

            //Scale random number to size of pool
            double point_select=(double)simulation_randoms->gen_random()/((double)MAX_RAND/(double)found_pool.count());
            int int_point_select=int(point_select);

            //Avoid rounding crash when random is rand max.
            if(int_point_select==found_pool.count())int_point_select--;
            select=found_pool[int_point_select];


            //Need to duplicate one
            organism progeny = *playing_field[select];

            //Mutate
            int temp_fitness=progeny.fitness;

            double mutations=((float)run_genome_size/100.)*organism_mutation;
            double organism_mutation_integral=(int) mutations;
            double fractional = modf(mutations, &organism_mutation_integral);
            int organism_mutation_int= (int) organism_mutation_integral;
            if(simulation_randoms->gen_random()<(fractional*MAX_RAND))organism_mutation_int++;

            QVector <int> SNPs;

            for(int i=0;i<organism_mutation_int;i++)
                {
                   //Scale random number to genome size
                   double point_mut=(double)simulation_randoms->gen_random()/((double)MAX_RAND/(double)run_genome_size);
                   int int_point_mut=int(point_mut);

                   //Sanity check as due to rounding, very occasionally, int_point_mut > genome_size and this causes a crash as trying to access, e.g. 500th entry in 500 array (where max is 499)
                   if (int_point_mut==run_genome_size)int_point_mut=run_genome_size-1;

                   SNPs.append(int_point_mut);

                   //Apply mutation
                   progeny.genome[int_point_mut]=!progeny.genome[int_point_mut];
                }

            //Update fitness
            progeny.fitness=fitness(&progeny,masks);

            if(no_deleterious && (temp_fitness<progeny.fitness))
                for(int i=0;i<SNPs.count();i++)progeny.genome[SNPs[i]]=!progeny.genome[SNPs[i]];

            //Reminder, in initialise prog genome is set equal to genome
            int diff=parent_genome_diff(&progeny, run_genome_size);

            /************* New species *************/
            //Assymetry of tree changes with level of difference below, plus balance between rate of mutation of environment and organism
            if (diff>=run_species_difference)
                {

                    if(work_log) work_out<<"New species has appeared this iteration - species "<<progeny.species_id<<" gives birth to "<<species_count+1<<" at iteration "<<iterations<<"\n\n";

                    //Update cladogenesis counter for each - use this to work out terminal branch lengths later.
                    progeny.cladogenesis=iterations;

                    //Record parent ID
                    int parent_species=playing_field[select]->species_id;

                    //Reset timer on all of this species to avoid clustering of speciation events as multiple individuals hit n mutations
                    //Do this independently for each playing field
                    for(int i=0; i<playing_field.count();i++)
                        if (playing_field[i]->species_id==parent_species)
                             for (int j=0;j<run_genome_size;j++)playing_field[i]->parent_genome[j]=progeny.genome[j];

                    species_list[parent_species]->cladogenesis=iterations;

                    //Update parent, which will be sister group to new one in tree.
                    progeny.parent_species_id=progeny.species_id;
                    //Reset last child as no children yet

                    //Iterate species count
                    species_count++;

                    //Update species ID, born interation
                    progeny.species_id=species_count;
                    progeny.born=iterations;

                    //Set parent to record last child species
                    species_list[parent_species]->children.append(species_count);

                    //genome → progenator genome
                    for (int j=0;j<run_genome_size;j++)progeny.parent_genome[j]=progeny.genome[j];

                    //Add to species list
                    species_list.append(new organism(run_genome_size));
                    *species_list[species_count]=progeny;

                    //Only print if not Sansomian speciation
                    if(sansomian)print_blank(species_count);
                    else print_genome(species_list[species_count], species_count);

                    //Update tree string, write to GUI. First sort out zero padding.
                    QString prog_species_ID;
                    if(taxon_number<100) prog_species_ID=QString("%1").arg(progeny.parent_species_id, 2, 10, QChar('0'));
                    else prog_species_ID=QString("%1").arg(progeny.parent_species_id, 3, 10, QChar('0'));

                    QString species_ID;
                    if(taxon_number<100) species_ID=QString("%1").arg(progeny.species_id, 2, 10, QChar('0'));
                    else species_ID=QString("%1").arg(progeny.species_id, 3, 10, QChar('0'));

                    //Then write string
                    //If first iteration, write manually so as to avoid starting with excess brackets.
                    if(species_count==1 && taxon_number<100)tnt_string="(00 01)";
                    else if (species_count==1 && taxon_number>=100)tnt_string="(000 001)";
                    else{
                            //Then split string at the ancestral species - first search for the progenator
                            int position = tnt_string.indexOf(prog_species_ID);

                            //You should never find a bracket or not find anything
                            if (position<1) QMessageBox::warning(0,"Eesh","You shouldn't see this. There's been an error writing the tree string. Please contact RJG in the hope he can sort this out.");

                            // Split tree file at this point, into left, and right of position.
                            QString tre_left=tnt_string.left(position);
                            QString tre_right=tnt_string.right(tnt_string.length()-(position+prog_species_ID.length()));

                            //Strip spaces if required - is not for TNT...
                            //if(tre_left.at(tre_left.length()-1)==' ')tre_left.remove(tre_left.length()-1,1);
                            //if(tre_right.at(0)==' ')tre_right.remove(0,1);

                            //Build new string
                            QString ins=QString("(%1 %2)").arg(prog_species_ID).arg(species_ID);
                            tnt_string=tre_left+ins+tre_right;
                        }

                        //Update display
                        ui->tree_Display->setText(tnt_string);
                        //resize as required
                        int h = (int) ui->tree_Display->document()->size().height();
                        ui->tree_Display->setFixedHeight(h);
                        ui->tree_Display->updateGeometry();

                }

            //Place progeny in playing field - used to place randomly; scaling random number to playing field size int settle=simulation_randoms->gen_random()/(MAX_RAND/pfield_size);
            //Due to rounding, this is occasionally =pfield_size, which causes crash. Fix: if (settle==pfield_size)settle=pfield_size-1;
            //Settle progeny post-mutation *playing_field[settle]=progeny;
            //Or insert new organism into playing field (qvector of organisms) at rght place here then shunt least fit off bottom; while (playing_field[position]->fitness>progeny.fitness)position++; playing_field.insert();

           //Now settle by overwriting least fit one, or if >1 lowest fitness, but overwriting a randomly chosen one of these.
           QVector <int> delete_pool;
           int lowest_fitness=-1;

           //Find lowest fitness in playing field
           for (int i=0;i<playing_field.count();i++)
                  if(playing_field[i]->fitness>lowest_fitness)
                                                   lowest_fitness=playing_field[i]->fitness;

           //Write list of least fit organisms
           for (int i=0;i<playing_field.count();i++)
                   if(playing_field[i]->fitness==lowest_fitness)delete_pool.append(i);
           int overwrite;
           if (delete_pool.count()==1)overwrite=delete_pool[0];
           else
            {
               //Scale random number to size of pool
               double point_delete=(double)simulation_randoms->gen_random()/((double)MAX_RAND/(double)delete_pool.count());
               int int_point_delete=int(point_delete);
               //Deal with case where max rand gives value of size of delete pool (and max of array is delete pool -1)
               if(int_point_delete==delete_pool.count())int_point_delete--;
               overwrite=delete_pool[int_point_delete];
           }

           //Overwrite the thing
           *playing_field[overwrite]=progeny;

           //Debugging code for outputting playing field
           if(work_log)
           {
               work_out<<"Playing field post palcement of new organism/at end of iteration, if new organism not added:\n";
               work_out<<"\n\nPlaying field\n\n";
               for (int j=0;j<pfield_size;j++)
                   {
                   work_out<<"Playingfield pos: "<<j<<" Species ID: "<<playing_field[j]->species_id<<"\tGenome:\t";
                   for(int i=0;i<run_genome_size;i++)
                       {
                        if(playing_field[j]->genome[i]==false)work_out<<"0";
                        else work_out<<"1";
                        qApp->processEvents();
                       }
                   work_out<<"\tParent genome:\t";
                   for(int i=0;i<run_genome_size;i++)
                       {
                        if(playing_field[j]->parent_genome[i]==false)work_out<<"0";
                        else work_out<<"1";
                        qApp->processEvents();
                       }

                      work_out<<"\tFitness:\t"<<playing_field[j]->fitness;
                      work_out<<"\n";
                   }

           }


            //Increase iteration count
            iterations++;

            QString status=QString("Iteration: %1").arg(iterations);
            if (batch_running)status.prepend(QString("Run number: %1; ").arg(runs));
            if (calc_factor_running)status.prepend(QString("Calculating strip uninformative factor. "));
            ui->statusBar->showMessage(status);

            //Update gui
            qApp->processEvents();

            /************* Mutate environment *************/

            //Calculate mutation # as previously, and using same variables for ease - this is the number of mutations total for each mask
            double env_mutations=((float)run_genome_size/100.)*environment_mutation;
            double env_mutation_integral=(int) env_mutations;

            //Sort out the probabilities of mextra mutation given remainder
            double env_fractional = modf(env_mutations, &env_mutation_integral);
            int env_mutation_int= (int) env_mutation_integral;
            if(simulation_randoms->gen_random()<(env_fractional*MAX_RAND))env_mutation_int++;

            //Mutate irrespective of playing field mode settings if there are multiple ones
                for(int k=0;k<env_mutation_int;k++)
                        for (int i=0; i<mask_number; i++)
                            {
                                //Scale random number to genome size - doubles then convert to avoid rounding errors creating number out of bounds
                                double mut=(double)simulation_randoms->gen_random()/((double)MAX_RAND/(double)run_genome_size);

                                int mutint=int(mut);

                                //Avoid rounding crash
                                if(mutint==run_genome_size)mutint=run_genome_size-1;

                                masks[i][mutint]=!masks[i][mutint];
                            }

            //Calculate fitness
            for (int i=0;i<pfield_size;i++)playing_field[i]->fitness=fitness(playing_field[i],masks);

            //Check and record species genome if it is last surviving instance, for both extinction and sansomian speciation.
            //Some redundancy here, but easier approach than waiting for extinction then recovering genome

            // For each species, Loop through playing field and count and when one instance of a species exists, overwrite genome in species list, and update display
            for (int i=0;i<=species_count;i++)
            {
                int count=0, index=-1;

                for (int j=0;j<pfield_size;j++)
                    if(playing_field[j]->species_id==i)
                        {
                        count++;
                        index=j;
                        }

                if(count==1)
                    {
                     //Plus one as will actually be made extinct next iteration if overwritten.
                     species_list[i]->extinct=iterations+1;
                     if(work_log) work_out<<"For write genome at extinction, replacing species list species "<<species_list[i]->species_id<<" that is entry "<<i<<" with genome of playing field "<<index<<"\n\n";
                     if(sansomian) for(int j=0;j<run_genome_size;j++)species_list[i]->genome[j]=playing_field[index]->genome[j];
                    }

                //Print if has gone extinct
                if(count==0)print_genome(species_list[i],i);
            }

    }while(species_count<taxon_number-1);

    if(work_log)
    {
       work_out<<"\n\nMasks at end of run:\n";
       for (int j=0;j<mask_number;j++)
               {
                   work_out<<"Mask:"<<j<<"\t";
                   for (int i=0;i<run_genome_size;i++)
                       {
                       if (masks[j][i])work_out<<"1";
                       else  work_out<<"0";;
                       }
                work_out<<"\n";
                }
            work_out<<"\n\n";
    }

    /************* Mop up writing genome for any organisms still alive under Sansomian speciation and for extinction/branch lengths *************/
    //QVector list for each species ID
     QVector <int> alive;

     //For alive organisms need to record genome of one that is alive (rather than when it was born - in extreme case zero could not have died and have genome from beginning of run)
     //Best to record one of the fittest - hence on this occasion, for ease, going to sort playing field
     //Only works with c++ 11 thanks to lamda - hence addition of CONFIG += c++11 in .pro
     std::sort(playing_field.begin(),playing_field.end(),[](const organism* OL, const organism* OR){return OL->fitness<OR->fitness;});

     //Go down playing field
     for (int j=0;j<pfield_size;j++)
             {
                int flag=0;
                //Check if the entry on playing field has previously been recorded/updated - if so ignore
                for (int i=0;i<alive.length();i++)if(alive[i]==playing_field[j]->species_id)flag=1;

                if  (flag==0)
                    {
                    //Update the extinction and if required, genome.
                    int species=playing_field[j]->species_id;

                    alive.append(species);
                    if(sansomian)
                                {
                                for (int k=0;k<run_genome_size;k++)species_list[species]->genome[k]=playing_field[j]->genome[k];
                                print_genome(species_list[species],species);
                                }
                    species_list[species]->extinct=iterations;
                    }
             }

     if(work_log)
     {
        work_out<<"\n\nRun is finished. Now have some cleaning up to do.\n\n";

        work_out<<"\nCurrent tree: "<<tnt_string<<"\n\nSpecies list at end is:\n";

        for (int i=0;i<species_list.length();i++){
                                                    work_out<<"Entry "<<i<<" is species "<<species_list[i]->species_id<<" ";
                                                    for(int k=0;k<run_genome_size;k++)
                                                    {
                                                     if(species_list[i]->genome[k]==false)work_out<<"0";
                                                     else work_out<<"1";
                                                     qApp->processEvents();
                                                    }
                                                     work_out<<"\tBorn: "<<species_list[i]->born<<"\tExtinct: "<<species_list[i]->extinct<<"\tLast cladogenesis: "<<species_list[i]->cladogenesis<<"\n";
                                    }

           work_out<<"\n\n";
        }

    /************* Strip uninformative characters, if required *************/
    //Needed to make a decision here and arbitrarily decide how to deal with partations used for defining fitness, species, and the whole genome.
    //At present split into two partitions - that used for fitness calculation, and that not, and treat individually.
    //Don't think preserving that partition used for species definition is so important, so ignoring - but may well prove to be wrong.

    //Work out which are uninformative - create list
    QList <int> uninformative;
    QList <int> uninformative_non_coding;

    //Test for informative
    for (int i=0;i<run_genome_size;i++)
        {
        int count=0;
        for (int j=0; j<taxon_number; j++) if(species_list[j]->genome[i])count++;
        if (count<2 || count>(taxon_number-2)) uninformative.append(i);
        }

    int number_uninformative=uninformative.length()+uninformative_non_coding.length();
    QString status_string = QString ("There are %1 uninformtive characters in your matrix, and the software is not set to strip these out. This is not necessarily a problem, but I thought you should know.").arg(number_uninformative);

    if(!strip_uninformative && uninformative.length()>0)ui->statusBar->showMessage(status_string);
    //Strip the characters as requried
    else
    {
        if(work_log)
            {
                work_out<<"Stripping uninformative characters. Prior to removal:\n";
                for (int i=0;i<taxon_number;i++)
                    {
                        work_out<<"Species_"<<i<<"\t";
                        for(int j=0;j<run_genome_size;j++)
                            {
                            if(species_list[i]->genome[j]==false)work_out<<"0\t";
                            else work_out<<"1\t";
                            }
                    work_out<<"\n";
                   }

            }

        //Delete uninformative characters
        for (int j=0; j<taxon_number; j++)
                for (int i=uninformative.size()-1;i>=0;i--)
                    species_list[j]->genome.remove(uninformative[i]); //Start at end and work back to avoid numbering issues post-deletion.

        //Sort out variables
        if(strip_uninformative)
        {
            run_genome_size=genome_size;
            run_species_difference=species_difference;
        }

        if(calc_factor_running)
        {
        //Record how many characters here, no need to do anything more
        informative_characters=species_list[0]->genome.size();
        clear_vectors(playing_field, species_list, masks);
        return;
        }

        bool char_number = true;

        if(strip_uninformative)
            if (species_list[0]->genome.size()<run_genome_size) char_number=false;

        if (!char_number)
            {
                if (error_batch==false)
                   {
                    if(batch_running)error_batch=true;
                    QMessageBox *warningBox = new QMessageBox;
                    //Delete when closed so no memory leak
                    warningBox->setAttribute(Qt::WA_DeleteOnClose, true);
                    warningBox->setWindowTitle(tr("Oops"));
                    warningBox->setText("It seems there are not enough informative characters to pull this off. Best either try different settings, or email RJG and he can sort. If you're running a batch, the program will keep on trying (and this is the sole error message you'll see, which will time out after ten minutes) - but if it's not got anywhere after half an hour or so, maybe quit, restart and change settings (e.g. species difference) to try and have more luck.");
                    warningBox->show();
                    //Close after ten minutes.
                    QTimer::singleShot(6000000, warningBox, SLOT(close()));
                    }

                reset_triggered();
                if(work_log) work_out<<"Return at !char_number\n";

                clear_vectors(playing_field, species_list, masks);
                return;
            }

         if(work_log)
            {
                work_out<<"After removal:\n";

                for (int i=0;i<taxon_number;i++)
                {
                    work_out<<"Species_"<<i<<"\t";
                    for(int j=0;j<run_genome_size;j++)
                        {
                        if(species_list[i]->genome[j]==false)work_out<<"0\t";
                        else work_out<<"1\t";
                        }
                    work_out<<"\n";
               }
            }
    }

    /******** Check for intrisnically unresolvable clades *****/

     bool unresolvable=false;
     QVector <QVector <int> > unresolvable_species;

     if(work_log) work_out<<"Checking for unresolvable clades";

     QString message("A heads up. There are intrinscially unresolvable taxa in this matrix (i.e. at least two taxa have identical genomes); this could affect your results. For more information, check out:\n\nBapst, D.W., 2013. When can clades be potentially resolved with morphology?. PLoS One, 8(4), p.e62312.\n\nThe taxa in question are:");


     //Compare each against each
     for (int i=0; i<taxon_number-1; i++)
         for (int j=i+1; j<taxon_number;j++)
            {
                int count=0;
                for (int k=0;k<run_genome_size;k++)if(species_list[i]->genome[k]==species_list[j]->genome[k])count++;

                //Taxa have identical genomes
                if (count==run_genome_size)
                    {
                    unresolvable=true;

                    //Clump them into identical clusters
                    bool match=false;

                    for(int l=0;l<unresolvable_species.count();l++)
                        {
                        int count2=0;
                        for (int k=0;k<run_genome_size;k++)
                            if(species_list[unresolvable_species[l][0]]->genome[k]==species_list[i]->genome[k])count2++;

                        //Either add to already existing cluster
                        if (count2==run_genome_size && !match)
                                    {
                                     match=true;

                                        bool match2=false;
                                        for(int m=0; m<unresolvable_species[l].length();m++)
                                            if(unresolvable_species[l][m]==j)match2=true;
                                        if(!match2)unresolvable_species[l].append(j);

                                    }
                        }

                    //Or add new one
                    if (!match)
                        {
                        QVector <int> new_group;
                        new_group.append(i);
                        new_group.append(j);
                        unresolvable_species.append(new_group);
                        }

                    }
                }

     int unresolvable_count=0;
     for(int l=0;l<unresolvable_species.count();l++)unresolvable_count+=unresolvable_species[l].length();

     if(unresolvable && unresolvable_batch==false)
                    {
                     if(batch_running)unresolvable_batch = true;

                    //Warning - just so people know.
                    message.append(QString("\n\n"));

                    for(int l=0;l<unresolvable_species.count();l++)
                        {
                            for(int m=0;m<unresolvable_species[l].count();m++) groups.append(QString("%1 ").arg(unresolvable_species[l][m]));
                            groups.append(QString("\n"));
                        }

                    message.append(groups);

                    if(unresolvable_count>unresolvable_cutoff)
                    {
                        message.append(QString("\n\n"));
                        message.append(QString("In this case, you are above the cutoff of resolvable taxa - there are %1 identical terminals. If you are in batch mode, this will carry on trying until you have hit the requested number of runs. If you're not, you may want to try again (or hit run for, and then enter one run to automatically run it till one sticks).").arg(unresolvable_count));
                    }

                    QMessageBox *warningBox2 = new QMessageBox;
                    //Delete when closed so no memory leak
                    warningBox2->setAttribute(Qt::WA_DeleteOnClose, true);
                    warningBox2->setWindowTitle(tr("Warning"));
                    warningBox2->setText(message);
                    warningBox2->show();
                    //Close after ten minutes.
                    QTimer::singleShot(6000000, warningBox2, SLOT(close()));

                    if(work_log) work_out<<message;
                    }
     else
        {
         groups="There are no unresolvable taxa";
         if(work_log) work_out<<"\n\nNo unresolvable clades";
        }

     if(unresolvable_count>unresolvable_cutoff)
        {
        if(work_log) work_out<<"Return at less than unresolvable cutoff\n";
        reset_triggered();
        return;
        }

    /************* Write files *************/

    //File 01 - if needed this can be expanded and run from a loop to include more files...
    QString filename_01;
    if(!append || !batch_running )
    {
        filename_01 = base_01;
        filename_01.append(QString("%1").arg(runs, 3, 10, QChar('0')));
        filename_01.append(ext_01);
    }
    else
    {
        filename_01 = base_01;
        filename_01.append("batch");
        filename_01.append(ext_01);
    }

    filename_01.prepend(save_path.absolutePath()+QDir::separator());

    QFile file_01(filename_01);
    if(!append){if(!file_01.open(QIODevice::WriteOnly|QIODevice::Text)){QMessageBox::warning(0, "Error!", "Error opening file 1 to write to.");gui_finish_run(); clear_vectors(playing_field, species_list, masks);return;}}
    else {if (!file_01.open(QIODevice::Append|QIODevice::Text)){QMessageBox::warning(0, "Error!", "Error opening file 1 to write to.");gui_finish_run(); clear_vectors(playing_field, species_list, masks);return;}}

    //File 02 - if needed this can be expanded and run from a loop to include more files...
    QString filename_02;
    if(!append || !batch_running)
    {
        filename_02 = base_02;
        filename_02.append(QString("%1").arg(runs, 3, 10, QChar('0')));
        filename_02.append(ext_02);
    }
    else
    {
        filename_02 = base_02;
        filename_02.append("batch");
        filename_02.append(ext_02);
    }

    filename_02.prepend(save_path.absolutePath()+QDir::separator());

    QFile file_02(filename_02);
    if(!append){if(!file_02.open(QIODevice::WriteOnly|QIODevice::Text)){QMessageBox::warning(0, "Error!", "Error opening file 2 to write to.");gui_finish_run(); clear_vectors(playing_field, species_list, masks);return;}}
    else {if (!file_02.open(QIODevice::Append|QIODevice::Text)){QMessageBox::warning(0, "Error!", "Error opening file 2 to write to.");gui_finish_run(); clear_vectors(playing_field, species_list, masks);return;}}

    QString filename_03;
    filename_03 = base_03;
    filename_03.append(QString("%1").arg(runs, 3, 10, QChar('0')));
    filename_03.append(ext_03);
    filename_03.prepend(save_path.absolutePath()+QDir::separator());


    QTextStream file_01_out(&file_01);
    QString filestring_01_write=filestring_01;

    filestring_01_write.replace("||Matrix||",print_matrix(species_list, run_genome_size));
    filestring_01_write.replace("||TNT_Tree||",tnt_string);
    filestring_01_write.replace("||MrBayes_Tree||",print_newick_bl(0,species_list));
    filestring_01_write.replace("||Time||",print_Time());
    filestring_01_write.replace("||Settings||",print_Settings());
    filestring_01_write.replace("||Alive_Record||",alive_record);
    QString cnumber;
    filestring_01_write.replace("||Character_Number||", cnumber.number(run_genome_size));
    QString tnumber;
    filestring_01_write.replace("||Taxon_Number||",tnumber.number(taxon_number));
    QString sruns;
    filestring_01_write.replace("||Count||",sruns.number(runs));
    filestring_01_write.replace("||Unresolvable||",groups);
    QString sdif;
    filestring_01_write.replace("||SD||", sdif.number(species_difference));
    QString oCharNum;
    filestring_01_write.replace("||OCN||", oCharNum.number(genome_size));
    QString ni;
    filestring_01_write.replace("||Uninformative||", ni.number(number_uninformative));
    QString identical;
    filestring_01_write.replace("||Identical||", identical.number(unresolvable_count));

    file_01_out<<filestring_01_write;

    file_01.close();

    QTextStream file_02_out(&file_02);
    QString filestring_02_write=filestring_02;

    filestring_02_write.replace("||Matrix||",print_matrix(species_list, run_genome_size));
    filestring_02_write.replace("||TNT_Tree||",tnt_string);
    filestring_02_write.replace("||MrBayes_Tree||",print_newick_bl(0,species_list));
    filestring_02_write.replace("||Time||",print_Time());
    filestring_02_write.replace("||Settings||",print_Settings());
    filestring_02_write.replace("||Alive_Record||",alive_record);
    QString cnumber2;
    filestring_02_write.replace("||Character_Number||", cnumber2.number(run_genome_size));
    QString tnumber2;
    filestring_02_write.replace("||Taxon_Number||",tnumber2.number(taxon_number));
    QString sruns2;
    filestring_02_write.replace("||Count||",sruns2.number(runs));
    filestring_02_write.replace("||Unresolvable||",groups);
    QString ni2;
    filestring_02_write.replace("||Uninformative||", ni2.number(number_uninformative));
    QString identical2;
    filestring_02_write.replace("||Identical||", identical2.number(unresolvable_count));


    file_02_out<<filestring_02_write;

    file_02.close();

    if(write_tree)
        {
            //File 03 is tree file in .nex format - withour zero padding
            QFile file_03(filename_03);
            if(!file_03.open(QIODevice::WriteOnly|QIODevice::Text)){QMessageBox::warning(0, "Error!", "Error opening treefile to write to.");gui_finish_run(); clear_vectors(playing_field, species_list, masks);return;}

            QTextStream file_03_out(&file_03);

            filestring_03.replace("||Time||",print_Time());
            filestring_03.replace("||Settings||",print_Settings());

            file_03_out<<filestring_03;

            for (int i=0;i<taxon_number;i++)
            {
                if(taxon_number<100)file_03_out<<(QString("S_%1").arg(i+1, 2, 10, QChar('0')));
                else file_03_out<<(QString("S_%1").arg(i+1, 3, 10, QChar('0')));

                file_03_out<<"\t\t"<<"Species_"<<i<<",\n";
            }
            file_03_out<<"\t\t;\n\ntree tree1 = [&U]";

            file_03_out<<print_newick_bl(0,species_list)<<";\n\nEND;";

            file_03.close();
        }

    if(work_log)
     {
        work_out<<"On exit, "<<(QString(PRODUCTNAME))<<" thinks tree is "<<print_newick_bl(0,species_list)<<"\n or in TNT format: "<<tnt_string;
        work_log_file.close();
     }

    runs++;

    //Sort GUI
    if(!batch_running && !calc_factor_running) gui_finish_run();

    //Sort memory
    clear_vectors(playing_field, species_list, masks);
}

void MainWindow::escape()
{
    escape_pressed=true;
}

void MainWindow::pause_triggered()
{
pause_flag=!pause_flag;
}

void MainWindow::reset_triggered()
{
 /******* Sort out displays *******/
    // Clear table
    for (int i=0;i<ui->character_Display->rowCount();i++)
        for(int j=0;j<ui->character_Display->columnCount();j++)
            {
            QTableWidgetItem *item(ui->character_Display->item(i,j));
            item->setText(" ");
            }
    //Clear tree string
    QString tnt_string(" ");
    ui->tree_Display->setText(tnt_string);
    ui->tree_Display->updateGeometry();
    ui->statusBar->clearMessage();

 /******* And variables / gui if required - i.e. if hit by user. Don't really need this, but there as safety net *******/
    if(!batch_running)
    {
        gui_finish_run();
        pause_flag=false;
    }
 }

void MainWindow::runfor_triggered()
{

/******** Batch mode - multiple runs *****/

    bool ok;
    int run_for = QInputDialog::getInt(this, "Run for...","How many runs?", 25, 1, 10000, 1, &ok);
    if (!ok)return;

      gui_start_run();
      batch_running=true;

      runs=0;

      QProgressBar progress;
      progress.setRange(0,run_for);

      ui->statusBar->addPermanentWidget(&progress);

          do
          {
                start_triggered();
                reset_triggered();
                progress.setValue(runs);
          } while(runs<run_for && !escape_pressed);

      ui->statusBar->removeWidget(&progress);

      escape_pressed=false;

      //Reset gui etc.
      batch_running=false;
      error_batch=false;
      factor_settings_batch=false;
      unresolvable_batch=false;
      gui_finish_run();

    runs=0;
}

void MainWindow::settings_triggered()
{
Settings sdialogue;

sdialogue.exec();

if (sdialogue.resize_grid)
        {
        reset_triggered();
        resize_grid();
        }

if (sdialogue.recalc_strip_unin_factor)
            recalculate_strip_uniformative_factor();

}

void MainWindow::resize_grid()
{
    int width = genome_size;
    if (width>128)width=128;
    ui->character_Display->setColumnCount(width);
    ui->character_Display->setRowCount(taxon_number);

    //Titles
    QStringList labellist;
    for(int i=0;i<taxon_number;i++)labellist<<QString("Species " + QString::number(i));
    ui->character_Display->setVerticalHeaderLabels(labellist);

    //Populate table, and then modify these items as required when needed - think this is  better as less faffing with memory that creating and deleting new ones
    for (int i=0;i<taxon_number;i++)
        for(int j=0;j<genome_size;j++)
            if(ui->character_Display->item(i,j)==0)ui->character_Display->setItem(i, j, new QTableWidgetItem(" "));

    qApp->processEvents();

}

void MainWindow::output_triggered()
{
Output odialogue;
odialogue.exec();
}

void MainWindow::about_triggered()
{
About adialogue;
adialogue.exec();
}


void MainWindow::print_blank(int row)
{
    if(row>ui->character_Display->rowCount())return;
    for(int i=0;i<ui->character_Display->columnCount();i++)
        {
         QTableWidgetItem *item(ui->character_Display->item(row,i));
         item->setText("_");
         qApp->processEvents();
        }
}

void MainWindow::print_genome(const organism *org, int row)
{
    for(int i=0;i<ui->character_Display->columnCount();i++)
        {
         QTableWidgetItem *item(ui->character_Display->item(row,i));
         if(org->genome[i]==false)item->setText("0");
         else item->setText("1");
         qApp->processEvents();
        }
}


//Masks passed as a const reference.
int MainWindow::fitness(const organism *org, const QVector<QVector<bool> > &masks)
{
    int counts=0;

    for (int i=0;i<mask_number;i++)
        for (int j=0;j<org->genome.size();j++)
            if (org->genome[j]!=masks[i][j])counts++;

    //Define fitness as the distance away from having ~50% of genome different from all masks in environment. 0 highest, fitness size/2 lowest.
   return(qAbs(counts-(mask_number*(org->genome.size()/2))));

}

int MainWindow::genome_diff(const organism *org1, const organism *org2, int run_genome_size)
{
    int diff=0;
    for (int j=0;j<run_genome_size;j++)
        if (org1->genome[j]!=org2->genome[j])diff++;
    return diff;
}

int MainWindow::parent_genome_diff(const organism *org1, int run_genome_size)
{
    int diff=0;
    // Loop to select size to allow decoupling of species definition from genome size.
    for (int j=0;j<run_genome_size;j++)
       if (org1->genome[j]!=org1->parent_genome[j])diff++;
    return diff;
}

QString MainWindow::print_matrix(const QVector <organism *> species_list, int run_genome_size)
{
QString matrix_string;
QTextStream matrix(&matrix_string);

    for (int i=0;i<taxon_number;i++)
        {
        matrix<<"Species_"<<i<<"\t";
        for(int j=0;j<run_genome_size;j++)
            {
            if(species_list[i]->genome[j]==false)matrix<<"0";
            else matrix<<"1";
            }
        matrix<<"\n";
       }

return matrix_string;
}

QString MainWindow::print_Time()
{
    //Timestamp
    QDateTime date = QDateTime::currentDateTime();
    return date.toString();
}

//Recursive function for writing tree in Newick format, no branch lengths
QString MainWindow::print_newick(int species, QVector <organism*> &species_list)
{
    QString tree;
    QTextStream tree_out(&tree);

    int offspring = species_list[species]->children.length();

    //Zero padding
    QString species_ID;
    if(taxon_number<100) species_ID=QString("%1").arg(species_list[species]->species_id, 2, 10, QChar('0'));
    else species_ID=QString("%1").arg(species_list[species]->species_id, 3, 10, QChar('0'));

    if(offspring==0)
            {
            tree_out<<species_ID<<",";
            return tree;
            }

    for(int i=0;i<offspring;i++)tree_out<<"("<<print_newick(species_list[species]->children[i],species_list);

    tree_out<<species_ID;

    for  (int i=0;i<offspring;i++)tree_out<<")";

    if(species>0)tree_out<<",";

    return tree;
}

//Recursive function for writing trees with branch lengths in Newick format
QString MainWindow::print_newick_bl(int species, QVector <organism*> &species_list)
{
    QString tree;
    QTextStream tree_out(&tree);

    int offspring = species_list[species]->children.length();

    //Zero padding
    QString species_ID;
    if(taxon_number<100) species_ID=QString("S_%1").arg(species_list[species]->species_id+1, 2, 10, QChar('0'));
    else species_ID=QString("S_%1").arg(species_list[species]->species_id+1, 3, 10, QChar('0'));

    //For terminal cases (reused for branches to nodes below)
    int branch_length=species_list[species]->extinct-species_list[species]->cladogenesis;

    //Return recursion and print terminal taxa, and without offspring
    if(offspring==0)
            {
            //Comma here for terminal duos (e.g. XX,XY) and ladders (e.g. XX,(XY)) - else trees are not correctly parsed
            tree_out<<species_ID<<":"<<branch_length<<",";
            return tree;
            }

    //Then sort where taxa have offpsring - recurse
    for(int i=0;i<offspring;i++)tree_out<<"("<<print_newick_bl(species_list[species]->children[i],species_list);

    //Write offspring
    tree_out<<species_ID<<":"<<branch_length;

    //Close all brackets and sort out branch lengths
    for  (int i=offspring-1;i>=0;i--)
        {
        int child = species_list[species]->children[i];
        if(i>0)branch_length=species_list[child]->born-species_list[child-1]->born;
        else branch_length=species_list[child]->born-species_list[species]->born;
        tree_out<<"):"<<branch_length;
        }

    //Comma here for internal clade boundaries - e.g. ),(
    if(species>0)tree_out<<",";

    return tree;
}

QString MainWindow::print_Settings()
{

    QString settings_string;
    QTextStream settings(&settings_string);

    settings<<"variables : genome_size "<<genome_size
     <<" taxon_number "<<taxon_number
     <<" playingfield_size "<<pfield_size
     <<" species_difference "<<species_difference
     <<" environment_mutation "<<environment_mutation
     <<" organism_mutation "<<organism_mutation
     <<" unresolvable_cutoff "<<unresolvable_cutoff
     <<" mask_number "<<mask_number
     <<" strip_uninformative "<<strip_uninformative
     <<" append "<<append
     <<" write_tree "<<write_tree
     <<" sansomian "<<sansomian
     <<" no_deleterious "<<no_deleterious;

    return settings_string;
}

void MainWindow::recalculate_strip_uniformative_factor()
{
        bool temp_strip=strip_uninformative;
        calc_factor_running=true;
        strip_uninformative=true;
        strip_uninformative_factor=1.0;

        double strip_uninformative_factor_av=0.;

        gui_start_run();

        QProgressBar progress;
        progress.setRange(0,10);

        ui->statusBar->addPermanentWidget(&progress);

        for (int i=0;i<10;i++)
            {
                informative_characters=0;
                progress.setValue(i);
                start_triggered();
                strip_uninformative_factor_av+=(float)(genome_size*strip_uninformative_factor)/(float)informative_characters;
            }

        //Based on some experimentation, multiplying this by 1.3 will mean that ~90% of runs have enough characters - do this to keep number as low as possible for speed and branch length considerations, plus minimising number identical taxa post strip
        strip_uninformative_factor=(strip_uninformative_factor_av/10.);
        strip_uninformative_factor*=1.3;

        ui->statusBar->removeWidget(&progress);

        //Reset gui etc.
        gui_finish_run();

        calc_factor_running=false;
        strip_uninformative=temp_strip;

        factor_settings=print_Settings();

        reset_triggered();
        QString status=QString("Strip uninformative factor calculated as %1").arg(strip_uninformative_factor);
        ui->statusBar->showMessage(status);

}


void MainWindow::gui_start_run()
{
startButton->setEnabled(false);
pauseButton->setEnabled(true);
resetButton->setEnabled(false);
runForButton->setEnabled(false);
settingsButton->setEnabled(false);
}

void MainWindow::gui_finish_run()
{
startButton->setEnabled(true);
pauseButton->setEnabled(false);
resetButton->setEnabled(true);
runForButton->setEnabled(true);
settingsButton->setEnabled(true);
}


void MainWindow::clear_vectors(QVector <organism*> &playing_field, QVector <organism*> &species_list, QVector<QVector<bool> > &masks)
{
/************** Delete vectors / masks then free up memory to avoid leaks *************/
    qDeleteAll(playing_field);
    playing_field.clear();
    playing_field.squeeze();

    qDeleteAll(species_list);
    species_list.clear(); species_list.squeeze();

    masks.clear(); masks.squeeze();
}

void MainWindow::count_peaks()
{
       //Save settings to load at end
       save();

       bool ok;
       genome_size = QInputDialog::getInt(this, "Count peaks...","How many bits?", 32, 1, 64, 1, &ok);
       if(!ok){load();reset_triggered();return;}
       mask_number = QInputDialog::getInt(this, "Count peaks...","How many masks?", 3, 1, 64, 1, &ok);
       if(!ok){load();reset_triggered();return;}

       taxon_number=1;

       // Load masks
       QVector<QVector<bool> > masks;

       for (int j=0;j<mask_number;j++)
            {
                masks[j].append(QVector <bool>());
                for (int i=0;i<genome_size;i++)
                    {
                    if (simulation_randoms->gen_random() > (MAX_RAND/2)) masks[j].append(bool(false));
                    else  masks[j].append(bool(true));
                    }
            }


       resize_grid();

       organism org(genome_size);
       QVector <quint64> totals;
       for (int i=0; i< genome_size*mask_number; i++)totals.append(0);

       //Lookups for printing genomes
       quint64 lookups[64];
       lookups[0]=1;
       for (int i=1;i<64;i++)lookups[i]=lookups[i-1]*2;

       quint64 max = (quint64) pow(2.,(double)genome_size);

       //Progress bar max value is 2^16 - scale to this
       quint16 pmax = -1;
       QProgressBar progress;
       progress.setRange(0,pmax);
       ui->statusBar->addPermanentWidget(&progress);

       QVector <QVector <int> > genomes;
       bool record_genomes = false;
       if (genome_size<21) record_genomes=true;

       if(record_genomes)for(int i=0;i<genome_size*mask_number;i++)genomes.append(QVector <int>());


       for (quint64 x=0; x<max; x++)
               {

               //Create genome from number
               for (int i=0;i<genome_size;i++)
                   if(lookups[i] & x)org.genome[i]=true;
                       else org.genome[i]=false;

               //Update GUI every now and then to show not crashed
               if ((x%9999)==0)print_genome(&org,0);
               if((x%1000)==0){
                                double prog=((double)x/(double)max)*pmax;
                                progress.setValue((int)prog);
                                }

               org.fitness=fitness(&org,masks);

               totals[org.fitness]++;
               if(record_genomes)genomes[org.fitness].append(x);
                }

       //RJG - Set up save directory
       QDir save_path(path->text());
       if(!save_path.mkpath(QString(PRODUCTNAME)+"_output")){QMessageBox::warning(this,"Error","Cant save output files. Permissions issue?");return;}
       else save_path.cd(QString(PRODUCTNAME)+"_output");

        QString peaks_filename = (QString(PRODUCTNAME)+"_peak_counting.txt");
        peaks_filename.prepend(save_path.absolutePath()+QDir::separator());
        QFile peaks_file(peaks_filename);
        if (!peaks_file.open(QIODevice::Append|QIODevice::Text))QMessageBox::warning(0, "Error!", "Error opening curve file to write to.");
        QTextStream peaks_out(&peaks_file);

        peaks_out<<QString(PRODUCTNAME)<<" peak count for the settings: "<<print_Settings()<<"\n";

       for (int j=0;j<mask_number;j++)
                {
                   peaks_out<<"\nMask: "<<j<<": ";
                        for (int k=0;k<genome_size;k++)peaks_out<<masks[j][k];
                }


        peaks_out<<"\n\nGenomes tested: "<<max<<", distribution:\n";
        for (int i=0; i< genome_size*mask_number; i++)peaks_out<<"Fit to environment: "<<i<<" Number of genomes: "<<totals[i]<<"\n";

        peaks_out<<"\n\nGenome fit to environment as follows:\n";
        for (int i=0;i<genomes.length();i++)
                if (!genomes[i].empty())
                    {
                    peaks_out<<"Fit to environment "<<i<<"\n";
                    for (int j=0;j<genomes[i].length();j++)
                        peaks_out<<print_genome_int(genomes[i][j],genome_size,lookups)<<"\n";
                    }
        peaks_out<<"\n";
        peaks_file.close();

        //Load previous settings again
        load();
        resize_grid();
        reset_triggered();
}

QString MainWindow::print_genome_string(const organism *org, int genome_size_local)
{
    QString genome;
    for(int i=0;i<genome_size_local;i++)
        if(org->genome[i])genome.append("1");
                else genome.append("0");
    return genome;
}

QString MainWindow::print_genome_int(int genome_local, int genome_size_local, const quint64 *lookups_local)
{
    QString genome;
    for(int i=0;i<genome_size_local;i++)
            if(lookups_local[i] & genome_local)genome.append("1");
                else genome.append("0");

    return genome;
}
