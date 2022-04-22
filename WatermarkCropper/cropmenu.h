#ifndef CROPMENU_H
#define CROPMENU_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QCheckBox>
#include <QDir>
#include <clickable.h>
#include <QProgressDialog>

namespace Ui {
class cropMenu;
}

class cropMenu : public QWidget
{
    Q_OBJECT

public:
    explicit cropMenu(QWidget *parent = nullptr);
    ~cropMenu();
    void loadImages(QStringList);
    float histDetect(cv::Mat, cv::Mat);
    void detectWatermark();
    float compareImage(QString, cv::Mat);
    cv::Mat createHistogram(QString);
    void noImageMessage();


signals:
    void firstWindow();



private slots:
    void on_goBack_clicked();

    void closeEvent(QCloseEvent*);

    void checkBoxChecked(QCheckBox*, int, QString);

    void labelClicked(QString);

    void saveImages(QDir);

    void on_save_clicked();

    void on_saveAs_clicked();

private:
    Ui::cropMenu *ui;

    std::vector<std::pair<QString, bool>> validImages;
    QString watermarkPath;
    std::string watermarkFilePath;


};

#endif // CROPMENU_H
