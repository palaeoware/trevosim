#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "simulation_variables.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr, simulationVariables *simSettings = nullptr);
    ~Settings();

    simulationVariables *settings;

private slots:
    void on_buttonBox_accepted();
    void slotGenomeSizeChanged();
    void slotSelectSizeChanged();
    void slotFitnessSizeChanged();
    void slotEnvironmentNumberChanged();
    void slotrunForTaxaChanged();
    void slotMixingChanged();
    void slotMixingProbabilityChanged();
    void slotStochasticChanged();
    void slotPlayingfieldNumberChanged();
    void slotExpandingPlayingfieldChanged();
    void slotRandomOverwriteChanged();
    void slotEngineersChanged();
    void slotEngineersRadioClicked();
    void docs1();
    void docs2();


private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
