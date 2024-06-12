#include "settings.h"
#include "ui_settings.h"
#include "mainwindow.h"

#include <QFormLayout>
#include <QComboBox>

Settings::Settings(QWidget *parent, simulationVariables *simSettings) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    settings = simSettings;

    ui->setupUi(this);
    setWindowTitle("Simulation Settings");
    setWindowIcon(QIcon ("://resources/icon.png"));

    //Organism tab set up
    //Connect slots for on the fly maxima
    QObject::connect(ui->s_genome_size, QOverload<int>::of(&QSpinBox::valueChanged), this, &Settings::slotGenomeSizeChanged);
    QObject::connect(ui->s_genome_size, QOverload<int>::of(&QSpinBox::valueChanged), this, &Settings::slotGenomeSizeChanged);
    QObject::connect(ui->s_select_size, QOverload<int>::of(&QSpinBox::valueChanged), this, &Settings::slotSelectSizeChanged);
    QObject::connect(ui->s_fitness_size, QOverload<int>::of(&QSpinBox::valueChanged), this, &Settings::slotFitnessSizeChanged);
    QObject::connect(ui->s_taxon_number, QOverload<int>::of(&QSpinBox::valueChanged), this, &Settings::slotrunForTaxaChanged);
    QObject::connect(ui->s_multiple, QOverload<int>::of(&QSpinBox::valueChanged), this, &Settings::slotPlayingfieldNumberChanged);
    QObject::connect(ui->c_expanding_playingfield, &QCheckBox::stateChanged, this, &Settings::slotExpandingPlayingfieldChanged);
    QObject::connect(ui->c_random_overwrite, &QCheckBox::stateChanged, this, &Settings::slotRandomOverwriteChanged);
    QObject::connect(ui->c_ecosystem_engineers, &QCheckBox::stateChanged, this, &Settings::slotEngineersChanged);
    QObject::connect(ui->c_stochastic, &QCheckBox::stateChanged, this, &Settings::slotStochasticChanged);
    QObject::connect(ui->button_group_once_pers, &QButtonGroup::buttonClicked, this, &Settings::slotEngineersRadioClicked);

    ui->s_genome_size->setValue(settings->genomeSize);
    ui->s_select_size->setValue(settings->speciesSelectSize);
    ui->s_fitness_size->setValue(settings->fitnessSize);
    ui->s_fitness_target->setValue(settings->fitnessTarget);
    ui->s_taxon_number->setValue(settings->runForTaxa);
    ui->s_run_for->setValue(settings->runForIterations);
    ui->s_species_difference->setValue(settings->speciesDifference);
    ui->s_organism_mutation->setValue(settings->organismMutationRate);
    ui->s_unresolvable_c->setValue(settings->unresolvableCutoff);
    ui->s_selection->setValue(settings->selectionCoinToss);
    ui->s_stochasticDepth->setValue(settings->stochasticDepth);
    ui->s_EE_frequency->setValue(settings->ecosystemEngineeringFrequency);

    ui->c_strip_uninformative->setChecked(settings->stripUninformative);
    ui->c_sansomian->setChecked(settings->sansomianSpeciation);
    ui->c_beneficial_mut->setChecked(settings->discardDeleterious);
    ui->c_extinction->setChecked(settings->environmentalPerturbation);
    ui->c_mixing_perturbation->setChecked(settings->mixingPerturbation);
    ui->c_noSelection->setChecked(settings->noSelection);
    ui->c_random_overwrite->setChecked(settings->randomOverwrite);
    ui->c_stochastic->setChecked(settings->stochasticLayer);
    ui->c_expanding_playingfield->setChecked(settings->expandingPlayingfield);
    ui->c_match_fitness_peaks->setChecked(settings->matchFitnessPeaks);
    ui->c_ecosystem_engineers->setChecked(settings->ecosystemEngineers);

    //Set sensible maxima
    ui->s_select_size->setMaximum(settings->genomeSize);
    ui->s_fitness_size->setMaximum(settings->genomeSize);
    int max = settings->genomeSize;
    if (settings->speciesSelectSize < settings->genomeSize)max = settings->speciesSelectSize;
    ui->s_species_difference->setMaximum(max);
    ui->s_fitness_target->setMaximum(ui->s_fitness_size->value()*settings->maskNumber);

    //Environment tab set up
    //Slots
    QObject::connect(ui->c_mixing, &QCheckBox::stateChanged, this, &Settings::slotMixingChanged);

    //Set values
    ui->s_environment_mutation->setValue(settings->environmentMutationRate);
    ui->s_playingfield_size->setValue(settings->playingfieldSize);
    ui->s_environment_number->setValue(settings->environmentNumber);
    ui->s_masks_per_environment->setValue(settings->maskNumber);
    ui->c_mixing->setChecked(settings->mixing);
    ui->s_mixing_1_to_0->setValue(settings->mixingProbabilityOneToZero);
    ui->s_mixing_0_to_1->setValue(settings->mixingProbabilityZeroToOne);
    //Connect this here so as not to mess with above.
    QObject::connect(ui->s_mixing_1_to_0, QOverload<int>::of(&QSpinBox::valueChanged), this, &Settings::slotMixingProbabilityChanged);

    if (settings->playingfieldMasksMode == MASKS_MODE_IDENTICAL)ui->r_identical->setChecked(true);
    else if (settings->playingfieldMasksMode == MASKS_MODE_INDEPENDENT)ui->r_independent->setChecked(true);
    else ui->r_start->setChecked(true);

    if (settings->runMode == RUN_MODE_TAXON)ui->r_taxon_number->setChecked(true);
    else if (settings->runMode == RUN_MODE_ITERATION)ui->r_iterations->setChecked(true);

    if (settings->ecosystemEngineersArePersistent)ui->r_persistent_EE->setChecked(true);
    else ui->r_once_EE->setChecked(true);
    //Sort out GUI on load
    slotEngineersChanged();
    slotEngineersRadioClicked();
    slotPlayingfieldNumberChanged();
    if (settings->ecosystemEngineersAddMask)ui->r_add_mask->setChecked(true);
    else ui->r_overwrite_mask->setChecked(true);

    if (ui->c_expanding_playingfield->isChecked())ui->s_playingfield_size->setValue(ui->s_taxon_number->value());

    //Force update so the slot deals with ensuring correct options are enabled/disabled
    ui->c_mixing->setChecked(true);
    if (settings->mixing)ui->c_mixing->setChecked(true);
    else ui->c_mixing->setChecked(false);

    ui->s_multiple->setValue(settings->playingfieldNumber);
}


