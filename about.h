#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = 0);
    ~About();

private:
    Ui::About *ui;
    QString returnLicense();
};

#endif // ABOUT_H
