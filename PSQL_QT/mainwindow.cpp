#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlField>

#include <QFileDialog>

using namespace tinyxml2;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("ip3");
    db.setUserName("postgres");
    db.setPassword("password");
    if (!db.open()){
        qDebug() << db.lastError().text();
        return;
    }
    else{
        qDebug() << "Success!";
    }

    model = new QSqlTableModel(this, db);
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->setTable("ip_portrait");
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);
    ui->tableView->horizontalHeader()->setSectionsClickable(1);
    model->select();
    ui->tableView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    model->submitAll();
    QSqlQuery query = QSqlQuery(db);
}


void MainWindow::on_pushButton_2_clicked()
{
    model->revertAll();
}


void MainWindow::on_pushButton_3_clicked()
{
    model->select();
}


void MainWindow::on_btnAdd_clicked()
{
    qDebug() << "Inserting row: " << model->insertRow(model->rowCount());
}


void MainWindow::on_btnDelete_clicked()
{
    int selectedRow = ui->tableView->currentIndex().row();
    if (selectedRow >= 0){
        qDebug() << "Deleting row: " << model->removeRow(selectedRow);
    }
    else{
        qDebug() << "No row selected";
    }
}


void MainWindow::on_btnSave_clicked()
{
    QSqlQuery query;
    QSqlQuery query2;
    QSqlQuery query3;

    query.exec("SELECT * FROM ip_portrait");

    QString id;
    QString row = ui->lineEdit->text();
    int row_id = row.toInt();
    qDebug() << row_id << query.size();
    if (row_id <= 0 || row_id > query.size()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Внимание");
        msgBox.setText("Введен неверный ID");
        msgBox.exec();
        return;
    }


    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "SQL_Portraits", tr("Text Files (*.xml)"));

    QString spath = fileName;
    QByteArray bpath = spath.toLocal8Bit();
    const char *path = bpath.data();

    XMLDocument xmlDoc;
    XMLDeclaration* declaration=xmlDoc.NewDeclaration();
    xmlDoc.InsertFirstChild(declaration);

//    query.exec("SELECT * FROM ip_portrait");
    int count = query.record().count();
    qDebug() << count;

    query2.exec("SELECT * FROM ip_term");
    int count_term = query2.record().count();
    qDebug() << count_term;

    query3.exec("SELECT * FROM ip_term_shingles");
    int count_shingles = query3.record().count();
    qDebug() << count_shingles;

    XMLElement* root=xmlDoc.NewElement("ip_portrait");
    while (query.next()) {
        if (query.record().field(0).name() == "id" && query.value(0).toString() == row) {
            id = query.value(0).toString();
            for (int i = 0; i < count; i++) {

                QByteArray ba = query.record().field(i).name().toLocal8Bit();
                const char *tb = ba.data();

                QString ib = query.value(i).toString();
                QByteArray bai = ib.toLocal8Bit();
                const char *tbi = bai.data();

                root->SetAttribute(tb, tbi);
            }
        query.last();
        }

        xmlDoc.InsertEndChild(root);

        query2.first();
        do {
            XMLElement* termR = xmlDoc.NewElement("ip_term");
            if ((query2.record().field(1).name() == "ip_id") && (query2.value(1).toString() == id)) {
                for (int i = 0; i < count_term; i++){

                    QByteArray ba = query2.record().field(i).name().toLocal8Bit();
                    const char *tb = ba.data();

                    QString ib = query2.value(i).toString();
                    QByteArray bai = ib.toLocal8Bit();
                    const char *tbi = bai.data();

                    termR->SetAttribute(tb, tbi);
                    root->InsertEndChild(termR);
                }
            }
        }while (query2.next());

        query3.first();
        do {
            XMLElement* shingleR = xmlDoc.NewElement("ip_term_shingles");
            if ((query3.record().field(1).name() == "ip_id") && (query3.value(1).toString() == id)) {
                for (int i = 0; i < count_shingles; i++){

                    QByteArray ba = query3.record().field(i).name().toLocal8Bit();
                    const char *tb = ba.data();

                    QString ib = query3.value(i).toString();
                    QByteArray bai = ib.toLocal8Bit();
                    const char *tbi = bai.data();

                    shingleR->SetAttribute(tb, tbi);
                    root->InsertEndChild(shingleR);
                }
            }
        }while (query3.next());
    }

    xmlDoc.SaveFile(path);
    XMLError eResult = xmlDoc.SaveFile(path);
    if (eResult != XML_SUCCESS) qDebug() << "error";
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        model = new QSqlTableModel(this, db);
        model->setEditStrategy(QSqlTableModel::OnRowChange);
        model->setTable("ip_portrait");
        model->select();
        ui->tableView->setModel(model);
        ui->tableView->setSortingEnabled(true);
        ui->tableView->sortByColumn(0, Qt::AscendingOrder);
        ui->tableView->horizontalHeader()->setSectionsClickable(1);
        ui->label->setEnabled(true);
        ui->lineEdit->setEnabled(true);
        ui->label->show();
        ui->lineEdit->show();
        break;
    case 1:
        model = new QSqlTableModel(this, db);
        model->setEditStrategy(QSqlTableModel::OnRowChange);
        model->setTable("ip_term");
        model->select();
        ui->tableView->setModel(model);
        ui->tableView->setSortingEnabled(true);
        ui->tableView->sortByColumn(0, Qt::AscendingOrder);
        ui->tableView->horizontalHeader()->setSectionsClickable(1);
        ui->label->setEnabled(false);
        ui->lineEdit->setEnabled(false);
        ui->label->hide();
        ui->lineEdit->hide();
        break;
    case 2:
        model = new QSqlTableModel(this, db);
        model->setEditStrategy(QSqlTableModel::OnRowChange);
        model->setTable("ip_term_shingles");
        model->select();
        ui->tableView->setModel(model);
        ui->tableView->setSortingEnabled(true);
        ui->tableView->sortByColumn(0, Qt::AscendingOrder);
        ui->tableView->horizontalHeader()->setSectionsClickable(1);
        ui->label->setEnabled(false);
        ui->lineEdit->setEnabled(false);
        ui->label->hide();
        ui->lineEdit->hide();
        break;
    default:
        break;
    }
}

