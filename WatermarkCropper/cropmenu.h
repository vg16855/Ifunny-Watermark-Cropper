#ifndef CROPMENU_H
#define CROPMENU_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QCheckBox>


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
    float detectWatermark(cv::Mat, cv::Mat);
    cv::Mat createHistogram(QString);


signals:
    void firstWindow();




private slots:
    void on_goBack_clicked();

    void on_cropImages_clicked();

    void closeEvent(QCloseEvent*);

    void checkBoxChecked(QCheckBox*, int, QString);


private:
    Ui::cropMenu *ui;

    std::vector<std::pair<QString, bool>> validImages;

};

#endif // CROPMENU_H
