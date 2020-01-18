#include "settings.h"
#include "ui_settings.h"
#include "mainwindow.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    resize_grid=false;

    ui->setupUi(this);
    setWindowTitle("Simulation Settings");
    setWindowIcon(QIcon ("://resources/icon.png"));

    //Organism tab set up
    //Connect slots for on the fly maxima
    QObject::connect(ui->s_genome_size, SIGNAL(valueChanged(int)), this, SLOT(s_genome_size_changed()));
    QObject::connect(ui->s_taxon_number,SIGNAL(valueChanged(int)), this, SLOT(s_taxon_number_changed()));

    ui->s_genome_size->setValue(genome_size);
    ui->s_taxon_number->setValue(taxon_number);
    ui->s_species_difference->setValue(species_difference);
    ui->s_organism_mutation->setValue(organism_mutation);
    ui->s_unresolvable_c->setValue(unresolvable_cutoff);

    ui->c_strip_uninformative->setChecked(strip_uninformative);
    ui->c_sansomian->setChecked(sansomian);
    ui->c_beneficial_mut->setChecked(no_deleterious);

    //Set sensible maxima
    int max=genome_size;
    ui->s_species_difference->setMaximum(max);

    recalc_strip_unin_factor=false;

    //Environment tab set up

    //Set values
    ui->s_environment_mutation->setValue(environment_mutation);
    ui->s_playingfield_size->setValue(pfield_size);
    ui->s_masks_per_environment->setValue(mask_number);
}


void Settings::on_buttonBox_accepted()
{
    //Sort out GUI if required
    if (taxon_number!=ui->s_taxon_number->value()||genome_size!= ui->s_genome_size->value())resize_grid=true;
    recalc_strip_unin_factor=ui->c_recalc->isChecked();

    //Modify variables - organism tab
    genome_size=ui->s_genome_size->value();
    taxon_number=ui->s_taxon_number->value();
    species_difference=ui->s_species_difference->value();
    organism_mutation=ui->s_organism_mutation->value();
    unresolvable_cutoff=ui->s_unresolvable_c->value();

    strip_uninformative=ui->c_strip_uninformative->isChecked();
    sansomian=ui->c_sansomian->isChecked();
    no_deleterious=ui->c_beneficial_mut->isChecked();

    //Modify variables - environment tab
    pfield_size=ui->s_playingfield_size->value();
    mask_number=ui->s_masks_per_environment->value();
    environment_mutation=ui->s_environment_mutation->value();
}

void Settings::s_genome_size_changed()
{
//Set sensible maxima on the fly
ui->s_species_difference->setMaximum(ui->s_genome_size->value());
ui->s_species_difference->setValue(ui->s_genome_size->value()/10);
}

void Settings::s_taxon_number_changed()
{
    ui->s_unresolvable_c->setMaximum(ui->s_taxon_number->value());
}

Settings::~Settings()
{
    delete ui;
}