void MainWindow::on_btnLoad_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.xml)"));

    XMLDocument xmlDoc;

    QString spath = fileName;
    QByteArray bpath = spath.toLocal8Bit();
    const char *path = bpath.data();

    QSqlQuery query;
    QSqlQuery query2;
    QSqlQuery query3;

    QString atRValueS;
    QString atRNameS;

    QString prep = "INSERT INTO ip_portrait (id, ";
    QString prepV;

    QString termPrep = "INSERT INTO ip_term (id, ip_id, ";
    QString termPrepV;

    QString shinglesPrep = "INSERT INTO ip_term_shingles (id, ip_id, ";
    QString shinglesPrepV;

    int id, id_term, id_term_shingles;
    QString Sid, Sid_term, Sid_term_shingles;

//    const char *id_chek;
    QString id_chek, name_id_chek = "id", name_ip_id_chek = "ip_id";

    xmlDoc.LoadFile(path);
    XMLError eResult = xmlDoc.LoadFile(path);
    if (eResult != XML_SUCCESS) {qDebug() << "Load Error";}

    XMLElement *portrait=xmlDoc.RootElement();
    qDebug() << portrait->Name();
    const XMLAttribute *attributeOfRoot = portrait->FirstAttribute();

    query.exec("SELECT * FROM ip_portrait");
    id = query.size();
    qDebug() << id;
    if(attributeOfRoot->Name() == name_id_chek){
        id_chek = attributeOfRoot->Value();
        attributeOfRoot = attributeOfRoot->Next();
    }
    while(attributeOfRoot){
        qDebug() << attributeOfRoot->Name() << "=" << attributeOfRoot->Value();
        if (attributeOfRoot->Next() != nullptr) {
            atRNameS = QString::fromStdString(attributeOfRoot->Name());
            prep.append(atRNameS + ", ");

            atRValueS = QString::fromStdString(attributeOfRoot->Value());
            prepV.append("'" + atRValueS + "', ");
        }
        else{

            atRNameS = QString::fromStdString(attributeOfRoot->Name());
            prep.append(atRNameS);

            atRValueS = QString::fromStdString(attributeOfRoot->Value());
            prepV.append("'" + atRValueS + "'");
        }
        attributeOfRoot = attributeOfRoot->Next();
    }
    id++;
    Sid = QString::number(id);
    prep.append(") VALUES (" + Sid + ", " + prepV + ")");
    qDebug() << prep;
    query.exec(prep);
    if (!query.execBatch()) qDebug() << query.lastError();

    XMLElement *surface=portrait->FirstChildElement();
    const XMLAttribute *attributeOfSurface;

    QString surfaceName;
    while (surface)
    {
        surfaceName = QString::fromStdString(surface->Name());
        qDebug() << surfaceName << ":";
        attributeOfSurface = surface->FirstAttribute();

        if (surfaceName == "ip_term"){

            query2.exec("SELECT * FROM ip_term");
            id_term = query2.size();
            qDebug() << id_term;

            attributeOfSurface = attributeOfSurface->Next();
            if(attributeOfSurface->Name() == name_ip_id_chek && attributeOfSurface->Value() == id_chek){
                attributeOfSurface = attributeOfSurface->Next();
                while (attributeOfSurface)
                {
                    qDebug() << attributeOfSurface->Name() << " = " << attributeOfSurface->Value();
                    if (attributeOfSurface->Next() != nullptr) {
                        atRNameS = QString::fromStdString(attributeOfSurface->Name());
                        termPrep.append(atRNameS + ", ");

                        atRValueS = QString::fromStdString(attributeOfSurface->Value());
                        termPrepV.append("'" + atRValueS + "', ");
                    }
                    else{
                        atRNameS = QString::fromStdString(attributeOfSurface->Name());
                        termPrep.append(atRNameS);

                        atRValueS = QString::fromStdString(attributeOfSurface->Value());
                        termPrepV.append("'" + atRValueS + "'");
                    }
                attributeOfSurface=attributeOfSurface->Next();
                }
                id_term++;
                Sid_term = QString::number(id_term);
                termPrep.append(") VALUES (" + Sid_term + ", " + Sid + ", " + termPrepV + ")");
                qDebug() << termPrep;
                query2.exec(termPrep);
                if (!query2.execBatch()) qDebug() << query2.lastError();
                termPrep = "INSERT INTO ip_term (id, ip_id, ";
                termPrepV = nullptr;
            }
        }

        if (surfaceName == "ip_term_shingles"){
            query3.exec("SELECT * FROM ip_term_shingles");
            id_term_shingles = query3.size();
            qDebug() << id_term_shingles;

            attributeOfSurface = attributeOfSurface->Next();
            if(attributeOfSurface->Name() == name_ip_id_chek && attributeOfSurface->Value() == id_chek){
                attributeOfSurface = attributeOfSurface->Next();
                while (attributeOfSurface)
                {
                    qDebug() << attributeOfSurface->Name() << " = " << attributeOfSurface->Value();

                    if (attributeOfSurface->Next() != nullptr) {
                        atRNameS = QString::fromStdString(attributeOfSurface->Name());
                        shinglesPrep.append(atRNameS + ", ");

                        atRValueS = QString::fromStdString(attributeOfSurface->Value());
                        shinglesPrepV.append("'" + atRValueS + "', ");
                    }
                    else{
                        atRNameS = QString::fromStdString(attributeOfSurface->Name());
                        shinglesPrep.append(atRNameS);

                        atRValueS = QString::fromStdString(attributeOfSurface->Value());
                        shinglesPrepV.append("'" + atRValueS + "'");
                    }
                    attributeOfSurface=attributeOfSurface->Next();
                }
                id_term_shingles++;
                Sid_term_shingles = QString::number(id_term_shingles);
                shinglesPrep.append(") VALUES (" + Sid_term_shingles + ", " + Sid + ", " + shinglesPrepV + ")");
                qDebug() << shinglesPrep;
                query3.exec(shinglesPrep);
                if (!query3.execBatch()) qDebug() << query3.lastError();
                shinglesPrep = "INSERT INTO ip_term_shingles (id, ip_id, ";
                shinglesPrepV = nullptr;
            }
        }
        surface=surface->NextSiblingElement();
    }
}
