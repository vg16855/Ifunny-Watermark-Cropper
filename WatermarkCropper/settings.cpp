#include <iostream>
#include "settings.h"
#include "ui_settings.h"

settings::settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
}

double correlationThreshold = .5;
double chiSquareThreshold;
double intersectionThreshold;
double bhattacharyyaThreshold;

settings::~settings()
{
    delete ui;
}

void settings::on_bhattacharyyaSpinBox_valueChanged(double arg1)
{
    bhattacharyyaThreshold = arg1;
}


void settings::on_intersectionSpinBox_valueChanged(double arg1)
{
    intersectionThreshold = arg1;
}


void settings::on_chiSquareSpinBox_valueChanged(double arg1)
{
    chiSquareThreshold = arg1;
}


void settings::on_correlationSpinBox_valueChanged(double arg1)
{
    correlationThreshold = arg1;
}


void settings::on_histogramComboBox_currentIndexChanged(int index)
{
    std::cout << "Index changed" << std::endl;
}


void settings::on_comboBox_2_currentIndexChanged(int index)
{
    std::cout << "Index changed" << std::endl;
}


void settings::on_buttonBox_accepted()
{

}


void settings::on_buttonBox_rejected()
{

}

