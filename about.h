#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr, QString title = "TREvoSim");
    ~About();

private:
    Ui::About *ui;
    QString returnLicense();

private slots:
    void codeOnGitHub();
    void bugReport();
    void docs();
};

#endif // ABOUT_H
