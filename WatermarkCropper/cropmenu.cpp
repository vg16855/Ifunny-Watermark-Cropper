#include "cropmenu.h"
#include "clickable.h"
#include "settings.h"
#include "ui_cropmenu.h"
#include <QDir>
#include <QLabel>
#include <QCheckBox>
#include <QProgressDialog>
#include <QFileInfo>
#include <QFileDialog>
#include <QString>
#include <QStandardPaths>
#include <QImageReader>
#include <QMessageBox>
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

const int HIST_INDEX = 0;
const int NORM_INDEX = 1;
const int FILE_PATH = 0;
const int CHECKED = 1;
const int IMAGE = 2;

int hBins = 180;
int sBins = 256;
int histSize[] = {hBins, sBins};
int numChecked = 0;

float hRanges[] = {0,180};
float sRanges[] = {0, 256};
const float* ranges[] = { hRanges, sRanges };
int channels[] = {0,1};
QString initialFilePath;


cropMenu::cropMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cropMenu)
{
    ui->setupUi(this);
    this->setWindowTitle("Cropping Menu");
    QFont text = QFont("Lato", 12, QFont::Bold);
    QFont smallText = QFont("Lato", 10);

    ui->cropNumber->setFont(text);
    ui->imageNumber->setFont(text);
    ui->filenameLabel->setFont(text);
    ui->save->setFont(smallText);
    ui->goBack->setFont(smallText);
    ui->saveAs->setFont(smallText);
}


cropMenu::~cropMenu()
{
    std::cout << "Deleting Images" << std::endl;
    int numItems = validImages.size();
    for(int i = 0; i < numItems; i++){
        QLayoutItem* item = ui->imageGridView->takeAt(0);
        delete item->widget();
        delete item;
    }
    delete ui;
}

