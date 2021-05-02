#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addcostswindow.h"

QJsonObject openUserData(); //main.cpp
void saveToJson(QJsonObject data); //main.cpp
void addPlans(QString category, int sum); //main.cpp
QJsonObject openUserInfo(); //main.cpp
void addUserInfo(QString login, QString password, int id); //main.cpp

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    initializeChartColors();

    login = openUserInfo().value("login").toString();
    password = openUserInfo().value("password").toString();
    userID = openUserInfo().value("id").toInt();

    chart = new QChart();
    ui->setupUi(this);
    if(userID) {
        reloadAllData();
        ui->stackedWidget->setCurrentIndex(2);
    }

    QDate currentDate = QDate::currentDate();
    startDay.setDate(currentDate.year(), currentDate.month(), 1);
    createNewSelectedData();
    ui->selectRange->setDate(startDay);
    updateMap();
    updateTable();
    updatePlansTable();

    createNewChart();
    chartview = new QChartView(chart);
    chartview->setRenderHint(QPainter::Antialiasing);
    chartview->chart()->setAnimationOptions(QChart::AllAnimations);
    chartview->setParent(ui->chartWidget);
    ui->addPlanSumLine->setValidator(new QIntValidator(1, 999999, this));
}

void MainWindow::initializeChartColors() {
    //all with 300 material.io/resources/color
    chartColors["Продукты"] = "#4fc3f7";         //light blue
    chartColors["Всё для дома"] = "#af4448";     //red (dark)
    chartColors["Кафе"] = "#c7a4ff";             //deep purple (light)
    chartColors["Коммун. услуги"] = "#883997";   //purple (dark)
    chartColors["Транспорт"] = "#7986cb";        //indigo
    chartColors["Здоровье"] = "#81c784";         //green
    chartColors["Красота"] = "#ffb74d";          //orange
    chartColors["Одежда"] = "#009faf";           //cyan (dark)
    chartColors["Досуг"] = "#ff8a65";            //deep orange
    chartColors["Прочее"] = "#62757f";           //grey (dark)
}

void MainWindow::readData() {
    fullUserData = openUserData();
    userCostsData = fullUserData.value("data").toArray();
    userPlansData = fullUserData.value("plans").toArray();
}

void MainWindow::createNewChart() {
    series = new QPieSeries();
    QMapIterator<QString, int> i(costsTypeCount);
    while (i.hasNext()) {
        i.next();
         series->append(i.key(), i.value());
    }
    paintChart();
    chart->removeAllSeries();
    chart->addSeries(series);
    series->setHorizontalPosition(0.245);
    series->setPieSize(1);
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->legend()->setFont(QFont("Trebuchet MS", 10));
    chart->legend()->setLabelColor(Qt::black);
    chart->legend()->detachFromChart();
    chart->legend()->setGeometry(QRectF(440, 10, 260, 190));
    chart->setMinimumSize(QSize(740, 250));
    chart->setMaximumSize(QSize(740, 250));
    chart->setBackgroundRoundness(0);
}

void MainWindow::paintChart() {
    for (auto slice : series->slices()) {
        QString name = slice->label();
        slice->setColor(chartColors[name]);
    }
}

void MainWindow::updateMap() {
    costsTypeCount.clear();
    for (int i = 0; i < selectedCostsDataRange.count(); ++i) {
        QString type = selectedCostsDataRange.at(i).toObject().value("category").toString();
        int count = selectedCostsDataRange.at(i).toObject().value("sum").toInt();
        if (costsTypeCount.contains(type)) {
            costsTypeCount[type] += count;
        }
        else {
            costsTypeCount.insert(type, count);
        }
    }
}

