#ifndef ADDCOSTSWINDOW_H
#define ADDCOSTSWINDOW_H

#include <QDialog>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace Ui {
class AddCostsWindow;
}

class AddCostsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddCostsWindow(QWidget *parent = nullptr);
    ~AddCostsWindow();

    bool isAdded = false;

    int tempID, tempSum;

    QString tempDate, tempCategory, tempComment;

private slots:
    void on_addCAddButton_pressed();

    void on_addCAddButton_released();

    void on_addCRejectButton_pressed();

    void on_addCRejectButton_released();

    void on_addCRejectButton_clicked();

    void on_addCAddButton_clicked();

private:
    Ui::AddCostsWindow *ui;

    QString secondaryButtonFrameFocusStyle = "QFrame{"
    "border-style: solid;"
    "border-width: 5px;"
    "border-color: rgb(192,196,200);"
    "border-radius: 6px;"
    "background-color: rgb(192,196,200);"
    "}";

    QString acceptButtonFrameFocusStyle = "QFrame{"
    "border-style: solid;"
    "border-width: 5px;"
    "border-color: rgb(157, 204, 182);"
    "border-radius: 6px;"
    "background-color: rgb(157, 204, 182);"
    "}";

    QString defaultFrameStyle = "QFrame{"
    "border: none;"
    "background-color: white;"
    "}";
};

#endif // ADDCOSTSWINDOW_H
