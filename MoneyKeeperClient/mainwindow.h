#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItemModel>
#include <QtCharts>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void on_regEnterButton_pressed();

    void on_regEnterButton_released();

    void on_regToAuthButton_pressed();

    void on_regToAuthButton_released();

    void on_authEnterButton_pressed();

    void on_authEnterButton_released();

    void on_authToRegButton_pressed();

    void on_authToRegButton_released();

    void on_addCostsButton_pressed();

    void on_addCostsButton_released();

    void on_removeCostsButton_pressed();

    void on_removeCostsButton_released();

    void on_changeDataRangeButton_pressed();

    void on_changeDataRangeButton_released();

    void on_removePlanButton_pressed();

    void on_removePlanButton_released();

    void on_addPlanButton_pressed();

    void on_addPlanButton_released();

    void on_updateButton_pressed();

    void on_updateButton_released();

    void on_exitButton_pressed();

    void on_exitButton_released();

    void on_addPlanButton_clicked();

    void on_removePlanButton_clicked();

    void on_regEnterButton_clicked();

    void on_authToRegButton_clicked();

    void on_regToAuthButton_clicked();

    void on_addCostsButton_clicked();

    void on_changeDataRangeButton_clicked();

    void on_removeCostsButton_clicked();

    void on_updateButton_clicked();

    void on_exitButton_clicked();

    void readUserInfo();

    void clearAllInputLines();

    void clearAll();

    void createNewSelectedData();

    void updateMap();

    void updateTable();

    void updatePlansTable();

    void createNewChart();

    void paintChart();

    void addToSelectedData();

    void addToChart();

    void removeFromSelectedData(int removeID);

    void RemoveFromChart(QString name, int sum);

    bool allowAddingPlan(QString category, int sum);

    void on_authEnterButton_clicked();

    void reloadAllData();

    void removeCostRequest(int costID);

    void addPlanRequest(int sum, QString category);

    void removePlanRequest(QString category);

    void regRequestFinished(QNetworkReply *reply);

    void authRequestFinished(QNetworkReply *reply);

    void reloadDataRequestFinished(QNetworkReply *reply);

    void addCost(QString category, QString comment, QString date, int sum);

    void createAllManagers();

    void createMessageBox(QString reply);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool needShow = false;

private:
    Ui::MainWindow *ui;

    QDate startDay;

    QJsonObject fullUserData;

    QJsonArray userCostsData, userPlansData, selectedCostsDataRange;

    QMap<QString, int> allCostSums;

    QMap<QString, QString> chartColors {
        //all with 300 material.io/resources/color

        {"????????????????", "#4fc3f7"},         //light blue
        {"?????? ?????? ????????", "#af4448"},     //red (dark)
        {"????????", "#c7a4ff"},             //deep purple (light)
        {"????????????. ????????????", "#883997"},   //purple (dark)
        {"??????????????????", "#7986cb"},        //indigo
        {"????????????????", "#81c784"},         //green
        {"??????????????", "#ffb74d"},          //orange
        {"????????????", "#009faf"},           //cyan (dark)
        {"??????????", "#ff8a65"},            //deep orange
        {"????????????", "#62757f"}            //grey (dark)
    };

    QChart *chart;

    QPieSeries *series;

    QChartView *chartview;

    QStandardItemModel *costsTableModel, *planTableModel;

    QNetworkAccessManager *reloadAllManager, *addCostManager,
    *removeCostManager, *addPlanManager, *removePlanManager,
    *authManager, *regManager;

    QNetworkRequest request;

    QString login, password;

    QString server = "http://localhost:5000/";

    int userID = 0, maxCostID = 0;

    QString primaryButtonFrameFocusStyle = "QFrame{"
            "border-style: solid;"
            "border-width: 5px;"
            "border-color: rgb(147,190,251);"
            "border-radius: 6px;"
            "background-color: rgb(147,190,251);"
            "}";

    QString deleteButtonFrameFocusStyle = "QFrame{"
            "border-style: solid;"
            "border-width: 5px;"
            "border-color: rgb(240, 168, 175);"
            "border-radius: 6px;"
            "background-color: rgb(240, 168, 175);"
            "}";

    QString acceptButtonFrameFocusStyle = "QFrame{"
            "border-style: solid;"
            "border-width: 5px;"
            "border-color: rgb(157, 204, 182);"
            "border-radius: 6px;"
            "background-color: rgb(157, 204, 182);"
            "}";

    QString secondaryButtonFrameFocusStyle = "QFrame{"
            "border-style: solid;"
            "border-width: 5px;"
            "border-color: rgb(181,185,189);"
            "border-radius: 6px;"
            "background-color: rgb(181,185,189);"
            "}";

    QString defaultFrameStyle = "QFrame{"
            "border: none;"
            "background-color: white;"
            "}";

    QString exitButtonFrameFocusStyle = "QFrame{"
            "border-style: solid;"
            "border-width: 5px;"
            "border-color: rgb(192,196,200);"
            "border-radius: 6px;"
            "background-color: rgb(192,196,200);"
            "}";

};
#endif // MAINWINDOW_H
