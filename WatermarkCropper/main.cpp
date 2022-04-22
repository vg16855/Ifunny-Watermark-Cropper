#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("TeamFortress");
    QCoreApplication::setApplicationName("iFunnyWatermarkCropper");
    QCoreApplication::setApplicationVersion("1.0");

    MainWindow w;
    w.show();
    return a.exec();
}