//Loads images and displays them on a QGridLayout
void cropMenu::loadImages(QStringList fileList)
{
    //gets the folder that the images came from
    QFileInfo startingInfo(fileList[0]);
    initialFilePath = startingInfo.path();

    //Get filepath of watermark(THE APPLICATION FOLDER)
    watermarkPath = QCoreApplication::applicationDirPath();
    watermarkPath.append("/Cropped Ifunny Watermark 140 x 20.png");
    watermarkFilePath = watermarkPath.toStdString();

    //Setup loading bar for loading
    QPointer<QProgressDialog> imageLoadProgress = new QProgressDialog("Loading Images", "Abort", 0, fileList.size(), this);
    imageLoadProgress->setWindowModality(Qt::WindowModal);
    imageLoadProgress->show();

    if(fileList.isEmpty()){
        std::cout << "Empty Array" << std::endl;
        return;
    }
    //Loads images into grid view
    int j = 0;
    int k = 0;
    int validCounter = 0;

    for(int i = 0; i < fileList.length(); i++){
        if(imageLoadProgress->wasCanceled()){
            close();
            return;
        }

        //Error Checking
        QImage image;
        QImageReader imageCheck(fileList[i]);
        if(!image.load(fileList[i])){
            std::cout << "invalid image: "<< fileList[i].toStdString() << std::endl;
            imageLoadProgress->setValue(i+1);
            continue;
        }
        if(image.width() <= 140 || image.height() <= 20){
            std::cout << "Image too Small" << std::endl;
            imageLoadProgress->setValue(i+1);
            continue;
        }

        if(imageCheck.supportsAnimation() && imageCheck.imageCount() > 1){
            std::cout << "Animated images not allowed" << std::endl;
            imageLoadProgress->setValue(i+1);
            continue;
        }
        //for placing images in a grid
        if(k > 5){
            k = 0;
            j++;
        }

        //Displays image on UI and makes it clickable
        QPixmap bitmap;
        bitmap.convertFromImage(image, Qt::AutoColor);
        QPointer<Clickable> label = new Clickable(this);
        label->setToolTip(fileList[i]);
        label->setFixedSize(100, 100);
        label->setPixmap(bitmap.scaled(100,100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setAlignment(Qt::AlignCenter);
        ui->imageGridView->addWidget(label, j, k, {Qt::AlignTop, Qt::AlignLeft});
        connect(label , &Clickable::clicked, this, [=](){emit(labelClicked(fileList[i]));});

        //Connects label to a checkbox
        QPointer<QCheckBox> checkBox = new QCheckBox(ui->imageGridView->itemAt(validCounter)->widget());
        checkBox->setCheckable(true);

        connect(checkBox, &QCheckBox::stateChanged, [=](bool checked) {
            if (checked){
                emit checkBoxChecked(checkBox, validCounter, fileList[i]);
                checkBox->parentWidget()->setStyleSheet("QLabel { border: 2px solid red; }");
                numChecked++;
            }
            else{
                emit checkBoxChecked(checkBox, validCounter, fileList[i]);
                checkBox->parentWidget()->setStyleSheet("border: 0px");
                numChecked--;
            }
            ui->cropNumber->setText(QString("Images to Crop:  %1").arg(numChecked));
        });

        //Updating some variables for next iteration
        validImages.push_back(std::make_tuple(fileList[i], false, image));
        k++;
        validCounter++;
        QApplication::processEvents();
        imageLoadProgress->setValue(i+1);
    }
    //Displays info on screen
    int numItems = ui->imageGridView->count();
    ui->imageNumber->setText(QString("Number of Images:  %1").arg(numItems));
    detectWatermark();
}


void cropMenu::detectWatermark(){
    //initializes some stuff to setup
    cv::Mat watermark = createHistogram(watermarkPath);
    QPointer<QProgressDialog> imageComparison = new QProgressDialog("Identifying Watermarks", "Abort", 0, validImages.size(), this);
    imageComparison->setWindowModality(Qt::WindowModal);
    imageComparison->show();

    //Identifies any watermarks in the imagelist
    QSettings settings;
    float histogramThreshold;
    float normThreshold;

    if(settings.contains("thresh/hist")){
        histogramThreshold = settings.value("thresh/hist").toDouble();
    }
    else{
        histogramThreshold = 0.75;
    }
    if(settings.contains("thresh/norm")){
        normThreshold = settings.value("thresh/norm").toDouble();
    }
    else{
        normThreshold = 1.0;
    }

    cv::Mat matWatermark = cv::imread(watermarkFilePath, cv::IMREAD_COLOR);

    //Iterates through all valid images
    for (int i = 0; i < validImages.size(); i++){
        if(imageComparison->wasCanceled()){
            close();
            return;
        }
        if(settings.value("compare/algo").toInt() == HIST_INDEX){
            //Performing Histogram Algorithm
            cv::Mat imageToCheck = createHistogram(std::get<FILE_PATH>(validImages[i]));
            if(imageToCheck.empty()){
                std::cout << "Error Creating Histogram, try changing the file name" << std::endl;
                imageComparison->setValue(i+1);
                ui->cropNumber->setText(QString("Images to Crop:  %1").arg(numChecked));
                continue;
            }

            //Calculates score based on similarity(lower = better)
            float score = histDetect(imageToCheck, watermark);
            if(score < histogramThreshold){
                QCheckBox* checkBox;
                checkBox = ui->imageGridView->itemAt(i)->widget()->findChild<QCheckBox*>();
                checkBox->setCheckState(Qt::Checked);
            }
        }
        else if(settings.value("compare/algo").toInt() == NORM_INDEX){
            //Performing Normalization Algorithm

            //Calculates score based on similarity(lower = better)
            float score = compareImage(std::get<FILE_PATH>(validImages[i]), matWatermark);
            if(score == -1){
                std::cout << "Error Calculating Norm, try changing the file name" << std::endl;
                imageComparison->setValue(i+1);
                ui->cropNumber->setText(QString("Images to Crop:  %1").arg(numChecked));
                continue;
            }
            if(score < normThreshold){
                QCheckBox* checkBox;
                checkBox = ui->imageGridView->itemAt(i)->widget()->findChild<QCheckBox*>();
                checkBox->setCheckState(Qt::Checked);
            }
        }
        QApplication::processEvents();
        imageComparison->setValue(i+1);
    }
    ui->cropNumber->setText(QString("Images to Crop:  %1").arg(numChecked));
}


//Creates Histogram matrix based on given image
cv::Mat cropMenu::createHistogram(QString fileName){
    //Gets image from file
    cv::Mat img = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
    if(img.empty()){
        std::cout << "!!! Failed imread(): image not found" << std::endl;
        std::cout << "Offending File: " << fileName.toStdString() << std::endl;
        return cv::Mat();
    }

    //Converts into HSV format and gets the bottom right corner of image
    cv::Mat hsvImg = img(cv::Rect(img.cols-140, img.rows-20, 140, 20));
    cv::cvtColor(hsvImg, hsvImg, cv::COLOR_BGR2HSV_FULL);

    //Create histogram based on HSV
    cv::Mat histImg;
    cv::calcHist(&hsvImg, 1, channels, cv::Mat(), histImg, 2, histSize, ranges, true, false);

    cv::normalize(histImg, histImg, 0,1,cv::NORM_MINMAX, -1, cv::Mat());
    return histImg;
}


float cropMenu::histDetect(cv::Mat img, cv::Mat watermark){

    //compares the watermark with the bottom right corner of the image
    float score = cv::compareHist(img, watermark, cv::HISTCMP_BHATTACHARYYA);
    return score;
}


float cropMenu::compareImage(QString fileName, cv::Mat watermark){
    //error checking
    cv::Mat image = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
    if(image.empty()){
        std::cout << "!!! Failed imread(): image not found" << std::endl;
        std::cout << "Offending File: " << fileName.toStdString() << std::endl;
        return -1;
    }

    //Get bottom right corner of image
    cv::Mat corner = image(cv::Rect(image.cols-140, image.rows-20, 140, 20));

    //Compare two matrices
    if ( corner.rows > 0 && corner.rows == watermark.rows && corner.cols > 0 && corner.cols == watermark.cols ) {
        // Calculate the L2 relative error between images.
        double errorL2 = norm( corner, watermark, cv::NORM_L2);
        // Convert to a reasonable scale, since L2 error is summed across all pixels of the image.
        double similarity = errorL2 / (double)( corner.rows * corner.cols );
        return similarity;
    }

    return -1;
}


void cropMenu::on_goBack_clicked()
{
    numChecked = 0;
    close();
    emit firstWindow();
}


void cropMenu::checkBoxChecked(QCheckBox*, int pos, QString filePath){
    std::get<CHECKED>(validImages[pos]) ^= true;
}


void cropMenu::labelClicked(QString filePath){
    //Displays a preview of the clicked image on the window
    int height = ui->previewLabel->height();
    int width = ui->previewLabel->width();
    QPixmap pix(filePath);
    QStringList pieces = filePath.split( "/" );
    QString fileDisplay = "Filename: " + pieces.last();
    ui->filenameLabel->setText(fileDisplay);
    ui->previewLabel->setPixmap(pix.scaled(width,height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


void cropMenu::closeEvent(QCloseEvent*)
{
    std::cout << "Window Closed!" << std::endl;
    numChecked = 0;
    close();
    emit firstWindow();

}


void cropMenu::saveImages(QDir directory){
    //Create progress bar
    QPointer<QProgressDialog> imageSaveProgress = new QProgressDialog("Saving Images", "Abort", 0, validImages.size(), this);
    imageSaveProgress->setWindowModality(Qt::WindowModal);
    imageSaveProgress->show();
    connect(imageSaveProgress, SIGNAL(canceled()), this, SLOT(myCustomCancel()));
    int failed = 0;
    //Iterates through all the checked images
    for(int i = 0; i < validImages.size(); i++){
        if(imageSaveProgress->wasCanceled()){
            close();
            return;
        }
        if(std::get<CHECKED>(validImages[i])){
            //Saves the cropped versuib
            QImage original = std::get<IMAGE>(validImages[i]);
            QRect rect(0, 0, original.width(), original.height() - 20);
            QImage cropped = original.copy(rect);
            QFile f(std::get<FILE_PATH>(validImages[i]));
            QFileInfo fileInfo(f);
            QString fileName(directory.absoluteFilePath(fileInfo.fileName()));
            bool successful = cropped.save(fileName, nullptr, 100);
            if(!successful){
                std::cout << "Error: File Save failed" << std::endl;
                failed++;
            }
        }
        QApplication::processEvents();
        imageSaveProgress->setValue(i+1);
    }
    QString str("Save Complete! %1 images failed to save.");
    str = str.arg(failed);
    QMessageBox info;
    info.setText(str);
    info.setIcon(QMessageBox::Information);
    info.setWindowTitle("Save Successful");
    info.exec();
}


void cropMenu::noImageMessage(){
    QMessageBox info;
    info.setText("No watermarks are being cropped, "
                 "check some images or try a "
                 "different set of images");
    info.setIcon(QMessageBox::Information);
    info.setWindowTitle("No Watermarks");
    info.exec();
}


void cropMenu::on_save_clicked()
{
    if(numChecked == 0){
        noImageMessage();
        return;
    }
    QDir directory(initialFilePath);
    saveImages(directory);
    close();
}


void cropMenu::on_saveAs_clicked()
{
    if(numChecked == 0){
        noImageMessage();
        return;
    }
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QDir directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 savePath,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if(!directory.exists()){
        return;
    }

    saveImages(directory);
    close();
}