void Settings::on_buttonBox_accepted()
{
    //Modify variables - organism tab
    settings->genomeSize = ui->s_genome_size->value();
    settings->speciesSelectSize = ui->s_select_size->value();
    settings->fitnessSize = ui->s_fitness_size->value();
    settings->fitnessTarget = ui->s_fitness_target->value();
    settings->runForTaxa = ui->s_taxon_number->value();
    settings->runForIterations = ui->s_run_for->value();
    settings->speciesDifference = ui->s_species_difference->value();
    settings->organismMutationRate = ui->s_organism_mutation->value();
    settings->unresolvableCutoff = ui->s_unresolvable_c->value();
    settings->selectionCoinToss = ui->s_selection->value();
    settings->stochasticDepth = ui->s_stochasticDepth->value();
    settings->ecosystemEngineeringFrequency = ui->s_EE_frequency->value();

    settings->stripUninformative = ui->c_strip_uninformative->isChecked();
    settings->sansomianSpeciation = ui->c_sansomian->isChecked();
    settings->discardDeleterious = ui->c_beneficial_mut->isChecked();
    settings->environmentalPerturbation = ui->c_extinction->isChecked();
    settings->mixing = ui->c_mixing->isChecked();
    settings->mixingPerturbation = ui->c_mixing->isChecked();
    settings->noSelection = ui->c_noSelection->isChecked();
    settings->randomOverwrite = ui->c_random_overwrite->isChecked();
    settings->stochasticLayer = ui->c_stochastic->isChecked();
    settings->expandingPlayingfield = ui->c_expanding_playingfield->isChecked();
    settings->matchFitnessPeaks = ui->c_match_fitness_peaks->isChecked();
    settings->ecosystemEngineers = ui->c_ecosystem_engineers->isChecked();

    //Modify variables - environment tab
    settings->playingfieldSize = ui->s_playingfield_size->value();
    settings->environmentNumber = ui->s_environment_number->value();
    settings->maskNumber = ui->s_masks_per_environment->value();
    settings->environmentMutationRate = ui->s_environment_mutation->value();
    settings->mixingProbabilityOneToZero = ui->s_mixing_1_to_0->value();
    settings->mixingProbabilityZeroToOne = ui->s_mixing_0_to_1->value();
    settings->playingfieldNumber = ui->s_multiple->value();
    settings->ecosystemEngineersAddMask = ui->r_add_mask->isChecked();

    if (ui->r_identical->isChecked())settings->playingfieldMasksMode = MASKS_MODE_IDENTICAL;
    else if (ui->r_independent->isChecked())settings->playingfieldMasksMode = MASKS_MODE_INDEPENDENT;
    else settings->playingfieldMasksMode = MASKS_MODE_IDENTICAL_START;

    if (ui->r_taxon_number->isChecked())settings->runMode = RUN_MODE_TAXON;
    else if (ui->r_iterations->isChecked())settings->runMode = RUN_MODE_ITERATION;

    if (ui->r_once_EE->isChecked())settings->ecosystemEngineersArePersistent = false;
    else settings->ecosystemEngineersArePersistent = true;

    if (ui->c_stochastic->isChecked())
    {
        QDialog stochasticDialog(this);
        stochasticDialog.setWindowTitle("TRevoSim Stochastic Mapping");
        stochasticDialog.setMinimumSize(275, 400);
        //Form layout allows labels for each int - organised in rows
        QFormLayout stochasticForm(&stochasticDialog);
        QLabel lab("Please enter the mapping for the stochastic layer:");
        lab.setWordWrap(true);
        stochasticForm.addRow(&lab);

        quint16 lookups[4];
        lookups[0] = 1;
        for (int i = 1; i < 4; i++)lookups[i] = lookups[i - 1] * 2;

        // Add the lineEdits with their respective labels
        QList<QComboBox *> maps;
        for (int i = 0; i < 16; ++i)
        {
            QComboBox *combo = new QComboBox(&stochasticDialog);
            combo->setMaximumWidth(150);
            combo->addItem("0");
            combo->addItem("1");
            combo->setCurrentIndex(settings->stochasticMap[i]);
            QString label = QString("%1 = ").arg(i);

            //Create bits from number
            QString bits;
            QTextStream out(&bits);
            for (int j = 0; j < 4; j++)
                if (lookups[j] & i) out << "1";
                else out << "0";

            label.append(bits);
            label.append("   ");
            stochasticForm.addRow(label, combo);
            maps << combo;
        }

        // Add standard buttons
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &stochasticDialog);
        stochasticForm.addRow(&buttonBox);
        QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &stochasticDialog, &Settings::accept);
        QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &stochasticDialog, &Settings::reject);

        stochasticForm.setFormAlignment(Qt::AlignCenter);
        stochasticForm.setAlignment(Qt::AlignCenter);

        // Show the dialog as modal
        if (stochasticDialog.exec() == QDialog::Accepted)
        {
            int count = 0;
            //If the user didn't dismiss the dialog, do something with the fields
            foreach (QComboBox *combo, maps)
            {
                settings->stochasticMap[count] = combo->currentIndex();
                count++;
            }
        }

        foreach (QComboBox * combo, maps) delete combo;
    }
}

