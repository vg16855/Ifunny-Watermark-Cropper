#include <iostream>
#include "settings.h"
#include "ui_settings.h"

settings::settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
}

double histogramThreshold = .5;
double matrixNormthreshold = 1;

settings::~settings()
{
    delete ui;
}



void settings::on_comparisonComboBox_currentIndexChanged(int index)
{
    std::cout << "Combobox Changed" << std::endl;
}


void settings::on_histSpinBox_valueChanged(double arg1)
{

}


void settings::on_histDefault_clicked()
{

}


void settings::on_normSpinBox_valueChanged(double arg1)
{

}


void settings::on_normDefault_clicked()
{

}


void settings::on_dialogButton_accepted()
{

}


void settings::on_dialogButton_rejected()
{

}

