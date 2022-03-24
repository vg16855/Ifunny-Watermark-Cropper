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


void MainWindow::on_chooseImages_clicked()
{
    //Choose Images
    openCropMenu();
    QStringList fileList = QFileDialog::getOpenFileNames(this,
                                                         "Select one or more files to open",
                                                         "/home",
                                                         "Images (*.png *.webp *.jpeg *.jfif *.jpg"
                                                         "*.tif *.tiff *tga *.bmp*)");
    crop->loadImages(fileList);
    crop->show();
    this->close();
}


void MainWindow::on_chooseFolder_clicked()
{
    //Choose Folder
    crop->show();
    this->close();
}

