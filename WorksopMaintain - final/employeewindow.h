#ifndef EMPLOYEEWINDOW_H
#define EMPLOYEEWINDOW_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QtSql>

// Forward declaration of the Ui namespace class
namespace Ui {
class employeewindow;
}

class employeewindow : public QDialog
{
    Q_OBJECT

public:
    explicit employeewindow(QWidget *parent = nullptr);
    ~employeewindow();

private slots:
    void on_empClose_clicked();
    void on_addEmpbtn_clicked();
    void on_deleteEmpbtn_clicked();
    void on_showEmpData_clicked();

private:
    Ui::employeewindow *ui;
};

#endif // EMPLOYEEWINDOW_H
