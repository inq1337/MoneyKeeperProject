#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addcostswindow.h"
#include "progressbardelegate.h"

QJsonObject openUserInfo(); //main.cpp
void addUserInfo(QString login, QString password, int id); //main.cpp
void removeUserInfo(); //main.cpp

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    readUserInfo();
    chart = new QChart();
    createAllManagers();

    ui->setupUi(this);
    ui->centralwidget->hide();

    QDate currentDate = QDate::currentDate();
    startDay.setDate(currentDate.year(), currentDate.month(), 1);
    ui->selectRange->setDate(startDay);
    if (userID) {
        QString req = server + "login?" + "login=" + login + "&password=" + password;
        request.setUrl(QUrl(req));
        authManager->get(request);
    }
    else {
        createNewChart();
        updateTable();
        updatePlansTable();
        ui->centralwidget->show();
    }

    chartview = new QChartView(chart);
    chartview->setRenderHint(QPainter::Antialiasing);
    chartview->chart()->setAnimationOptions(QChart::AllAnimations);
    chartview->resize(ui->chartFrame->size());
    chartview->setParent(ui->chartFrame);
    ui->addPlanSumLine->setValidator(new QIntValidator(1, 999999, this));
}

void MainWindow::createAllManagers() {
    reloadAllManager = new QNetworkAccessManager();
    addCostManager = new QNetworkAccessManager();
    removeCostManager = new QNetworkAccessManager();
    addPlanManager = new QNetworkAccessManager();
    regManager = new QNetworkAccessManager();
    removePlanManager = new QNetworkAccessManager();
    authManager = new QNetworkAccessManager();

    QObject::connect(reloadAllManager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(reloadDataRequestFinished(QNetworkReply*)));
    QObject::connect(regManager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(regRequestFinished(QNetworkReply*)));
    QObject::connect(authManager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(authRequestFinished(QNetworkReply*)));

    QObject::connect(addCostManager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {
            if (reply->error()) {
                QMessageBox::information(this, "????", reply->errorString());
                return;
            }
            QString answer = reply->readAll();
        }
    );
    QObject::connect(removeCostManager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {
            if (reply->error()) {
                createMessageBox(reply->errorString());
                return;
            }
            QString answer = reply->readAll();
        }
    );
    QObject::connect(addPlanManager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {
            if (reply->error()) {
                createMessageBox(reply->errorString());
                return;
            }
            QString answer = reply->readAll();
        }
    );
    QObject::connect(removePlanManager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {
            if (reply->error()) {
                createMessageBox(reply->errorString());
                return;
            }
            QString answer = reply->readAll();
        }
    );
}

void MainWindow::readUserInfo() {
    login = openUserInfo().value("login").toString();
    password = openUserInfo().value("password").toString();
    userID = openUserInfo().value("id").toInt();
}

void MainWindow::createNewChart() {
    series = new QPieSeries();
    QMapIterator<QString, int> i(allCostSums);
    while (i.hasNext()) {
        i.next();
         series->append(i.key(), i.value());
    }
    paintChart();
    chart->removeAllSeries();
    chart->addSeries(series);
    series->setHorizontalPosition(0.209);
    series->setPieSize(1);
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->legend()->setFont(QFont("Trebuchet MS", 10));
    chart->legend()->setLabelColor(Qt::black);
    chart->legend()->detachFromChart();
    chart->legend()->setGeometry(QRectF(390, 10, 330, 220));
    chart->setBackgroundRoundness(0);
}

void MainWindow::paintChart() {
    for (auto slice : series->slices()) {
        QString name = slice->label();
        slice->setColor(chartColors[name]);
    }
}

