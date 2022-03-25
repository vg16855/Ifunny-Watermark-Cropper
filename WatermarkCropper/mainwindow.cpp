#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "cropmenu.h"
#include "settings.h"

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
                                                         "Images (*.png *.webp *.jpeg *.jfif *.jpg *.tif *.tiff *.tga *.bmp *.JPG *.PNG)");
    crop->loadImages(fileList);
    crop->show();
    this->close();
}


void MainWindow::on_chooseFolder_clicked()
{
    openCropMenu();
    //Choose Folder
    QDir directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    std::cout << "Turning directory into images" << std::endl;

    QStringList list = directory.entryList(QDir::Files);
    QStringList images = QStringList(list.size());
    for (int i = 1; i < list.size(); i++)
    {
        qDebug() << "Filename " << i << " = " << directory.filePath(list.at(i));
        images[i] = directory.filePath(list.at(i));
    }
    crop->loadImages(images);
    crop->show();
    this->close();
}


void MainWindow::on_actionOptions_triggered()
{
    settings *setting = new settings();
    setting->show();
}

