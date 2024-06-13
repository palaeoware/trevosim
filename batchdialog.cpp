#include "batchdialog.h"
#include "ui_batchdialog.h"
#include <QDebug>

batchDialog::batchDialog(QWidget *parent, int *runForConstructor, int defaultValue):
    QDialog(parent),
    ui(new Ui::batchDialog)
{
    ui->setupUi(this);
    QFont font;
    font.setWeight( QFont::Bold );
    ui->batch_title->setFont(font);
    ui->batch_title->setText("How many replicates would you like to run?");

    QString label =
        QString(" TREvoSim will run the first replicate in serial."
                " This allows it to update the display, and provide warnings if anything goes wrong."
                " If that first run completes successfully, it will then run the remaining replicates in parallel."
                " This will not update the GUI."
                " TREvoSim will then repeat any replicate that fails until all have completed."
                " You may need to click cancel if the number of failed runs does not decrease with time.");
    ui->batch_description->setText(label);
    ui->batch_description->setAlignment(Qt::AlignJustify);

    runFor = runForConstructor;
}


void batchDialog::on_buttonBox_accepted()
{
    *runFor = ui->runForSpinBox->value();
}

batchDialog::~batchDialog()
{
    delete ui;
}
