#include "mainwindow.h"

#include <QApplication>
int userId;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

QJsonObject openUserInfo() {
    QFile infoFile;
    infoFile.setFileName("userInfo.json");

    if (infoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray(infoFile.readAll()));
        infoFile.close();
        QJsonObject jObj = doc.object();
        return jObj;
    }
    else {
        QJsonObject jObj;
        return jObj;
    }
}

void saveInfoToJson(QJsonObject info) {
    QJsonDocument docToWrite(info);
    QFile file;
    file.setFileName("userInfo.json");
    file.open(QIODevice::WriteOnly);
    file.write(docToWrite.toJson());
    file.close();
}

QJsonObject openUserData() {
    QFile dataFile;
    dataFile.setFileName("userData.json");

    if (dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray(dataFile.readAll()));
        dataFile.close();
        QJsonObject jObj = doc.object();
        return jObj;
    }
    else {
        QJsonObject jObj;
        return jObj;
    }
}

void addUserInfo(QString login, QString password, int id) {
    QJsonObject userInfo = openUserInfo();
    userInfo.insert("id", id);
    userInfo.insert("login", login);
    userInfo.insert("password", password);

    saveInfoToJson(userInfo);
}
void saveToJson(QJsonObject data) {
    QJsonDocument docToWrite(data);
    QFile file;
    file.setFileName("userData.json");
    file.open(QIODevice::WriteOnly);
    file.write(docToWrite.toJson());
    file.close();
}

void addCosts (int id, int sum, QString date, QString category, QString comment) {
    QJsonObject fullUserData = openUserData();
    QJsonArray costsData = fullUserData.value("data").toArray();
    QJsonObject oneAddition, newFullUserData;

    oneAddition.insert("id", id);
    oneAddition.insert("date", date);
    oneAddition.insert("category", category);
    oneAddition.insert("sum", sum);
    oneAddition.insert("comment", comment);

    costsData.push_back(oneAddition);
    newFullUserData.insert("data", costsData);
    newFullUserData.insert("plans", fullUserData.value("plans").toArray());
    saveToJson(newFullUserData);
}

void addPlans(QString category, int sum) {
    QJsonObject fullUserData = openUserData();
    QJsonArray plansData = fullUserData.value("plans").toArray();
    QJsonObject oneAddition, newFullUserData;

    oneAddition.insert("category", category);
    oneAddition.insert("sum", sum);

    plansData.push_back(oneAddition);
    newFullUserData.insert("data", fullUserData.value("data").toArray());
    newFullUserData.insert("plans", plansData);
    saveToJson(newFullUserData);
}