void MainWindow::updateMap() {
    allCostSums.clear();
    for (int i = 0; i < selectedCostsDataRange.count(); ++i) {
        QString type = selectedCostsDataRange.at(i).toObject().value("category").toString();
        int count = selectedCostsDataRange.at(i).toObject().value("sum").toInt();
        if (allCostSums.contains(type)) {
            allCostSums[type] += count;
        }
        else {
            allCostSums.insert(type, count);
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
    costsTableModel = new QStandardItemModel(nullptr);
    costsTableModel->setHorizontalHeaderLabels(QStringList()<<"ID"<<""<<"????????"<<"??????????????????"<<"??????????"<<"??????????????????????");

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

        costsTableModel->appendRow(QList<QStandardItem*>() << costsTableIDCol << costsTableSpacingCol
                                   << costsTableDateCol << costsTableCategoryCol << costsTableSumCol << costsTableCommentCol);
    }
    ui->costsTable->setModel(costsTableModel);
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
    QMapIterator<QString, int> i(allCostSums);
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
                        series->slices().at(j)->setValue(allCostSums[sliceName]);
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
    AddCostsWindow addCosts;
    addCosts.setModal(true);
    addCosts.exec();
    if (addCosts.isAdded) {
        addCost(addCosts.tempCategory, addCosts.tempComment,
                addCosts.tempDate, addCosts.tempSum);
    }
}

void MainWindow::addCost(QString category, QString comment, QString date, int sum) {
    QJsonObject addCostObj;
    int costID = maxCostID + 1;
    maxCostID += 1;
    addCostObj.insert("category", category);
    addCostObj.insert("comment", comment);
    addCostObj.insert("date", date);
    addCostObj.insert("id", costID);
    addCostObj.insert("sum", sum);
    userCostsData.append(addCostObj);
    addToSelectedData();
    updateMap();
    addToChart();
    updateTable();
    updatePlansTable();

    QString req = "http://localhost:5000/addCosts?userID=" + QString::number(userID) + "&id="
            + QString::number(costID) + "&sum=" + QString::number(sum) + "&date="
            + date + "&category=" + category + "&comment=" + comment;

    request.setUrl(QUrl(req));
    addCostManager->get(request);
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
                updateMap();
                updatePlansTable();
                break;
            }
        }
    }
}

void MainWindow::removeCostRequest(int costID) {
    QString req = server + "removeCost?" + "userID=" + QString::number(userID)
            + "&costID=" + QString::number(costID);
    request.setUrl(QUrl(req));
    removeCostManager->get(request);
}

void MainWindow::on_changeDataRangeButton_clicked() {
    startDay = QDate::fromString(ui->selectRange->text(), "yyyy.MM.dd");
    createNewSelectedData();
    updateMap();
    updateTable();
    updatePlansTable();
    createNewChart();
    chartview->update();
}

void MainWindow::updatePlansTable() {
    planTableModel = new QStandardItemModel(nullptr);
    planTableModel->setHorizontalHeaderLabels(QStringList()<<""<<"??????????????????"<<"??????????????????"<<"?? ??????????????????");

    for (int i = 0; i < userPlansData.count(); ++i) {
        QString category = userPlansData.at(i).toObject().value("category").toString();
        int sum = userPlansData.at(i).toObject().value("sum").toInt();
        int spentMoney;
        if (allCostSums.contains(category)) {
            spentMoney = allCostSums[category];
        }
        else {
            spentMoney = 0;
        }
        int spentPercentage = round(((double)spentMoney / (double)sum)*100);

        QStandardItem* spacingCol = new QStandardItem("");
        QStandardItem* categoryCol = new QStandardItem(category);
        QStandardItem* spentCol = new QStandardItem(QString::number(spentMoney)+ "/" + QString::number(sum));
        QStandardItem* spentPercentageCol = new QStandardItem(QString::number(spentPercentage));

        categoryCol->setData(QColor(chartColors[category]), Qt::TextColorRole);
        planTableModel->appendRow(QList<QStandardItem*>()<<spacingCol<<categoryCol<<spentCol<<spentPercentageCol);
    }
    ui->planTable->setModel(planTableModel);
    ui->planTable->setItemDelegateForColumn(3, new ProgressBarDelegate);
    ui->planTable->horizontalHeader()->setDefaultAlignment(Qt::AlignJustify);
    ui->planTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->planTable->setColumnWidth(0, 2);
    ui->planTable->setColumnWidth(1, 150);
    ui->planTable->setColumnWidth(2, 150);
    ui->planTable->horizontalHeader()->setStretchLastSection(true);
    ui->planTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
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
        QJsonObject onePlanAddition;
        onePlanAddition.insert("category", category);
        onePlanAddition.insert("sum", sum);
        userPlansData.append(onePlanAddition);
        updatePlansTable();
        addPlanRequest(sum, category);
    }
}

void MainWindow::addPlanRequest(int sum, QString category) {
    QString req = server + "addPlan?" + "userID=" + QString::number(userID) + "&category="
                + category + "&sum=" + QString::number(sum);
    request.setUrl(QUrl(req));
    addPlanManager->get(request);
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
                removePlanRequest(tableName);
                break;
            }
        }
    }
}

void MainWindow::removePlanRequest(QString category) {
    QString req = server + "removePlan?" + "userID=" + QString::number(userID) + "&category=" + category;
    request.setUrl(QUrl(req));
    removePlanManager->get(request);
}

void MainWindow::on_authEnterButton_clicked() {
    if (ui->authPasswordLine->text().length() < 6) {
        ui->authWrongLabel->setText("???????????????? ????????????");
        ui->authPasswordLine->clear();
    }
    else if (ui->authLoginLine->text().length() < 4) {
        ui->authWrongLabel->setText("?????? ???????????????????????? ?? ?????????? ????????????");
        ui->authLoginLine->clear();
    }
    else {
        login = ui->authLoginLine->text();
        password = ui->authPasswordLine->text();
        QString req = server + "login?" + "login=" + login + "&password=" + password;
        request.setUrl(QUrl(req));
        authManager->get(request);
    }
}

