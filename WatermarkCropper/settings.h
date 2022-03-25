#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class settings;
}

class settings : public QDialog
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = nullptr);
    ~settings();

private slots:
    void on_bhattacharyyaSpinBox_valueChanged(double arg1);

    void on_intersectionSpinBox_valueChanged(double arg1);

    void on_chiSquareSpinBox_valueChanged(double arg1);

    void on_correlationSpinBox_valueChanged(double arg1);

    void on_histogramComboBox_currentIndexChanged(int index);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::settings *ui;
};

#endif // SETTINGS_H
