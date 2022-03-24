#include "cropmenu.h"
#include "ui_cropmenu.h"
#include <QLabel>
#include <QSizePolicy>
#include <opencv2/opencv.hpp>

cv::String watermarkFilePath = "C:/Users/Bryan/Documents/Computer Science/CMSC 437/project-team-fortress/WatermarkCropper/Cropped Ifunny Watermark 140 x 20.png";
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
    std::cout << numItems << "Items to delete" << std::endl;
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
    if(fileList.isEmpty()){
        std::cout << "Empty Array" << std::endl;
        return;
    }
    int j = 0;
    int k = 0;
    for(int i = 0; i < fileList.length(); i++){
        if(k > 5){
            k = 0;
            j++;
        }
        QPixmap img(fileList[i]);
        QLabel *label = new QLabel();
        label->setFixedSize(100, 100);
        label->setPixmap(img.scaled(100,100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setAlignment(Qt::AlignCenter);
        ui->imageGridView->addWidget(label, j, k, {Qt::AlignTop, Qt::AlignLeft});
        k++;
    }
    detectWatermark(fileList[0]);
}

void cropMenu::detectWatermark(QString fileName){
    //reads in image
    cv::Mat img = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
    cv::Mat watermark = cv::imread(watermarkFilePath, cv::IMREAD_COLOR);
    if(img.empty() || watermark.empty()){
        std::cout << "!!! Failed imread(): image not found" << std::endl;
        return;
    }
    //Initialize Values once image is verified
    int hBins = 50;
    int sBins = 60;
    int histSize[] = {hBins, sBins};

    float hRanges[] = {0,180};
    float sRanges[] = { 0, 256 };
    const float* ranges[] = { hRanges, sRanges };
    int channels[] = {0,1};

    //converts image into Hue, Saturation, and Value format
    std::cout << "Converting to HSV" << std::endl;
    cv::Mat hsvImg;
    cv::Mat hsvWatermark;
    std::cout << "CVT Color" << std::endl;
    cv::cvtColor(img, hsvImg, cv::COLOR_BGR2HSV);
    cv::cvtColor(watermark, hsvWatermark, cv::COLOR_BGR2HSV);
    std::cout << "Bottom Right of Image" << std::endl;
    //gets bottom right of image
    cv::Mat hsvBottomRight = hsvImg(cv::Range(hsvImg.rows-20, hsvImg.rows), cv::Range(hsvImg.cols-140, hsvImg.cols));

    //Creating Histograms
    cv::Mat histBottomRight, histWatermark;
    std::cout << "Calculating Histograms" << std::endl;
    cv::calcHist(&hsvBottomRight, 1, channels, cv::Mat(), histBottomRight, 2, histSize, ranges, true, false);
    cv::normalize(histBottomRight, histBottomRight, 0,1,cv::NORM_MINMAX, -1, cv::Mat());
    cv::calcHist(&hsvWatermark, 1, channels, cv::Mat(), histWatermark, 2, histSize, ranges, true, false);
    cv::normalize(histWatermark, histWatermark, 0,1,cv::NORM_MINMAX, -1, cv::Mat());

    //compares the watermark with the bottom right corner of the image
    for(int compareMethod = 0; compareMethod < 4; compareMethod++){
        double score = cv::compareHist(histBottomRight, histWatermark, compareMethod);
        std::cout << "Comparison Score Method "<< compareMethod << ": " << score << std::endl;
    }
}

void cropMenu::on_goBack_clicked()
{
    close();
    emit firstWindow();
}

