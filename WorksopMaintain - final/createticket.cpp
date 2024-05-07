//Filename: createticket.cpp
#include "createticket.h"
#include "ui_createticket.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QtWidgets>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>


createticket::createticket(QWidget *parent)
    : QDialog(parent), ui(new Ui::createticket) {
    ui->setupUi(this);
    loadMechanicsIntoComboBox(); // load mechanics into the comboBox
    //connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int))); //same as below
    //currentIndexChanged is a predefined signal in the QComboBox class in Qt.
    connect(ui->comboBox, &QComboBox::currentIndexChanged, this, &createticket::on_comboBox_currentIndexChanged);
    connect(this, &createticket::newTicketCreated, this, &createticket::handleNewTicket); // signal testing purpose

}
void createticket::handleNewTicket(int tktID) {
    qDebug() << "New ticket created with ID:" << tktID;
}
//-------------------------------------
createticket::~createticket()
{
    delete ui;
}
//--------------------------------------load mechanic name to combo box function-----------------------------------------------
void createticket::loadMechanicsIntoComboBox() {
    QSqlDatabase db = DatabaseManager::database(); // Use DatabaseManager to get the database connection
    QSqlQuery query(db); // Use the database connection from DatabaseManager
    if (!query.exec("SELECT employeeID, employee_name FROM Employees")) {
        qDebug() << "Error querying mechanics from database:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        QString mechanicName = query.value(1).toString();
        ui->comboBox->addItem(mechanicName, QVariant(id));
    }
}

//----------------------------------------oncancel ticket-------------------------------------
void createticket::on_cancelTkt_clicked() {
    close();
}

//---------------------------------------- selecting emplyoee from combo box ----------------------

void createticket::on_comboBox_currentIndexChanged(int index)
{
    if (index < 0) return; // No selection
    QSqlDatabase db = DatabaseManager::database();
    QSqlQuery query(db);

    int employeeID = ui->comboBox->itemData(index).toInt(); // Retrieve the employee id

    qDebug() << "Selected Employee ID:" << employeeID;
    query.prepare("SELECT date, status FROM EmployeeSchedule WHERE employeeID = :employeeID");
    query.bindValue(":employeeID", employeeID);
    if (!query.exec()) {
        qDebug() << "Error querying employee schedule:" << query.lastError().text();
        return;
    }

     //formatting calender widget---
    QDate today = QDate::currentDate();
    QDate firstDayOfMonth = today.addDays(-today.day() + 1);
    QDate lastDayOfMonth = today.addMonths(1).addDays(-today.day());

    // Reset calendar highlights
    ui->calendarWidget->setDateTextFormat(QDate(), QTextCharFormat()); // Clear previous formats

    while (query.next()) {
        QDate date = query.value(0).toDate();
        QString status = query.value(1).toString();

        if (date >= firstDayOfMonth && date <= lastDayOfMonth) {
            QTextCharFormat format;

            // Mark "booked" days with a tooltip,
            if (status == "booked") {
                format.setToolTip("Booked");
                format.setBackground(Qt::red);
            } else if (status == "free") {
                format.setToolTip("Free");
                format.setBackground(Qt::green);
            }

            ui->calendarWidget->setDateTextFormat(date, format);
        }
    }
}

//-------------------Genrating pdf on save buttonclick--------------------------------------------------------------------

void createticket::on_saveTkt_clicked()
{
    QString registrationNumber = ui->regNum->text();
    QString brand = ui->brandName->text();
    QString model = ui->modelNum->text();
    QString problemDescription = ui->probDescription->toPlainText();
    QString mechanic = ui->comboBox->currentText();
    QDate appointmentDate = ui->calendarWidget->selectedDate();

    QSqlDatabase db = DatabaseManager::database();
    QSqlQuery query(db);

    // Start a transaction
    if (!db.transaction()) {
        qDebug() << "Error starting database transaction:" << db.lastError().text();
        return;
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS Tickets (tktID INTEGER PRIMARY KEY AUTOINCREMENT, registrationNumber TEXT, brand TEXT, model TEXT, problemDescription TEXT, mechanic TEXT, appointmentDate DATE)")) {
        qDebug() << "Error creating Tickets table: " << query.lastError().text();
    }

    query.prepare("INSERT INTO Tickets (registrationNumber, brand, model, problemDescription, mechanic, appointmentDate) "
                  "VALUES (:registrationNumber, :brand, :model, :problemDescription, :mechanic, :appointmentDate)");
    query.bindValue(":registrationNumber", registrationNumber);
    query.bindValue(":brand", brand);
    query.bindValue(":model", model);
    query.bindValue(":problemDescription", problemDescription);
    query.bindValue(":mechanic", mechanic);
    query.bindValue(":appointmentDate", appointmentDate.toString("yyyy-MM-dd")); // Bind appointmentDate

    if (!query.exec()) {
        qDebug() << "Error inserting into Tickets table:" << query.lastError().text();
        db.rollback(); // Roll back transaction if there's an error
        QMessageBox::critical(this, "Error", "Could not save the ticket. Please try again.");
        return;
    }

    if (!db.commit()) {
        qDebug() << "Error committing the transaction:" << db.lastError().text();
        QMessageBox::critical(this, "Error", "Could not commit the ticket data. Please try again.");
        return;
    }

    // Retrieve the auto-generated tktID
    QVariant lastId = query.lastInsertId();
    if (lastId.isValid()) {
        int tktID = lastId.toInt();
        qDebug() << "Generated Ticket ID:" << tktID;

        emit newTicketCreated(tktID); // Emit signal with the new tktID

        QMessageBox::information(this, "Save Ticket", "Ticket saved successfully. Ticket ID is: " + QString::number(tktID));
    } else {
        qDebug() << "Failed to retrieve the last inserted tktID.";
        QMessageBox::critical(this, "Error", "Could not retrieve the new ticket ID. Please try again.");
    }

    //------generating pdf-----------------//
    QString content = QString("Vehicle Information:\nRegistration Number: %1\nBrand: %2\nModel: %3\n\n"
                              "Problem Description:\n%4\n\nAssign a Mechanic: %5\nAppointment Date: %6")
                          .arg(registrationNumber)
                          .arg(brand)
                          .arg(model)
                          .arg(problemDescription)
                          .arg(mechanic)
                          .arg(appointmentDate.toString());

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("./ticket.pdf"); // output file path

    QPainter painter(&printer);
    QStringList lines = content.split('\n');
    // Set initial drawing position
    int x = 100;
    int y = 100;
    int lineSpacing = 20;
    // Set a regular font for body text
    QFont textFont("Arial", 10, QFont::Normal);
    painter.setFont(textFont);
    // Iterate over lines to draw each one
    for (const QString &line : lines) {
        painter.drawText(x, y, line);
        y += lineSpacing; // Move to the next line position
    }

    painter.end();
}