void Settings::slotGenomeSizeChanged()
{
    //Set sensible maxima on the fly
    ui->s_select_size->setMaximum(ui->s_genome_size->value());
    ui->s_fitness_size->setMaximum(ui->s_genome_size->value());
    ui->s_species_difference->setMaximum(ui->s_genome_size->value());
    ui->s_fitness_target->setMaximum(ui->s_genome_size->value()*settings->maskNumber);

    //Adjust values
    ui->s_select_size->setValue(ui->s_genome_size->value());
    ui->s_fitness_size->setValue(ui->s_genome_size->value());
    ui->s_species_difference->setValue(ui->s_genome_size->value() / 10);
}

//Set sensible maxima on the fly
void Settings::slotMixingProbabilityChanged()
{
    ui->s_mixing_0_to_1->setValue(ui->s_mixing_1_to_0->value());
}

//Set sensible maxima on the fly
void Settings::slotSelectSizeChanged()
{
    ui->s_species_difference->setMaximum(ui->s_select_size->value());
}


void Settings::slotrunForTaxaChanged()
{
    ui->s_unresolvable_c->setMaximum(ui->s_taxon_number->value());
    if (ui->c_expanding_playingfield->isChecked())ui->s_playingfield_size->setValue(ui->s_taxon_number->value());
}

void Settings::slotFitnessSizeChanged()
{
    ui->s_fitness_target->setMaximum(ui->s_fitness_size->value()*settings->maskNumber);
}

//Sort GUI

void Settings::slotMixingChanged()
{
    if (ui->c_mixing->isChecked())
    {
        ui->s_mixing_1_to_0->setEnabled(true);
        ui->c_mixing_perturbation->setEnabled(true);
        ui->label_15->setEnabled(true);
        if (ui->s_multiple->value() == 2)
        {
            ui->label_21->setEnabled(true);
            ui->s_mixing_0_to_1->setEnabled(true);
        }
    }
    else
    {
        ui->s_mixing_0_to_1->setEnabled(false);
        ui->s_mixing_1_to_0->setEnabled(false);
        ui->c_mixing_perturbation->setEnabled(false);
        ui->c_mixing_perturbation->setChecked(false);
        ui->label_15->setEnabled(false);
        ui->label_21->setEnabled(false);
    }
}

