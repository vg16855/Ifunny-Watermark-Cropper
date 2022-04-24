#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>

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


    void on_comparisonComboBox_currentIndexChanged();

    void on_histSpinBox_valueChanged();

    void on_histDefault_clicked();

    void on_normSpinBox_valueChanged();

    void on_normDefault_clicked();

    void on_dialogButton_accepted();

    void on_dialogButton_rejected();

private:
    Ui::settings *ui;
};

#endif // SETTINGS_H
