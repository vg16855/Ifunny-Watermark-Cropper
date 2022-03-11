#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "cropmenu.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Ifunny Watermark Cropper");
    crop = new cropMenu();
    connect(crop, &cropMenu::firstWindow, this, &MainWindow::show);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    //Choose Images
    crop->show();
    this->close();

}


void MainWindow::on_pushButton_2_clicked()
{
    //Choose Folder
    crop->show();
    this->close();
}

