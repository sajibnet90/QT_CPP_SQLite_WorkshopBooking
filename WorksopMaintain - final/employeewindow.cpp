//Filename: employeewindow.cpp

#include "employeewindow.h"
#include "ui_employeewindow.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include "databasemanager.h"

employeewindow::employeewindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::employeewindow)
{
    ui->setupUi(this);
    // The database connection retrieved from DatabaseManager
    QSqlDatabase db = DatabaseManager::database();
    QSqlQuery query(db);

    // Check if the Employees table exists, and create it if not
    //------------------------------Employees Table create----------------------------------
    if (!query.exec("CREATE TABLE IF NOT EXISTS Employees (employeeID INTEGER PRIMARY KEY, employee_name TEXT, cost_per_hour REAL)")) {
        qDebug() << "Error creating Employees table: " << query.lastError().text();
    } // Check if table is empty first
    if (!query.exec("SELECT COUNT(*) FROM Employees")) {
        qDebug() << "Error checking Employees table: " << query.lastError().text();
    }
    //-----------------------insert 3 sample data to Employees table----------------------------------------
    query.next();
    if (query.value(0).toInt() == 0) {
        qDebug() << "Populating new Employees table with sample data.";
        query.exec("INSERT INTO Employees (employeeID, employee_name, cost_per_hour) VALUES (1, 'John Doe', 20.50)");
        query.exec("INSERT INTO Employees (employeeID, employee_name, cost_per_hour) VALUES (2, 'Jane Smith', 18.75)");
        query.exec("INSERT INTO Employees (employeeID, employee_name, cost_per_hour) VALUES (3, 'Michael Johnson', 22.00)");
    }

    //------------------------------EmployeeSchedule Table create----------------------------------
    if (!query.exec("CREATE TABLE IF NOT EXISTS EmployeeSchedule (id INTEGER PRIMARY KEY, employeeID INTEGER, date DATE, status TEXT, FOREIGN KEY (employeeID) REFERENCES Employees(employeeID))")) {
        qDebug() << "Error creating EmployeeSchedule table: " << query.lastError().text();
    }
    //-----------------------------------------------------------------
    // Insert some random booking data to calander widget of Each employee
    QStringList Statuses = {"booked", "free"};
    QDate startDate = QDate::currentDate().addDays(-15); // Start date for the demo data

    // loop over each employee ID for sample
    for (int employeeID = 1; employeeID <= 3; ++employeeID) {
        for (int i = 0; i < 30; ++i) { // Next 30 days
            QDate date = startDate.addDays(i);
            QString status;

            // Check if the day is Saturday (6) or Sunday (7)
            int dayOfWeek = date.dayOfWeek();
            if (dayOfWeek == 6 || dayOfWeek == 7) {
                status = "free" ; // Mark weekends as always free
            }
            else {
                // Randomly assign "booked" or "free" for weekdays
                status = Statuses[rand() % Statuses.size()];
            }

            QSqlQuery query(db);
            query.prepare("INSERT INTO EmployeeSchedule (employeeID, date, status) VALUES (:employeeID, :date, :status) ON CONFLICT DO NOTHING");
            query.bindValue(":employeeID", employeeID);
            query.bindValue(":date", date.toString("yyyy-MM-dd"));
            query.bindValue(":status", status);
            if (!query.exec()) {
                qDebug() << "Error inserting into EmployeeSchedule for employee ID" << employeeID << ":" << query.lastError().text();
            }
        }
    }

    // Load the data into the table widget
    on_showEmpData_clicked();
}

//--------------------------------------------------------------------------
employeewindow::~employeewindow()
{
    delete ui;
}

//---------------------------------------------------------------------

