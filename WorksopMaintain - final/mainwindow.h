// Filename: mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_showEmp_clicked();

    void on_actionExit_triggered();

    void on_createTkt_clicked();

    void on_tktMng_clicked();

private:
    Ui::MainWindow *ui;
    //QSqlDatabase DB_Connection;
};



#endif // MAINWINDOW_H
