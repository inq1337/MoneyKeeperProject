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
    const QRect rect(QPoint(0,0), this->geometry().size());
    QBitmap b(rect.size());
    b.fill(QColor(Qt::color0));
    QPainter painter(&b);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(rect, 6, 6, Qt::AbsoluteSize);
    painter.end();
    this->setMask(b);

    ui->addCChooseDate->setDate(QDate::currentDate());
    ui->addCSumLine->setValidator(new QIntValidator(1, 999999, this));
}

addCostsWindow::~addCostsWindow() {
    delete ui;
}

void addCostsWindow::on_addCAddButton_clicked() {
    tempSum = ui->addCSumLine->text().toInt();
    tempDate = ui->addCChooseDate->text();
    tempCategory = ui->addCCategoryBox->currentText();
    tempComment = ui->addCComment->toPlainText();
    isAdded = true;
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
