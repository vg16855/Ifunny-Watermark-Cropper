#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "cropmenu.h"
#include "settings.h"
#include <QStandardPaths>
#include <QMessageBox>
#include <QFileDialog>

const QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

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
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select one or more files to open",
                                                         defaultPath,
                                                         "Images (*.png *.webp *.jpeg *.jfif *.jpg *.tif *.tiff *.tga *.bmp *.JPG *.PNG)");
    if(fileList.empty()){
        return;
    }
    crop->loadImages(fileList);
    crop->show();

}


void MainWindow::on_chooseFolder_clicked()
{
    openCropMenu();
    //Choose Folder
    QDir directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 defaultPath,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    //If user doesn't select folder
    if(directory.entryList().empty()){
        return;
    }

    QStringList list = directory.entryList(QDir::Files);
    QStringList images = QStringList(list.size());

    //If folder contains no images
    if(images.empty()){
            std::cout << "Empty Folder Given" << std::endl;
            return;
        }
    for (int i = 1; i < list.size(); i++)
    {
        qDebug() << "Filename " << i << " = " << directory.filePath(list.at(i));
        images[i] = directory.filePath(list.at(i));
    }
    crop->loadImages(images);
    crop->show();
}


void MainWindow::on_actionOptions_triggered()
{
    settings *setting = new settings();
    setting->show();
}


void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox info;
    info.setText("This app was made by two brothers who were sick of "
                 "ifunny putting stupid watermarks on everything");
    info.setIcon(QMessageBox::Information);
    info.setWindowTitle("About");
    info.exec();
}

