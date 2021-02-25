#include "output.h"
#include "ui_output.h"

#include "mainwindow.h"

Output::Output(QWidget *parent, simulationVariables *simSettings) :
    QDialog(parent),
    ui(new Ui::Output)
{
    settings = simSettings;
    ui->setupUi(this);
    setWindowTitle("Output settings");
    setWindowIcon(QIcon ("://resources/icon.png"));
    QFont font;
    font.setWeight( QFont::Bold );
    ui->output_title->setFont(font);

    ui->intro->setAlignment(Qt::AlignJustify);

    ui->label_f1->setFont(font);
    ui->label_f2->setFont(font);

    ui->intro->setWordWrap(true);
    ui->intro->setText("Using this output options dialogue you can set the text for up to two output files with the matrix and, if you so wish, the tree as well. You can also select to have the tree saved as a .nex file. In order to write the matrix into the file (tab-delineated, with taxon names Species_0 .... Species_n) type ||Matrix|| (||Stochastic_Matrix|| will output the stochastic character matrix). Similarly, to insert a tree TNT format, type ||TNT_Tree||, and for MrBayes ||MrBayes_Tree|| (this is a newick format, with branch lengths). Other useful things: ||Time|| will add a timestamp; ||Settings|| will write the settings to the file (comment out yourself as required); ||Character_Number|| will write character number; ||Taxon_Number|| will write taxon number; ||Count|| will add a counter for batch runs (starting from zero); ||Unresolvable|| will add a list of unresolvable taxa (if there are any, or a message confirming there are none if not); ||Identical|| will give a plain count of these; ||Alive_Record|| will print out a record of what was alive, when; and ||Uninformative|| will output the number of uninformative characters. Any issues, questions, unexpected behaviour, or seething resentment email RJG.\n");

    ui->file_01_base->setText(settings->logFileNameBase01);
    ui->file_01_extension->setText(settings->logFileExtension01);
    ui->string_f1->insertPlainText(settings->logFileString01);
    ui->string_f1->moveCursor (QTextCursor::Start) ;
    ui->string_f1->ensureCursorVisible() ;

    ui->file_02_base->setText(settings->logFileNameBase02);
    ui->file_02_extension->setText(settings->logFileExtension02);
    ui->string_f2->insertPlainText(settings->logFileString02);
    ui->string_f2->moveCursor (QTextCursor::Start) ;
    ui->string_f2->ensureCursorVisible() ;

    ui->c_append->setChecked(settings->append);
    ui->c_write_tree->setChecked(settings->writeTree);
    ui->c_species_curve->setChecked(settings->speciesCurve);
    ui->c_work_log->setChecked(settings->workingLog);

}

void Output::on_buttonBox_accepted()
{
    settings->append = ui->c_append->isChecked();
    settings->writeTree = ui->c_write_tree->isChecked();
    settings->speciesCurve = ui->c_species_curve->isChecked();
    settings->workingLog = ui->c_work_log->isChecked();

    if (settings->workingLog)QMessageBox::warning(nullptr, "Heads up",
                                                      "Work log outputs text every operation the programme performs, allowing its behaviour to be validated. Be aware that this creates very large text files, very quickly. You may want to run your tests with limited taxon and character numbers.");

    settings->logFileNameBase01 = ui->file_01_base->text();
    settings->logFileNameBase01.replace(" ", "_");
    QString user_ext = ui->file_01_extension->text();
    if (user_ext.at(0) != QChar('.') || user_ext.length() != 4) QMessageBox::warning(nullptr, "Oops", "I don't think extension 1 is valid. If it is, email RJG and he can sort.");
    else
    {
        settings->logFileExtension01 = user_ext;
        settings->logFileExtension01.replace(" ", "");
    }

    settings->logFileString01 = ui->string_f1->toPlainText();

    settings->logFileNameBase02 = ui->file_02_base->text();
    settings->logFileNameBase02.replace(" ", "_");

    user_ext = ui->file_02_extension->text();
    if (user_ext.at(0) != QChar('.') || user_ext.length() != 4) QMessageBox::warning(nullptr, "Oops", "I don't think extension 2 is valid. If it is, email RJG and he can sort.");
    else
    {
        settings->logFileExtension02 = user_ext;
        settings->logFileExtension02.replace(" ", "");
    }

    settings->logFileString02 = ui->string_f2->toPlainText();
}

Output::~Output()
{
    delete ui;
}
