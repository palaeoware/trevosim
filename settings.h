#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();
    bool resize_grid;
    bool recalc_strip_unin_factor;

private slots:
    void on_buttonBox_accepted();
    void s_genome_size_changed();
    void s_taxon_number_changed();

private:
    Ui::Settings *ui;

};

#endif // SETTINGS_H
