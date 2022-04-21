#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cropmenu.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_chooseImages_clicked();

    void on_chooseFolder_clicked();

    void on_actionOptions_triggered();

    void on_actionQuit_triggered();

private:
    void openCropMenu();
    Ui::MainWindow *ui;
    cropMenu *crop;
};
#endif // MAINWINDOW_H