void Settings::slotPlayingfieldNumberChanged()
{
    //We want to allow asymmetrical mixing with two playing fields, otherwise just have a set possibility
    if (ui->s_multiple->value() > 2)
    {
        ui->label_15->setText("Probability of mixing");
        ui->s_mixing_0_to_1->setEnabled(false);
        ui->label_21->setEnabled(false);
    }
    else if (ui->s_multiple->value() == 2)
    {
        ui->label_15->setText("Probability of mixing - PF2 to PF1");
        if (ui->c_mixing->isChecked())
        {
            ui->s_mixing_0_to_1->setEnabled(true);
            ui->label_21->setEnabled(true);
        }
    }

    //We want to offer options for the masks if we have more than one playing field, and for mixing, etc.
    if (ui->s_multiple->value() == 1)
    {
        ui->r_identical->setEnabled(false);
        ui->r_independent->setEnabled(false);
        ui->r_start->setEnabled(false);
        ui->c_mixing->setEnabled(false);
        ui->c_mixing->setChecked(false);
        //ui->s_mixing_0_to_1->setEnabled(false);
        //ui->s_mixing_1_to_0->setEnabled(false);
        //ui->label_15->setEnabled(false);
        //ui->label_21->setEnabled(false);
    }
    else
    {
        ui->r_identical->setEnabled(true);
        ui->r_independent->setEnabled(true);
        ui->r_start->setEnabled(true);
        ui->c_mixing->setEnabled(true);
        //ui->label_15->setEnabled(true);
        //ui->s_mixing_1_to_0->setEnabled(true);
    }
}

void Settings::slotStochasticChanged()
{
    if (ui->c_stochastic->isChecked())
    {
        ui->s_select_size->setEnabled(false);
        ui->s_select_size->setValue(ui->s_genome_size->value());
        ui->s_fitness_size->setEnabled(false);
        ui->s_fitness_size->setValue(ui->s_genome_size->value());
        ui->c_strip_uninformative->setChecked(false);
        ui->c_strip_uninformative->setEnabled(false);
    }
    else
    {
        ui->s_select_size->setEnabled(true);
        ui->s_fitness_size->setEnabled(true);
        ui->c_strip_uninformative->setEnabled(true);
    }
}

void Settings::slotExpandingPlayingfieldChanged()
{
    if (ui->c_expanding_playingfield->isChecked())
    {
        ui->s_playingfield_size->setEnabled(false);
        ui->s_playingfield_size->setValue(ui->s_taxon_number->value());
        if (ui->c_random_overwrite->isChecked())ui->c_random_overwrite->setChecked(false);
    }
    else ui->s_playingfield_size->setEnabled(true);

}

void Settings::slotRandomOverwriteChanged()
{
    if (ui->c_random_overwrite->isChecked() && ui->c_expanding_playingfield->isChecked())ui->c_expanding_playingfield->setChecked(false);
}

void Settings::slotEngineersChanged()
{
    if (ui->c_ecosystem_engineers->isChecked())
    {
        ui->r_once_EE->setEnabled(true);
        ui->r_persistent_EE->setEnabled(true);
        ui->r_add_mask->setEnabled(true);
        ui->r_overwrite_mask->setEnabled(true);
        ui->s_EE_frequency->setEnabled(true);
        ui->EE_Label->setEnabled(true);
        ui->EE_Label_2->setEnabled(true);
        ui->EE_Label_3->setEnabled(true);

    }
    else
    {
        ui->r_once_EE->setEnabled(false);
        ui->r_persistent_EE->setEnabled(false);
        ui->r_add_mask->setEnabled(false);
        ui->r_overwrite_mask->setEnabled(false);
        ui->s_EE_frequency->setEnabled(false);
        ui->EE_Label->setEnabled(false);
        ui->EE_Label_2->setEnabled(false);
        ui->EE_Label_3->setEnabled(false);
    }
}


void Settings::slotEngineersRadioClicked()
{
    if (ui->r_once_EE->isChecked())
    {
        ui->s_EE_frequency->setEnabled(false);
        ui->EE_Label->setEnabled(false);
    }
    else
    {
        ui->s_EE_frequency->setEnabled(true);
        ui->EE_Label->setEnabled(true);
    }
}

Settings::~Settings()
{
    delete ui;
}
