#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QFontDatabase>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(Resources);

    QFontDatabase::addApplicationFont(":/Resources/Lato-Regular.ttf");
    QFontDatabase::addApplicationFont(":/Resources/BebasNeue-Regular.ttf");

    // set app style sheet
    QFile styleSheetFile(":/Resources/Diffnes.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet{QLatin1String(styleSheetFile.readAll())};
    a.setStyleSheet(styleSheet);

    QCoreApplication::setOrganizationName("TeamFortress");
    QCoreApplication::setApplicationName("iFunnyWatermarkCropper");
    QCoreApplication::setApplicationVersion("1.0");

    MainWindow w;
    w.show();
    return a.exec();
}
