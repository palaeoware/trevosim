#ifndef BATCHDIALOG_H
#define BATCHDIALOG_H

#include <QDialog>

namespace Ui {
class batchDialog;
}

class batchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit batchDialog(QWidget *parent, int *runForConstructor, int defaultValue);
    ~batchDialog();

private:
    Ui::batchDialog *ui;
    int *runFor;

private slots:
    void on_buttonBox_accepted();
};

#endif // BATCHDIALOG_H