void MainWindow::createNewSelectedData() {
    QJsonArray tempArr;
    for (int i = 0; i < userCostsData.count(); ++i) {
        QDate dataDate = QDate::fromString(userCostsData.at(i).toObject().value("date").toString(), "yyyy.MM.dd");
        if (dataDate >= startDay) {
            int id = userCostsData.at(i).toObject().value("id").toInt();
            int sum = userCostsData.at(i).toObject().value("sum").toInt();
            QString date, category, comment;
            date = userCostsData.at(i).toObject().value("date").toString();
            category = userCostsData.at(i).toObject().value("category").toString();
            comment = userCostsData.at(i).toObject().value("comment").toString();
            QJsonObject oneAddition;
            oneAddition.insert("id", id);
            oneAddition.insert("date", date);
            oneAddition.insert("category", category);
            oneAddition.insert("sum", sum);
            oneAddition.insert("comment", comment);
            tempArr.push_back(oneAddition);
        }
    }
    selectedCostsDataRange = tempArr;
}

void MainWindow::addToSelectedData() {
    for (int i = 0; i < userCostsData.count(); ++i) {
        QDate dataDay = QDate::fromString(userCostsData.at(i).toObject().value("date").toString(), "yyyy.MM.dd");
        if (dataDay >= startDay) {
            if (!selectedCostsDataRange.contains(userCostsData.at(i).toObject())) {
                selectedCostsDataRange.append(userCostsData.at(i).toObject());
            }
        }
    }
}

void MainWindow::removeFromSelectedData(int removeID) {
    for (int i = 0; i < selectedCostsDataRange.count(); ++i) {
        int currentID = selectedCostsDataRange.at(i).toObject().value("id").toInt();
        if (removeID == currentID) {
            selectedCostsDataRange.removeAt(i);
        }
    }
}

void MainWindow::updateTable() {
    QStandardItemModel* costsTablemodel = new QStandardItemModel(nullptr);
    costsTablemodel->setHorizontalHeaderLabels(QStringList()<<"ID"<<""<<"Дата"<<"Категория"<<"Сумма"<<"Комментарий");

    for (int i = 0; i < selectedCostsDataRange.count(); ++i) {
        QString id, date, category, sum, comment;
        id = QString::number(selectedCostsDataRange.at(i).toObject().value("id").toInt());
        date = selectedCostsDataRange.at(i).toObject().value("date").toString();
        category = selectedCostsDataRange.at(i).toObject().value("category").toString();
        sum = QString::number(selectedCostsDataRange.at(i).toObject().value("sum").toInt());
        comment = selectedCostsDataRange.at(i).toObject().value("comment").toString();

        QStandardItem* costsTableIDCol = new QStandardItem(id);
        QStandardItem* costsTableSpacingCol = new QStandardItem("");
        QStandardItem* costsTableDateCol = new QStandardItem(date);
        QStandardItem* costsTableCategoryCol = new QStandardItem(category);
        QStandardItem* costsTableSumCol = new QStandardItem(sum);
        QStandardItem* costsTableCommentCol = new QStandardItem(comment);

        costsTableCategoryCol->setData(QColor(chartColors[category]), Qt::TextColorRole);

        costsTablemodel->appendRow(QList<QStandardItem*>() << costsTableIDCol << costsTableSpacingCol
                                   << costsTableDateCol << costsTableCategoryCol << costsTableSumCol << costsTableCommentCol);
    }
    ui->costsTable->setModel(costsTablemodel);
    ui->costsTable->horizontalHeader()->setDefaultAlignment(Qt::AlignJustify);
    ui->costsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->costsTable->setColumnHidden(0, true);
    ui->costsTable->setColumnWidth(1, 2);
    ui->costsTable->setColumnWidth(2, 105);
    ui->costsTable->setColumnWidth(3, 165);
    ui->costsTable->setColumnWidth(4, 90);
    ui->costsTable->horizontalHeader()->setStretchLastSection(true);
    ui->costsTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->costsTable->sortByColumn(0, Qt::DescendingOrder);
}

void MainWindow::addToChart() {
    QMapIterator<QString, int> i(costsTypeCount);
    if (series->slices().count() == 0) {
        createNewChart();
    }
    else {
        while (i.hasNext()) {
            i.next();
            bool existence = false;
            for (int j = 0; j < series->slices().count(); ++j) {
                QString sliceName = series->slices().at(j)->label();
                int count = series->slices().at(j)->value();
                if (i.key() == sliceName) {
                    existence = true;
                    if (i.value() != count) {
                        series->slices().at(j)->setValue(costsTypeCount[sliceName]);
                    }
                }
            }
            if (!existence) {
                series->append(i.key(), i.value());
            }
        }
        paintChart();
    }
    chartview->update();
}

