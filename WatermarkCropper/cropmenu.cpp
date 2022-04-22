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
}

cropMenu::~cropMenu()
{
    std::cout << "Deleting Images" << std::endl;
    int numItems = validImages.size();
    std::cout << numItems << " Items to delete" << std::endl;
    for(int i = 0; i < numItems; i++){
        std::cout << "Deleting image " << i << std::endl;
        QLayoutItem* item = ui->imageGridView->takeAt(0);
        delete item->widget();
        delete item;
    }
    delete ui;
}

void cropMenu::loadImages(QStringList fileList)
{
    //gets the folder that the images came from
    QFileInfo startingInfo(fileList[0]);
    initialFilePath = startingInfo.path();
    std::cout << initialFilePath.toStdString() << std::endl;

    //Get filepath of watermark(THE APPLICATION FOLDER)
    watermarkPath = QCoreApplication::applicationDirPath();
    watermarkPath.append("/Cropped Ifunny Watermark 140 x 20.png");
    watermarkFilePath = watermarkPath.toStdString();

    //Setup loading bar for loading
    QPointer<QProgressDialog> imageLoadProgress = new QProgressDialog("Loading Images", "Abort", 0, fileList.size(), this);
    imageLoadProgress->setWindowModality(Qt::WindowModal);
    imageLoadProgress->show();
    imageLoadProgress->deleteLater();
    connect(imageLoadProgress, SIGNAL(canceled()), this, SLOT(myCustomCancel()));

    if(fileList.isEmpty()){
        std::cout << "Empty Array" << std::endl;
        return;
    }
    std::cout << "Size of fileList: " << fileList.size() << std::endl;
    //Loads images into grid view
    int j = 0;
    int k = 0;
    int validCounter = 0;

    for(int i = 0; i < fileList.length(); i++){
        if(imageLoadProgress->wasCanceled())
            break;
        //Error Checking
        std::cout << fileList[i].toStdString() << std::endl;
        QImage image;
        if(!image.load(fileList[i])){
            std::cout << "invalid image" << std::endl;
            continue;
        }
        printf("Image Dimensions: %d %d \n", image.width(), image.height());
        if(image.width() < 140 || image.height() < 20){
            std::cout << "Image too Small" << std::endl;
            continue;
        }
        QImageReader reader(fileList[i]);
        if(reader.supportsAnimation()){
            std::cout << "Animated images not allowed" << std::endl;
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
                std::cout << "Checked" << std::endl;
                numChecked++;
            }
            else{
                emit checkBoxChecked(checkBox, validCounter, fileList[i]);
                std::cout << "Unchecked" << std::endl;
                numChecked--;
            }
            ui->cropNumber->setText(QString("Images to Crop:  %1").arg(numChecked));
        });

        //Updating some variables for next iteration
        validImages.push_back(std::make_pair(fileList[i], false));
        k++;
        validCounter++;
        imageLoadProgress->setValue(i+1);

    }
    //Displays info on screen
    int numItems = ui->imageGridView->count();
    ui->imageNumber->setText(QString("Number of Images:  %1").arg(numItems));
    detectWatermark();
}

