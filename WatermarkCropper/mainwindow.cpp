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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openCropMenu(){
    crop = new cropMenu();
    crop->setAttribute(Qt::WA_DeleteOnClose);
    connect(crop, &cropMenu::firstWindow, this, &MainWindow::show);
}

void MainWindow::on_pushButton_clicked()
{
    //Choose Images
    openCropMenu();
    QStringList fileList = QFileDialog::getOpenFileNames();
    crop->loadImages(fileList);
    crop->show();
    this->close();

}


void MainWindow::on_pushButton_2_clicked()
{
    //Choose Folder
    crop->show();
    this->close();
}