void MainWindow::RemoveFromChart(QString name, int sum) {
    for (int j = 0; j < series->slices().count(); ++j) {
        QString sliceName = series->slices().at(j)->label();
        int count = series->slices().at(j)->value();
        if (name == sliceName) {
            if (count != sum) {
                series->slices().at(j)->setValue(count - sum);
            }
            else {
                series->remove(series->slices().at(j));
            }
        }
    }
    chartview->update();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_addCostsButton_clicked() {
    addCostsWindow addCosts;
    addCosts.setModal(true);
    addCosts.exec();

    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(updateAllRequestFinished(QNetworkReply*)));

    QString req = server + "getAll?" + "userID=" + QString::number(userID) + "&password=" + password;
    qDebug() << userID;
    request.setUrl(QUrl(req));
    manager->get(request);
}

void MainWindow::updateAllRequestFinished(QNetworkReply* reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }
    QString str = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(str.toUtf8());
    fullUserData = jsonResponse.object();
    userCostsData = fullUserData.value("data").toArray();
    userPlansData = fullUserData.value("plans").toArray();
    addToSelectedData();
    updateMap();
    addToChart();
    updateTable();
}

void MainWindow::on_removeCostsButton_clicked() {
    QItemSelectionModel *select = ui->costsTable->selectionModel();
    if (select->hasSelection()) {
        QModelIndex index = select->currentIndex();
        int colID = index.sibling(index.row(), 0).data().toInt();
        int count = index.sibling(index.row(), 4).data().toInt();
        QString name = index.sibling(index.row(), 3).data().toString();

        ui->costsTable->model()->removeRow(index.row());

        for (int i = 0; i < userCostsData.count(); ++i) {
            int jsonID = userCostsData.at(i).toObject().value("id").toInt();
            if (jsonID == colID) {
                userCostsData.removeAt(i);
                removeCostRequest(colID);
                RemoveFromChart(name, count);
                removeFromSelectedData(colID);
                break;
            }
        }
    }
}

void MainWindow::removeCostRequest(int costID) {
    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(removeCostRequestFinished(QNetworkReply*)));

    QString req = server + "removeCost?" + "userID=" + QString::number(userID)
            + "&costID=" + QString::number(costID);
    qDebug() << req;
    request.setUrl(QUrl(req));
    manager->get(request);
}

void MainWindow::removeCostRequestFinished(QNetworkReply* reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }
    answer = reply->readAll();
    qDebug() << answer;
}

void MainWindow::on_changeDataRangeButton_clicked() {
    startDay = QDate::fromString(ui->selectRange->text(), "yyyy.MM.dd");
    createNewSelectedData();
    updateMap();
    updateTable();
    createNewChart();
    chartview->update();
}

void MainWindow::updatePlansTable() {
    QStandardItemModel* planModel = new QStandardItemModel(nullptr);
    planModel->setHorizontalHeaderLabels(QStringList()<<""<<"Категория"<<"Потрачено"<<"");

    for (int i = 0; i < userPlansData.count(); ++i) {
        QString category = userPlansData.at(i).toObject().value("category").toString();
        int sum = userPlansData.at(i).toObject().value("sum").toInt();
        int spentPercentage = round(((double)costsTypeCount[category] / (double)sum)*100);

        QStandardItem* spacingCol = new QStandardItem("");
        QStandardItem* categoryCol = new QStandardItem(category);
        QStandardItem* spentCol = new QStandardItem(QString::number(costsTypeCount[category])+
                                                    "/" + QString::number(sum));
        QStandardItem* spentPecentageCol = new QStandardItem(QString::number(spentPercentage) + "%");
        planModel->appendRow(QList<QStandardItem*>()<<spacingCol<<categoryCol<<spentCol<<spentPecentageCol);
    }
    ui->planTable->setModel(planModel);
    ui->planTable->horizontalHeader()->setDefaultAlignment(Qt::AlignJustify);
    ui->planTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->planTable->setColumnWidth(0, 2);
    ui->planTable->setColumnWidth(1, 150);
    ui->planTable->setColumnWidth(2, 150);
    ui->planTable->horizontalHeader()->setStretchLastSection(true);
}