void cropMenu::detectWatermark(){
    std::cout << "Making Watermark Histogram" << std::endl;
    std::cout << watermarkPath.toStdString() << std::endl;
    cv::Mat watermark = createHistogram(watermarkPath);
    QPointer<QProgressDialog> imageComparison = new QProgressDialog("Identifying Watermarks", "Abort", 0, validImages.size(), this);
    imageComparison->setWindowModality(Qt::WindowModal);
    imageComparison->show();

    connect(imageComparison, SIGNAL(canceled()), this, SLOT(myCustomCancel()));

    //Identifies any watermarks in the imagelist
    QSettings settings;
    float histogramThreshold = settings.value("thresh/hist").toDouble();
    float normThreshold = settings.value("thresh/norm").toDouble();
    cv::Mat matWatermark = cv::imread(watermarkFilePath, cv::IMREAD_COLOR);
    for (int i = 0; i < validImages.size(); i++){
        if(imageComparison->wasCanceled())
            break;
        if(settings.value("compare/algo").toInt() == HIST_INDEX){
            //Performing Histogram Algorithm
            std::cout << "Performing Histogram Algorithm" << std::endl;
            cv::Mat imageToCheck = createHistogram(validImages[i].first);
            if(imageToCheck.empty()){
                std::cout << "Error Creating Histogram, try changing the file name" << std::endl;
                imageComparison->setValue(i+1);
                ui->cropNumber->setText(QString("Images to Crop:  %1").arg(numChecked));
                continue;
            }
            float score = histDetect(imageToCheck, watermark);
            if(score < histogramThreshold){
                QCheckBox* checkBox;
                checkBox = ui->imageGridView->itemAt(i)->widget()->findChild<QCheckBox*>();
                checkBox->setCheckState(Qt::Checked);
            }
            std::cout << "Filepath: " << validImages[i].first.toStdString() << std::endl;
            std::cout << "Image Score: " << score << std::endl;
        }
        else if(settings.value("compare/algo").toInt() == NORM_INDEX){
            std::cout << "Performing Normalization Algorithm" << std::endl;
            float score = compareImage(validImages[i].first, matWatermark);

            if(score == -1){
                std::cout << "Error Calculating Norm, try changing the file name" << std::endl;
                imageComparison->setValue(i+1);
                ui->cropNumber->setText(QString("Images to Crop:  %1").arg(numChecked));
                continue;
            }

            std::cout << "Image Score: " << score << std::endl;
            if(score < normThreshold){
                QCheckBox* checkBox;
                checkBox = ui->imageGridView->itemAt(i)->widget()->findChild<QCheckBox*>();
                checkBox->setCheckState(Qt::Checked);
            }
        }
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


void cropMenu::checkBoxChecked(QCheckBox *checkBox, int pos, QString filePath){
    validImages[pos].second ^= true;
}

void cropMenu::labelClicked(QString filePath){
    int height = ui->previewLabel->height();
    int width = ui->previewLabel->width();
    QPixmap pix(filePath);
    QStringList pieces = filePath.split( "/" );
    QString fileDisplay = "Filename: " + pieces.last();
    ui->filenameLabel->setText(fileDisplay);
    ui->previewLabel->setPixmap(pix.scaled(width,height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    std::cout << "Label Clicked!!!" << std::endl;
}


void cropMenu::closeEvent(QCloseEvent* event)
{
    numChecked = 0;
    close();
    emit firstWindow();

}

void cropMenu::saveImages(QDir directory){
    QPointer<QProgressDialog> imageSaveProgress = new QProgressDialog("Saving Images", "Abort", 0, validImages.size(), this);
    imageSaveProgress->setWindowModality(Qt::WindowModal);
    imageSaveProgress->show();
    connect(imageSaveProgress, SIGNAL(canceled()), this, SLOT(myCustomCancel()));

    for(int i = 0; i < validImages.size(); i++){
        if(imageSaveProgress->wasCanceled())
            break;
        if(validImages[i].second){
            QImage original(validImages[i].first);
            QRect rect(0, 0, original.width(), original.height() - 20);
            QImage cropped = original.copy(rect);
            QFile f(validImages[i].first);
            QFileInfo fileInfo(f);
            QString fileName(directory.absoluteFilePath(fileInfo.fileName()));
            bool successful = cropped.save(fileName, nullptr, 100);
            if(!successful){
                std::cout << "Error: File Save failed" << std::endl;
            }
        }
        imageSaveProgress->setValue(i+1);
    }
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

void cropMenu::myCustomCancel(){
    std::cout << "Canceled" << std::endl;
    numChecked = 0;
    close();
    emit firstWindow();
}
