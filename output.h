#ifndef OUTPUT_H
#define OUTPUT_H

#include <QDialog>
#include <simulation_variables.h>

namespace Ui {
class Output;
}

class Output : public QDialog
{
    Q_OBJECT

public:
    explicit Output(QWidget *parent = nullptr, simulationVariables *simSettings = nullptr);
    simulationVariables *settings;
    ~Output();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Output *ui;
};

#endif // OUTPUT_H