void MainWindow::on_regEnterButton_clicked() {
    if (ui->regLoginLine->text().length() < 4) {
        ui->regWrongLabel->setText("?????????????????????? ?????????? ???????????? ??? 4 ??????????????");
        ui->regLoginLine->clear();
    }
    else if (ui->regPasswordLine->text().length() < 6) {
        ui->regWrongLabel->setText("?????????????????????? ?????????? ???????????? ??? 6 ????????????????");
        ui->regPasswordLine->clear();
        ui->regRepeatPasswordLine->clear();
    }
    else if (ui->regPasswordLine->text() !=
             ui->regRepeatPasswordLine->text()) {
        ui->regWrongLabel->setText("???????????? ???? ??????????????????");
        ui->regPasswordLine->clear();
        ui->regRepeatPasswordLine->clear();
    }
    else {
        login = ui->regLoginLine->text();
        password = ui->regPasswordLine->text();

        QString req = server + "register?" + "login=" + login + "&password=" + password;
        request.setUrl(QUrl(req));
        regManager->get(request);
    }
}

void MainWindow::regRequestFinished(QNetworkReply *reply) {
    if (reply->error()) {
        createMessageBox(reply->errorString());
        return;
    }
    QString answer = reply->readAll();

    if (answer == "already exists") {
        ui->regWrongLabel->setText("?????????? ??????????");
        ui->regLoginLine->clear();
    }
    else {
        clearAllInputLines();
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::authRequestFinished(QNetworkReply *reply) {
    if (reply->error()) {
        if (ui->centralwidget->isHidden()) {
            ui->centralwidget->show();
        }
        createMessageBox(reply->errorString());
        return;
    }
    QString answer = reply->readAll();

    if (answer == "no such user") {
        ui->authWrongLabel->setText("?????? ???????????????????????? ?? ?????????? ????????????");
        ui->authLoginLine->clear();
    }
    else if (answer == "wrong password") {
        ui->authWrongLabel->setText("???????????????? ????????????");
        ui->authPasswordLine->clear();
    }
    else {
        userID = answer.toUInt();
        addUserInfo(login, password, answer.toInt());
        readUserInfo();
        reloadAllData();
        clearAllInputLines();
        ui->stackedWidget->setCurrentIndex(2);
        if (ui->centralwidget->isHidden()) {
            ui->centralwidget->show();
        }
    }
}

void MainWindow::reloadAllData() {
    QString req = server + "getAll?" + "userID=" + QString::number(userID) + "&password=" + password;
    request.setUrl(QUrl(req));
    reloadAllManager->get(request);
}

void MainWindow::reloadDataRequestFinished(QNetworkReply* reply) {
    if (reply->error()) {
        createMessageBox(reply->errorString());
        return;
    }
    QString str = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(str.toUtf8());
    fullUserData = jsonResponse.object();
    userCostsData = fullUserData.value("data").toArray();
    userPlansData = fullUserData.value("plans").toArray();
    for (int i = 0; i < userCostsData.count(); ++i) {
        int tempCostID = userCostsData.at(i).toObject().value("id").toInt();
        if (tempCostID > maxCostID) {
            maxCostID = tempCostID;
        }
    }
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
    removeUserInfo();
    clearAllInputLines();
    clearAll();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::clearAllInputLines() {
    ui->authLoginLine->clear();
    ui->authPasswordLine->clear();
    ui->authWrongLabel->clear();
    ui->regLoginLine->clear();
    ui->regPasswordLine->clear();
    ui->regRepeatPasswordLine->clear();
    ui->regWrongLabel->clear();
    ui->addPlanSumLine->clear();
}

void MainWindow::clearAll() {
    userID = 0;
    maxCostID = 0;
    password = "";
    login = "";
    chart->removeAllSeries();
    costsTableModel->clear();
    planTableModel->clear();
    allCostSums.clear();
    while (selectedCostsDataRange.count()) {
        selectedCostsDataRange.pop_back();
    }
    while (userCostsData.count()) {
        userCostsData.pop_back();
    }
    while (userPlansData.count()) {
        userPlansData.pop_back();
    }
}

void MainWindow::createMessageBox(QString reply) {
    if (reply == "Connection refused") {
        QMessageBox::warning(this, "????????????", "???? ?????????????? ???????????????????????? ?? ??????????????");
    }
    else {
    QMessageBox::critical(this, "????????????", reply);
    }
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
