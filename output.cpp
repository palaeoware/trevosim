#include "output.h"
#include "ui_output.h"
#include "version.h"

#include <QDesktopServices>
#include <QMessageBox>


Output::Output(QWidget *parent, simulationVariables *simSettings) :
    QDialog(parent),
    ui(new Ui::Output)
{

    settings = simSettings;
    ui->setupUi(this);
    setWindowTitle("Output settings");
    setWindowIcon(QIcon ("://resources/icon.png"));

    //Sort out end of run outputs
    QFont font;
    font.setWeight( QFont::Bold );
    ui->output_title->setFont(font);
    ui->output_title->setText("End of run outputs");

    ui->intro_01->setAlignment(Qt::AlignJustify);

    ui->label_f1->setFont(font);
    ui->label_f2->setFont(font);
    ui->label_f3->setFont(font);

    ui->intro_01->setWordWrap(true);
    ui->intro_01->setText("This output dialogue allows you to set the text for up to two custom output files to reflect the state of a completed simulation. For example, you can output the matrix, the number of characters or terminals, and the tree (which can also be output as a separate file - see below). You can include commands for downstream analysis of the data. For more details of how to write outputs, please see the TREvoSim documentation (available by clicking the button below).");

    ui->intro_02->setWordWrap(true);
    ui->intro_02->setText("In addition to the two custom files above, this option allows you to save the tree as a .nex file. Provide a base name here.");

    QObject::connect(ui->docsPushButton, &QPushButton::clicked, this, &Output::docs);

    ui->file_01_base->setText(settings->logFileNameBase01);
    ui->file_01_extension->setText(settings->logFileExtension01);
    ui->string_f1->insertPlainText(settings->logFileString01);
    ui->string_f1->moveCursor (QTextCursor::Start) ;
    ui->string_f1->ensureCursorVisible();
    ui->c_write_file_01->setChecked(settings->writeFileOne);

    ui->file_02_base->setText(settings->logFileNameBase02);
    ui->file_02_extension->setText(settings->logFileExtension02);
    ui->string_f2->insertPlainText(settings->logFileString02);
    ui->string_f2->moveCursor (QTextCursor::Start) ;
    ui->string_f2->ensureCursorVisible();
    ui->c_write_file_02->setChecked(settings->writeFileTwo);

    ui->c_write_tree->setChecked(settings->writeTree);
    ui->file_03_base->setText(settings->logFileNameBase03);

    //Sort out running log outputs
    ui->output_title_2->setFont(font);
    ui->output_title_2->setText("Running log output");
    ui->intro_3->setAlignment(Qt::AlignJustify);
    ui->intro_3->setWordWrap(true);
    ui->intro_3->setText("This output dialogue allows you to set the text for a running log that reflects the state of a simulation as it runs. You can output, for example, the playing field, or the tree at a user-defined frequency. For more details of how to write outputs, please see the TREvoSim documentation (available by clicking the button below).");
    QObject::connect(ui->docsPushButton_2, &QPushButton::clicked, this, &Output::docs);

    ui->s_frequency->setValue(settings->runningLogFrequency);
    ui->c_write_running_log->setChecked(settings->writeRunningLog);
    ui->c_write_ee->setChecked(settings->writeEE);

    ui->running_log_body->insertPlainText(settings->runningLogString);

    ui->c_work_log->setChecked(settings->workingLog);
}

//make labels bold
//save file base name 03

void Output::on_buttonBox_accepted()
{
    settings->writeTree = ui->c_write_tree->isChecked();
    settings->workingLog = ui->c_work_log->isChecked();

    if (settings->workingLog)QMessageBox::warning(nullptr, "Heads up",
                                                      "Work log outputs text every operation the programme performs, allowing its behaviour to be validated. Be aware that this creates very large text files, very quickly. You may want to run your tests with limited taxon and character numbers.");

    settings->logFileNameBase01 = ui->file_01_base->text();
    settings->logFileNameBase01.replace(" ", "_");
    QString user_ext = ui->file_01_extension->text();
    if (user_ext.length() != 0)
    {
        if (user_ext.at(0) != QChar('.') || user_ext.length() != 4) QMessageBox::warning(nullptr, "Oops", "I don't think extension 1 is valid. If it is, email RJG and he can sort.");
        else
        {
            settings->logFileExtension01 = user_ext;
            settings->logFileExtension01.replace(" ", "");
        }
    }
    settings->logFileString01 = ui->string_f1->toPlainText();

    settings->logFileNameBase02 = ui->file_02_base->text();
    settings->logFileNameBase02.replace(" ", "_");

    user_ext = ui->file_02_extension->text();
    if (user_ext.length() != 0)
    {
        if (user_ext.at(0) != QChar('.') || user_ext.length() != 4) QMessageBox::warning(nullptr, "Oops", "I don't think extension 2 is valid. If it is, email RJG and he can sort.");
        else
        {
            settings->logFileExtension02 = user_ext;
            settings->logFileExtension02.replace(" ", "");
        }
    }

    settings->logFileString02 = ui->string_f2->toPlainText();

    settings->logFileNameBase03 = ui->file_03_base->text();
    settings->logFileNameBase03.replace(" ", "_");

    settings->writeEE = ui->c_write_ee->isChecked();
    settings->writeFileOne = ui->c_write_file_01->isChecked();
    settings->writeFileTwo = ui->c_write_file_02->isChecked();
    settings->writeRunningLog = ui->c_write_running_log->isChecked();

    settings->runningLogString = ui->running_log_body->toPlainText();

    settings->runningLogFrequency = ui->s_frequency->value();
}

void Output::docs()
{
    QDesktopServices::openUrl(QUrl(QString(DOCSURLLOG)));
}

Output::~Output()
{
    delete ui;
}