bool MainWindow::allowAddingPlan(QString category, int sum) {
    if (!sum) {
        return false;
    }
    for (int i = 0; i < userPlansData.count(); ++i) {
        QString existingCategory = userPlansData.at(i).toObject().value("category").toString();
        if (category == existingCategory) {
            return false;
        }
    }
    return true;
}

void MainWindow::on_addPlanButton_clicked() {
    QString category = ui->addPlanCategoryBox->currentText();
    int sum = ui->addPlanSumLine->text().toInt();

    if (allowAddingPlan(category, sum)) {
        addPlans(category, sum);
        readData();
        updatePlansTable();
    }
}

void MainWindow::on_removePlanButton_clicked() {
    QItemSelectionModel *select = ui->planTable->selectionModel();
    if (select->hasSelection()) {
        QModelIndex index = select->currentIndex();
        QString tableName = index.sibling(index.row(), 1).data().toString();

        ui->planTable->model()->removeRow(index.row());

        for (int i = 0; i < userPlansData.count(); ++i) {
            QString jsonName = userPlansData.at(i).toObject().value("category").toString();
            if (jsonName == tableName) {
                userPlansData.removeAt(i);
                QJsonObject data;
                data.insert("data", userCostsData);
                data.insert("plans", userPlansData);
                saveToJson(data);
                break;
            }
        }
    }
}

void MainWindow::on_authEnterButton_clicked() {
    login = ui->authLoginLine->text();
    password = ui->authPasswordLine->text();
    logIn();
}

void MainWindow::logIn() {
    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(authRequestFinished(QNetworkReply*)));

    QString req = server + "login?" + "login=" + login + "&password=" + password;
    request.setUrl(QUrl(req));
    manager->get(request);
}

void MainWindow::on_regEnterButton_clicked() {
    login = ui->regLoginLine->text();
    password = ui->regPasswordLine->text();
    QString repeatPassword = ui->regRepeatPasswordLine->text();

    if (login.length() < 4) {
        ui->regWrongLabel->setText("Минимальная длина логина — 4 символа");
        ui->regLoginLine->clear();
    }
    else if (password.length() < 6) {
        ui->regWrongLabel->setText("Минимальная длина пароля — 6 символов");
        ui->regPasswordLine->clear();
        ui->regRepeatPasswordLine->clear();
    }
    else if (password != repeatPassword) {
        ui->regWrongLabel->setText("Пароли не совпадают");
        ui->regPasswordLine->clear();
        ui->regRepeatPasswordLine->clear();
    }
    else {
        manager = new QNetworkAccessManager();
        QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(regRequestFinished(QNetworkReply*)));

        QString req = server + "register?" + "login=" + login + "&password=" + password;
        request.setUrl(QUrl(req));
        manager->get(request);
    }
}

void MainWindow::regRequestFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }
    answer = reply->readAll();

    if (answer == "alreadyExists") {
        ui->regWrongLabel->setText("Логин занят");
        ui->regLoginLine->clear();
    }
    else {
        userID = answer.toUInt();
        ui->stackedWidget->setCurrentIndex(2);
        addUserInfo(login, password, answer.toInt());
    }
}

void MainWindow::authRequestFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }
    answer = reply->readAll();

    if (answer == "noneUser") {
        ui->authWrongLabel->setText("Нет пользователя с таким именем");
        ui->authLoginLine->clear();
    }
    else {
        userID = answer.toUInt();
        ui->stackedWidget->setCurrentIndex(2);
        addUserInfo(login, password, answer.toInt());
    }
}

