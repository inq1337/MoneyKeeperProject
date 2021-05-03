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

void addUserInfo(QString login, QString password, int id) {
    QJsonObject userInfo = openUserInfo();
    userInfo.insert("id", id);
    userInfo.insert("login", login);
    userInfo.insert("password", password);
    saveInfoToJson(userInfo);
}

void removeUserInfo() {
    QFile infoFile;
    infoFile.setFileName("userInfo.json");
    infoFile.remove();
}
