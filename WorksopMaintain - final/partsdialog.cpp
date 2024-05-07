//Filename: partsdialog.cpp
#include "partsdialog.h"
#include "ui_partsdialog.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

PartsDialog::PartsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PartsDialog)
{
    ui->setupUi(this);
}

PartsDialog::~PartsDialog()
{
    delete ui;
}
//--------------------to set a part under a tktID--------------------------
void PartsDialog::setTicketID(int tktID) {
    m_associatedTicketID = tktID;
}
//--------------------------------------------------------------------
void PartsDialog::on_addPart_clicked()
{
    // Validate input from QLineEdit fields
    QString partID = ui->partID->text();
    QString partName = ui->partName->text();
    int quantity = ui->quantity->text().toInt();
    double unitPrice = ui->unitPrice->text().toDouble();

    // Perform validation checks
    if (partID.isEmpty() || partName.isEmpty() || quantity <= 0 || unitPrice <= 0 || m_associatedTicketID <= 0) {
        QMessageBox::critical(this, "Error", "Invalid input. Please provide valid values for all fields.");
        return;
    }

    // Insert the new part into the Parts table
    QSqlDatabase db = DatabaseManager::database();
    QSqlQuery query(db);

    // Create the Parts table schema if it does not exist
    if (!query.exec("CREATE TABLE IF NOT EXISTS Parts ("
                    "partID INTEGER PRIMARY KEY,"
                    "tktID INTEGER,"
                    "partName TEXT,"
                    "quantity INTEGER,"
                    "unitPrice REAL,"
                    "FOREIGN KEY(tktID) REFERENCES Tickets(tktID)"
                    ")")) {
        qDebug() << "Error creating Parts table:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to create Parts table. Please try again.");
        return;
    }

    // Insert the new part into the Parts table
    query.prepare("INSERT INTO Parts (tktID, partID, partName, quantity, unitPrice) "
                  "VALUES (:tktID, :partID, :partName, :quantity, :unitPrice)");
    query.bindValue(":tktID", m_associatedTicketID);
    query.bindValue(":partID", partID);
    query.bindValue(":partName", partName);
    query.bindValue(":quantity", quantity);
    query.bindValue(":unitPrice", unitPrice);

    if (!query.exec()) {
        qDebug() << "Error inserting part into Parts table:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to add part. Please try again.");
        return;
    }
    // Emit signal to notify ticket status dialog about the added part
    emit partAdded();
    QMessageBox::information(this, "Success", "Part added successfully.");
    close();
}

void PartsDialog::on_cancelPart_clicked()
{
    close();
}

