#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tinyxml2.h"

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

#include <QString>
#include <QBitArray>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_btnAdd_clicked();

    void on_btnDelete_clicked();

    void on_btnSave_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_btnLoad_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlTableModel* model;
};
#endif // MAINWINDOW_H
