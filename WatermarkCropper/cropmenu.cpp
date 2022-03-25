#include "cropmenu.h"
#include "ui_cropmenu.h"
#include <QLabel>
#include <QSizePolicy>
#include <QCheckBox>
#include <QProgressDialog>
#include <opencv2/opencv.hpp>

cv::String watermarkFilePath = "C:/Users/Bryan/Documents/Computer Science/CMSC 437/project-team-fortress/WatermarkCropper/Images/Cropped Ifunny Watermark 140 x 20.png";
//Initializes Values
int hBins = 50;
int sBins = 60;
int histSize[] = {hBins, sBins};

float hRanges[] = {0,180};
float sRanges[] = { 0, 256 };
const float* ranges[] = { hRanges, sRanges };
int channels[] = {0,1};
int compareMethod = 0;

cropMenu::cropMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cropMenu)
{
    ui->setupUi(this);
}

cropMenu::~cropMenu()
{
    std::cout << "Deleting Images" << std::endl;
    int numItems = ui->imageGridView->count();
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
    QProgressDialog imageLoadProgress("Loading Images", "Abort", 0, fileList.size(), this);
    imageLoadProgress.setWindowModality(Qt::WindowModal);
    imageLoadProgress.show();
    //Checks if array is empty
    if(fileList.isEmpty()){
        std::cout << "Empty Array" << std::endl;
        return;
    }
    std::cout << "Size of fileList: " << fileList.size() << std::endl;
    //Loads images into grid view
    int j = 0;
    int k = 0;

    for(int i = 0; i < fileList.length(); i++){
        std::cout << fileList[i].toStdString() << std::endl;
        if(k > 5){
            k = 0;
            j++;
        }
        QImage image;
        if(!image.load(fileList[i])){
            std::cout << "invalid image" << std::endl;
            continue;
        }
        std::cout << fileList[i].toStdString() << std::endl;
        printf("Image Dimensions: %d %d \n", image.width(), image.height());
        if(image.width() < 140 || image.height() < 20){
            std::cout << "Image too Small" << std::endl;
            continue;
        }
        QPixmap bitmap;
        bitmap.convertFromImage(image, Qt::AutoColor);
        QLabel *label = new QLabel();
        label->setFixedSize(100, 100);
        label->setPixmap(bitmap.scaled(100,100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setAlignment(Qt::AlignCenter);
        ui->imageGridView->addWidget(label, j, k, {Qt::AlignTop, Qt::AlignLeft});

        QCheckBox *checkBox = new QCheckBox(label);
        checkBox->setCheckable(true);

        connect(checkBox, &QCheckBox::stateChanged, [=](bool checked) {
            if (checked)
                emit checkBoxChecked(checkBox, i, fileList[i]);
            else{
                emit checkBoxChecked(checkBox, i, fileList[i]);
            }
        });

        validImages.push_back(std::make_pair(fileList[i], false));
        k++;
        imageLoadProgress.setValue(i+1);

    }
    //Locates watermarks
    int numItems = ui->imageGridView->count();
    cv::Mat watermark = createHistogram(QString::fromStdString(watermarkFilePath));
    std::cout << "Items to check: " <<numItems << std::endl;

    QProgressDialog imageComparison("Processing Images", "Abort", 0, fileList.size(), this);
    imageComparison.setWindowModality(Qt::WindowModal);
    imageComparison.show();

    for (int i = 0; i < numItems; i++){
        cv::Mat imageToCheck = createHistogram(validImages[i].first);
        detectWatermark(imageToCheck, watermark);
        imageComparison.setValue(i+1);
    }
}

//Creates Histogram matrix based on given image
cv::Mat cropMenu::createHistogram(QString fileName){
    //std::cout << fileName.toStdString() << std::endl;
    //Gets image from file
    cv::Mat img = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
    if(img.empty()){
        std::cout << "!!! Failed imread(): image not found" << std::endl;
        return cv::Mat();
    }

    //Converts into HSV format and gets the bottom right corner of image
    cv::Mat hsvImg = img(cv::Rect(img.cols-140, img.rows-20, 140, 20));
    //cv::imshow("Cropped Image", hsvImg);
    cv::cvtColor(hsvImg, hsvImg, cv::COLOR_BGR2HSV);
    //std::cout << "Conversion Successful" << std::endl;

    //Create histogram based on HSV
    cv::Mat histImg;
    cv::calcHist(&hsvImg, 1, channels, cv::Mat(), histImg, 2, histSize, ranges, true, false);
    cv::normalize(histImg, histImg, 0,1,cv::NORM_MINMAX, -1, cv::Mat());
    return histImg;
}

void cropMenu::detectWatermark(cv::Mat img, cv::Mat watermark){

    //compares the watermark with the bottom right corner of the image
    double score = cv::compareHist(img, watermark, compareMethod);
    std::cout << "Comparison Score Method "<< compareMethod << ": " << score << std::endl;
}

void cropMenu::on_goBack_clicked()
{
    close();
    emit firstWindow();
}


void cropMenu::on_cropImages_clicked()
{
    for(int i = 0; i < validImages.size(); i++){
        std::cout << validImages[i].second << std::endl;
    }
}
void cropMenu::checkBoxChecked(QCheckBox *checkBox, int pos, QString filePath){
    std::cout << "Filepath: " << filePath.toStdString() << std::endl;
    std::cout << pos-1 << std::endl;
    validImages[pos-1].second ^= true;

}
void cropMenu::closeEvent(QCloseEvent* event)
{
    emit firstWindow();
    close();

}

