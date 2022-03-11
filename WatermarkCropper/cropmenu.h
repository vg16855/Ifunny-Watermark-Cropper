#ifndef CROPMENU_H
#define CROPMENU_H

#include <QWidget>

namespace Ui {
class cropMenu;
}

class cropMenu : public QWidget
{
    Q_OBJECT

public:
    explicit cropMenu(QWidget *parent = nullptr);
    ~cropMenu();

signals:
    void firstWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::cropMenu *ui;
};

#endif // CROPMENU_H
