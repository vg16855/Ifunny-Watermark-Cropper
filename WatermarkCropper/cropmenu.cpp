#include "cropmenu.h"
#include "ui_cropmenu.h"

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

void cropMenu::on_pushButton_clicked()
{
    this->close();
    emit firstWindow();
}

