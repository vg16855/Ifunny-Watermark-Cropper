#include "cropmenu.h"
#include "ui_cropmenu.h"
#include <QLabel>
#include <QSizePolicy>

cropMenu::cropMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cropMenu)
{
    ui->setupUi(this);
}

cropMenu::~cropMenu()
{
    delete ui;
}

void cropMenu::loadImages(QStringList fileList)
{
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
        label->setPixmap(img.scaled(100,100));
        ui->imageGridView->addWidget(label, j, k);
        k++;
    }
}


void cropMenu::on_goBack_clicked()
{
    this->close();
    emit firstWindow();
}

