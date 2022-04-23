#include <iostream>
#include "settings.h"
#include "ui_settings.h"

const double DEFAULT_HIST_THRESH = .7;
const double DEFAULT_NORM_THRESH = 1;

settings::settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);

    QFont text = QFont("Lato", 12, QFont::Bold);
    ui->compareLabel->setFont(text);
    ui->comparisonComboBox->setFont(text);
    ui->histLabel->setFont(text);
    ui->normLabel->setFont(text);
    ui->histSpinBox->setFont(text);
    ui->normSpinBox->setFont(text);
    ui->histDefault->setFont(text);
    ui->normDefault->setFont(text);
    ui->dialogButton->setFont(text);

    //Initialize the saved values for the settings
    QSettings settings;
    std::cout << "Settings Filepath: " << settings.fileName().toStdString() << std::endl;

    if(settings.contains("thresh/hist")){
        ui->histSpinBox->setValue(settings.value("thresh/hist").toDouble());
    }
    else{
        ui->histSpinBox->setValue(DEFAULT_HIST_THRESH);
    }
    if(settings.contains("thresh/norm")){
        ui->normSpinBox->setValue(settings.value("thresh/norm").toDouble());
    }
    else{
        ui->normSpinBox->setValue(DEFAULT_NORM_THRESH);
    }
    if(settings.contains("compare/algo")){
        ui->comparisonComboBox->setCurrentIndex(settings.value("compare/algo").toInt());
    }
    else{
        ui->comparisonComboBox->setCurrentIndex(0);
    }
}


settings::~settings()
{
    delete ui;
}


//When the "Comparison Method" combo box gets changed
void settings::on_comparisonComboBox_currentIndexChanged(int index)
{
    std::cout << "Combobox Changed" << std::endl;
    std::cout << "Combobox Value: " << ui->comparisonComboBox->currentIndex() << std::endl;
}

//When the "Hist Threshold" spin box gets changed
void settings::on_histSpinBox_valueChanged(double arg1)
{
    std::cout << "Histogram Spinbox Changed" << std::endl;
    std::cout << "Histogram Spinbox Value: " << ui->histSpinBox->value() << std::endl;

}

//Resets the value of the histogram to the default value of .7
void settings::on_histDefault_clicked()
{
    std::cout << "Histogram Default Button Clicked" << std::endl;
    ui->histSpinBox->setValue(DEFAULT_HIST_THRESH);
}

//When the "Norm Threshold" spin box gets changed
void settings::on_normSpinBox_valueChanged(double arg1)
{
    std::cout << "Normalization Spinbox Changed" << std::endl;
    std::cout << "Normalization Spinbox Value: " << ui->normSpinBox->value() << std::endl;
}

//Resets the value of the normalization to the default value of 1
void settings::on_normDefault_clicked()
{
    std::cout << "Normalization Default Button Clicked" << std::endl;
    ui->normSpinBox->setValue(DEFAULT_NORM_THRESH);
}

//Only here do the settings actually get implemented AND saved
void settings::on_dialogButton_accepted()
{
    std::cout << "Settings Saved" << std::endl;

    //Save all the settings
    QSettings settings;
    settings.setValue("compare/algo", ui->comparisonComboBox->currentIndex());
    settings.setValue("thresh/hist", ui->histSpinBox->value());
    settings.setValue("thresh/norm", ui->normSpinBox->value());

    std::cout << "Comparison Combo Box Saved Value: " << settings.value("compare/algo").toInt() << std::endl;
    std::cout << "Histogram Spinbox Saved Value: " << settings.value("thresh/hist").toDouble() << std::endl;
    std::cout << "Normalization Spinbox Saved Value: " << settings.value("thresh/norm").toDouble() << std::endl;
}

//Cancels all changes and resets to what they were when the settings menu was first started
void settings::on_dialogButton_rejected()
{
    std::cout << "Settings Cancelled" << std::endl;

    QSettings settings;
    ui->comparisonComboBox->setCurrentIndex(settings.value("compare/algo").toInt());
    ui->histSpinBox->setValue(settings.value("thresh/hist").toDouble());
    ui->normSpinBox->setValue(settings.value("thresh/norm").toDouble());
}