void employeewindow::on_empClose_clicked()
{
    close();
}
//---------------------------------------------------------------------------
//-----------------------------Adding a new Employee to the List------------------
void employeewindow::on_addEmpbtn_clicked()
{
    QString employeeIDText = ui->empIdInput->text(); // Retrieve the text as QString
    int employeeID = employeeIDText.toInt(); // Convert the QString to int
    QString name = ui->empNameInput->text();
    double costPerHour = ui->empCostInput->text().toDouble();

    // Check if the employee ID is empty
    if (employeeIDText.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter an employee ID.");
        return;
    }

    qDebug() << "employee ID:" << employeeID << "Name:" << name << "Cost Per Hour:" << costPerHour;

    QSqlDatabase db = DatabaseManager::database(); // Get database connection from DatabaseManager
    QSqlQuery query(db);

    query.prepare("INSERT INTO Employees (employeeID, Employee_Name, Cost_per_hour) VALUES (:employeeID, :name, :cost_per_hour)");
    query.bindValue(":employeeID", employeeID);
    query.bindValue(":name", name);
    query.bindValue(":cost_per_hour", costPerHour);

    if(query.exec()) {
        qDebug() << "Employee added successfully.";

        // default schedule for the next 30 days for the new employee
        QDate date = QDate::currentDate();
        for(int i = 0; i < 30; ++i) {
            QSqlQuery scheduleQuery(db);
            scheduleQuery.prepare("INSERT INTO EmployeeSchedule (employeeID, date, status) VALUES (:employeeID, :date, 'free')");
            scheduleQuery.bindValue(":employeeID", employeeID);
            scheduleQuery.bindValue(":date", date.addDays(i).toString("yyyy-MM-dd"));
            if (!scheduleQuery.exec()) {
                qDebug() << "Error inserting default schedule for new employee:" << scheduleQuery.lastError().text();
            }
        }
    } else {
        qDebug() << "Error adding employee:" << query.lastError().text();
    }
}

//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void employeewindow::on_deleteEmpbtn_clicked()
{
    //selectedItems list will contain pointers to the items currently selected in the emptableWidget
    QList<QTableWidgetItem*> selectedItems = ui->emptableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        qDebug() << "No employee selected.";
        return; // No employee selected, exit function
    }
    // Get the Employee ID from the selected row
    int selectedRow = selectedItems.first()->row();
    int employeeID = ui->emptableWidget->item(selectedRow, 0)->text().toInt(); //  Employee ID is in the first column

    // Ask for confirmation before deleting the employee
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this, "Delete Employee", "Are you sure you want to delete this employee?",
                                         QMessageBox::Yes | QMessageBox::No);
    if (confirmation == QMessageBox::No) {
        qDebug() << "Deletion canceled by user.";
        return;
    }

    // Perform deletion
    QSqlDatabase db = DatabaseManager::database();
    QSqlQuery query(db);

    query.prepare("DELETE FROM Employees WHERE employeeID = :employeeID");
    query.bindValue(":employeeID", employeeID);
    if (query.exec()) {
        qDebug() << "Employee deleted successfully.";
        // Refresh table data
        on_showEmpData_clicked(); // Reload data after deletion
    } else {
        qDebug() << "Error deleting employee:" << query.lastError().text();
    }
}

//-----------------------------------------------------------------------------------
//-------------------------------Show EmployeeData button----------------------------------------------------
void employeewindow::on_showEmpData_clicked()
{
    ui->emptableWidget->clearContents();//clear window if there are previuos values
    ui->emptableWidget->setRowCount(0);
    QSqlDatabase db = DatabaseManager::database();
    QSqlQuery query("SELECT * FROM Employees", db);

    int row = 0;

    while(query.next()) {
        int employeeID = query.value("employeeID").toInt();
        QString name = query.value("Employee_Name").toString();
        double costPerHour = query.value("Cost_per_hour").toDouble();

        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(employeeID));
        QTableWidgetItem *nameItem = new QTableWidgetItem(name);
        QTableWidgetItem *costItem = new QTableWidgetItem(QString::number(costPerHour));

        ui->emptableWidget->insertRow(row);
        ui->emptableWidget->setItem(row, 0, idItem); // Employee ID column
        ui->emptableWidget->setItem(row, 1, nameItem); // Employee Name column
        ui->emptableWidget->setItem(row, 2, costItem); // Cost Per Hour column

        row++;
        }
};