void MainWindow::reloadAllData() {
    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(reloadDataRequestFinished(QNetworkReply*)));

    QString req = server + "getAll?" + "userID=" + QString::number(userID) + "&password=" + password;
    qDebug() << userID;
    request.setUrl(QUrl(req));
    manager->get(request);
}

void MainWindow::reloadDataRequestFinished(QNetworkReply* reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }
    QString str = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(str.toUtf8());
    fullUserData = jsonResponse.object();
    userCostsData = fullUserData.value("data").toArray();
    userPlansData = fullUserData.value("plans").toArray();
    createNewSelectedData();
    updateMap();
    updateTable();
    updatePlansTable();
    createNewChart();
}

void MainWindow::on_updateButton_clicked() {
    reloadAllData();
}

void MainWindow::on_exitButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_authToRegButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_regToAuthButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_authEnterButton_pressed() {
    ui->authEnterButtonFrame->setStyleSheet(primaryButtonFrameFocusStyle);
}

void MainWindow::on_authEnterButton_released() {
    ui->authEnterButtonFrame->setStyleSheet(defaultFrameStyle);
}

void MainWindow::on_regEnterButton_pressed() {
    ui->regEnterButtonFrame->setStyleSheet(primaryButtonFrameFocusStyle);
}

void MainWindow::on_regEnterButton_released() {
    ui->regEnterButtonFrame->setStyleSheet(defaultFrameStyle);
}

void MainWindow::on_authToRegButton_pressed() {
    ui->authToRegButtonFrame->setStyleSheet(secondaryButtonFrameFocusStyle);
}

void MainWindow::on_authToRegButton_released() {
    ui->authToRegButtonFrame->setStyleSheet(defaultFrameStyle);
}

void MainWindow::on_regToAuthButton_pressed() {
    ui->regToAuthButtonFrame->setStyleSheet(secondaryButtonFrameFocusStyle);
}

void MainWindow::on_regToAuthButton_released() {
    ui->regToAuthButtonFrame->setStyleSheet(defaultFrameStyle);
}

void MainWindow::on_addCostsButton_pressed() {
    ui->addCostsButtonFrame->setStyleSheet(acceptButtonFrameFocusStyle);
}

void MainWindow::on_addCostsButton_released() {
    ui->addCostsButtonFrame->setStyleSheet(defaultFrameStyle);
}

void MainWindow::on_changeDataRangeButton_pressed() {
    ui->changeDataRangeButtonFrame->setStyleSheet(primaryButtonFrameFocusStyle);
}

void MainWindow::on_changeDataRangeButton_released() {
    ui->changeDataRangeButtonFrame->setStyleSheet(defaultFrameStyle);
}

void MainWindow::on_removeCostsButton_pressed() {
    ui->removeCostsButtonFrame->setStyleSheet(deleteButtonFrameFocusStyle);
}

void MainWindow::on_removeCostsButton_released() {
    ui->removeCostsButtonFrame->setStyleSheet(defaultFrameStyle);
}

void MainWindow::on_addPlanButton_pressed() {
    ui->addPlanButtonFrame->setStyleSheet(acceptButtonFrameFocusStyle);
}

void MainWindow::on_addPlanButton_released() {
    ui->addPlanButtonFrame->setStyleSheet(defaultFrameStyle);
}

void MainWindow::on_removePlanButton_pressed() {
    ui->removePlanButtonFrame->setStyleSheet(deleteButtonFrameFocusStyle);
}

void MainWindow::on_removePlanButton_released(){
    ui->removePlanButtonFrame->setStyleSheet(defaultFrameStyle);
}

void MainWindow::on_updateButton_pressed() {
    ui->updateButtonFrame->setStyleSheet(primaryButtonFrameFocusStyle);
}

void MainWindow::on_updateButton_released() {
    ui->updateButtonFrame->setStyleSheet(defaultFrameStyle);
}

void MainWindow::on_exitButton_pressed() {
    ui->exitButtonFrame->setStyleSheet(exitButtonFrameFocusStyle);
}

void MainWindow::on_exitButton_released() {
    ui->exitButtonFrame->setStyleSheet(defaultFrameStyle);
}
