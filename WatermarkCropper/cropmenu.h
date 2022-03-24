#ifndef CROPMENU_H
#define CROPMENU_H

#include <QWidget>
#include <opencv2/opencv.hpp>

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
    void detectWatermark(cv::Mat, cv::Mat);
    cv::Mat createHistogram(QString);

signals:
    void firstWindow();

private slots:
    void on_goBack_clicked();

    void on_cropImages_clicked();

private:
    Ui::cropMenu *ui;
};

#endif // CROPMENU_H
