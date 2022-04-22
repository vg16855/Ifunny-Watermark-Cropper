#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //set app style sheet
    QFontDatabase::addApplicationFont(":/Resources/Lato-Regular.ttf");
    QFontDatabase::addApplicationFont(":/Resources/BebasNeueu-Regular.ttf");

    QString stylePath = QCoreApplication::applicationDirPath();
    stylePath.append("/Diffnes.qss");
    QFile styleSheetFile(stylePath);
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet { QLatin1String(styleSheetFile.readAll()) };
    a.setStyleSheet(styleSheet);

    QCoreApplication::setOrganizationName("TeamFortress");
    QCoreApplication::setApplicationName("iFunnyWatermarkCropper");
    QCoreApplication::setApplicationVersion("1.0");

    MainWindow w;
    w.show();
    return a.exec();
}
