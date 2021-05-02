#include "addcostswindow.h"
#include "mainwindow.h"
#include "ui_addcostswindow.h"

void addCosts (int id, int sum, QString date, QString category, QString comment); //main.cpp
QJsonObject openUserInfo(); //main.cpp

addCostsWindow::addCostsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addCostsWindow) {

    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ui->addCChooseDate->setDate(QDate::currentDate());
    ui->addCSumLine->setValidator(new QIntValidator(1, 999999, this));
}

addCostsWindow::~addCostsWindow() {
    delete ui;
}

void addCostsWindow::on_addCAddButton_clicked() {
    std::srand(time(nullptr));
    int tempID = rand();
    int tempSum = ui->addCSumLine->text().toInt();
    QString tempDate = ui->addCChooseDate->text();
    QString tempCategory = ui->addCCategoryBox->currentText();
    QString tempComment = ui->addCComment->toPlainText();

    QJsonObject userInfo = openUserInfo();
    QString userID = QString::number(userInfo.value("id").toInt());
    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(addRequestFinished(QNetworkReply*)));

    QString req = "http://localhost:5000/addCosts?userID=" + userID + "&id=" + QString::number(tempID)
            + "&sum=" + QString::number(tempSum) + "&date=" + tempDate + "&category=" + tempCategory + "&comment=" + tempComment;

    request.setUrl(QUrl(req));
    manager->get(request);
}
void addCostsWindow::addRequestFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }
    qDebug() << reply->readAll();
    this->close();
}

void addCostsWindow::on_addCRejectButton_clicked() {
    this->close();
}

void addCostsWindow::on_addCAddButton_pressed() {
    ui->addCAddButtonFrame->setStyleSheet(acceptButtonFrameFocusStyle);
}

void addCostsWindow::on_addCAddButton_released() {
    ui->addCAddButtonFrame->setStyleSheet(defaultFrameStyle);
}

void addCostsWindow::on_addCRejectButton_pressed() {
    ui->addCRejectButtonFrame->setStyleSheet(secondaryButtonFrameFocusStyle);
}

void addCostsWindow::on_addCRejectButton_released() {
    ui->addCRejectButtonFrame->setStyleSheet(defaultFrameStyle);
}
