#include "settings.h"
#include "ui_settings.h"
#include "mainwindow.h"

Settings::Settings(QWidget *parent, simulationVariables *simSettings) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    resizeGrid = false;
    recalculateStripUninformativeFactorOnClose = false;
    settings = simSettings;

    ui->setupUi(this);
    setWindowTitle("Simulation Settings");
    setWindowIcon(QIcon ("://resources/icon.png"));

    //Organism tab set up
    //Connect slots for on the fly maxima
    QObject::connect(ui->s_genome_size, SIGNAL(valueChanged(int)), this, SLOT(slotGenomeSizeChanged()));
    QObject::connect(ui->s_taxon_number, SIGNAL(valueChanged(int)), this, SLOT(slotTaxonNumberChanged()));
    QObject::connect(ui->s_multiple, SIGNAL(valueChanged(int)), this, SLOT(slotPlayingFieldNumberChanged()));

    ui->s_genome_size->setValue(settings->genomeSize);
    //RJG - some stuff redacted for 2.0.0 - email if you're interested
    ui->s_fitness_target->setValue(settings->fitnessTarget);
    ui->s_taxon_number->setValue(settings->taxonNumber);
    ui->s_species_difference->setValue(settings->speciesDifference);
    ui->s_organism_mutation->setValue(settings->organismMutationRate);
    ui->s_unresolvable_c->setValue(settings->unresolvableCutoff);
    ui->s_selection->setValue(settings->selectionCoinToss);
    ui->c_strip_uninformative->setChecked(settings->stripUninformative);
    ui->c_sansomian->setChecked(settings->sansomianSpeciation);
    ui->c_beneficial_mut->setChecked(settings->discardDeleterious);
    ui->c_random_overwrite->setChecked(settings->randomOverwrite);

    //Set sensible maxima
    int max = settings->genomeSize;
    //RJG - some stuff redacted for 2.0.0 - email if you're interested
    ui->s_species_difference->setMaximum(max);
    ui->s_fitness_target->setMaximum(ui->s_genome_size->value()*settings->maskNumber);

    recalculateStripUninformativeFactorOnClose = false;

    //Environment tab set up
    //Set values
    ui->s_environment_mutation->setValue(settings->environmentMutationRate);
    ui->s_playingfield_size->setValue(settings->playingfieldSize);
    ui->s_environment_number->setValue(settings->environmentNumber);
    ui->s_masks_per_environment->setValue(settings->maskNumber);
    //Connect this here so as not to mess with above.

    if (settings->playingfieldMasksMode == 0)ui->r_identical->setChecked(true);
    else if (settings->playingfieldMasksMode == 1)ui->r_independent->setChecked(true);
    else ui->r_start->setChecked(true);

    ui->s_multiple->setValue(settings->playingfieldNumber);
}


void Settings::on_buttonBox_accepted()
{
    //Sort out GUI if required
    if (settings->taxonNumber != ui->s_taxon_number->value() || settings->genomeSize != ui->s_genome_size->value())resizeGrid = true;
    recalculateStripUninformativeFactorOnClose = ui->c_recalc->isChecked();

    //Modify variables - organism tab
    settings->genomeSize = ui->s_genome_size->value();
    //RJG - some stuff redacted for 2.0.0 - email if you're interested
    settings->fitnessTarget = ui->s_fitness_target->value();
    settings->taxonNumber = ui->s_taxon_number->value();
    settings->speciesDifference = ui->s_species_difference->value();
    settings->organismMutationRate = ui->s_organism_mutation->value();
    settings->unresolvableCutoff = ui->s_unresolvable_c->value();
    settings->selectionCoinToss = ui->s_selection->value();

    settings->stripUninformative = ui->c_strip_uninformative->isChecked();
    settings->sansomianSpeciation = ui->c_sansomian->isChecked();
    settings->discardDeleterious = ui->c_beneficial_mut->isChecked();
    settings->randomOverwrite = ui->c_random_overwrite->isChecked();

    //Modify variables - environment tab
    settings->playingfieldSize = ui->s_playingfield_size->value();
    settings->environmentNumber = ui->s_environment_number->value();
    settings->maskNumber = ui->s_masks_per_environment->value();
    settings->environmentMutationRate = ui->s_environment_mutation->value();
    settings->playingfieldNumber = ui->s_multiple->value();

    if (ui->r_identical->isChecked())settings->playingfieldMasksMode = 0;
    else if (ui->r_independent->isChecked())settings->playingfieldMasksMode = 1;
    else settings->playingfieldMasksMode = 2;
}

void Settings::slotGenomeSizeChanged()
{

//Set sensible maxima on the fly
    ui->s_species_difference->setMaximum(ui->s_genome_size->value());
    ui->s_fitness_target->setMaximum(ui->s_genome_size->value()*settings->maskNumber);

//Adjust values
    ui->s_species_difference->setValue(ui->s_genome_size->value() / 10);
}

void Settings::slotTaxonNumberChanged()
{
    ui->s_unresolvable_c->setMaximum(ui->s_taxon_number->value());
}

void Settings::slotPlayingFieldNumberChanged()
{
    if(ui->s_multiple->value()==1)
    {
        ui->r_identical->setEnabled(false);
        ui->r_independent->setEnabled(false);
        ui->r_start->setEnabled(false);
    }
   else
    {
        ui->r_identical->setEnabled(true);
        ui->r_independent->setEnabled(true);
        ui->r_start->setEnabled(true);
    }
}


Settings::~Settings()
{
    delete ui;
}
