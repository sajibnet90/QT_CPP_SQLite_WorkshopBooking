//Filename: ticketstatus.cpp
#include "ticketstatus.h"
#include "partsdialog.h"
#include "ui_ticketstatus.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>


ticketStatus::ticketStatus(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ticketStatus)
{
    ui->setupUi(this);
    populateComboBoxWithTickets(); //loading Combobox with tktID

    connect(ui->comboBoxTktid_tktStatus, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ticketStatus::onTicketIDChanged);
    /*currentIndexChanged is a predefined signal in the QComboBox class in Qt.
     * QOverload<int>::of(&QComboBox::currentIndexChanged) is used to connect to currentIndexChanged
     * signal of the QComboBox class with int prameter,
     * QOverload is a template class in QT. it connects to overload of currentIndexChanged
    */
    // Connect calculateTotal button click signal to the calculateTotalSlot
    connect(ui->calculateTotal, &QPushButton::clicked, this, &ticketStatus::calculateTotalSlot);
    //save ticket
    connect(ui->printTktStatus, &QPushButton::clicked, this, &ticketStatus::saveTicketStatusToFile);
    // Set the total cost label initially invisible
    ui->totalCostValue->setVisible(false);

    //updateTable();
}

//----------------------------TotalCost--------------------------------------------
void ticketStatus::calculateTotalSlot()
{
    // Get the work hours from the QLineEdit
    double workHours = ui->workHourInp->text().toDouble();

    // Initialize total cost
    double FinaltotalCost = 0.0;
    double totalCost = 0.0;


    // Iterate through each row in the tableWidgetTktstatus
    for (int row = 0; row < ui->tableWidgetTktstatus->rowCount(); ++row)
    {
        // Get the quantity and unit price from the QTableWidgetItem objects in the current row
        int quantity = ui->tableWidgetTktstatus->item(row, 2)->text().toInt();
        double unitPrice = ui->tableWidgetTktstatus->item(row, 3)->text().toDouble();

        // Calculate the cost for the current row and add it to the total
        totalCost += quantity * unitPrice;
    }

    // Multiply the total cost by the work hours and update the QLabel
    double workCost = workHours * 10;
    FinaltotalCost = workCost + totalCost;
    ui->totalCostValue->setText(QString::number(FinaltotalCost));
    ui->totalCostValue->setVisible(true);
}
//-------------------for updating the table when selected tktID changes ----------------------------------------------------

void ticketStatus::onTicketIDChanged(int index) {
    if (index >= 0) {
        int selectedTicketID = ui->comboBoxTktid_tktStatus->currentData().toInt();
        m_selectedTicketID = selectedTicketID;
        updateTable(); // Load parts for the selected ticket ID
    }
}

//------------------------------------------------------------------
ticketStatus::~ticketStatus()
{
    delete ui;
}

//---------------------------------load combobox with tktID func----------------------------------------------

void ticketStatus::populateComboBoxWithTickets() {
    // Clear the combobox in case there are any old values
    ui->comboBoxTktid_tktStatus->clear();

    QSqlDatabase db = DatabaseManager::database();
    QSqlQuery query(db);

    if(query.exec("SELECT tktID FROM Tickets ORDER BY tktID ASC")) { // Fetch ticket IDs in descending order
        while (query.next()) {
            int tktID = query.value(0).toInt();
            ui->comboBoxTktid_tktStatus->addItem(QString::number(tktID), tktID);
        }
    } else {
        qDebug() << "Database Error: " << query.lastError().text();
    }
}
//------------------------------------------------------------------
void ticketStatus::on_cancelTktstatus_clicked()
{
    close();
}

//----------------------------Add Parts button func--------------------------------------

void ticketStatus::on_addPartsbtn_clicked()
{
    // Check if a ticket ID is selected validation
    if (ui->comboBoxTktid_tktStatus->currentIndex() == -1) {
        QMessageBox::warning(this, "Warning", "Please select a ticket ID.");
        return;
    }
    m_selectedTicketID = ui->comboBoxTktid_tktStatus->currentData().toInt(); // Set m_selectedTicketID

    PartsDialog* partWin = new PartsDialog(this);// new instance of PartsDialog and 'this' as parent
    partWin->setTicketID(m_selectedTicketID); // calling setTicketID method from PartsDialog and pass tktID
    //establishes a connection between the partAdded signal emitted by PartsDialog (partWin obj)
    //and the updateTable slot of the current ticketStatus (this)
    connect(partWin, &PartsDialog::partAdded, this, &ticketStatus::updateTable);
    connect(partWin, &PartsDialog::partAdded, this, &ticketStatus::handlePartAdded);//testin signal
    partWin->exec();
}
//-------signal test-----------

void ticketStatus::handlePartAdded() {
    qDebug() << "Part added signal received!";
}



//-----------------------------updatetable()---------------------------------------
void ticketStatus::updateTable() {
    // Clear existing items in the table
    ui->tableWidgetTktstatus->clearContents();
    ui->tableWidgetTktstatus->setRowCount(0);

    // Retrieve parts information for the current ticket ID from the Parts table
    QSqlDatabase db = DatabaseManager::database();
    QSqlQuery query(db);
    query.prepare("SELECT partID, partName, quantity, unitPrice FROM Parts WHERE tktID = :tktID");
    query.bindValue(":tktID", m_selectedTicketID);
    if (!query.exec()) {
        qDebug() << "Error retrieving parts information:" << query.lastError().text();
        return;
    }

    // Populate the table with parts information
    int row = 0;
    while (query.next()) {
        QTableWidgetItem *partIDItem = new QTableWidgetItem(query.value(0).toString());
        QTableWidgetItem *partNameItem = new QTableWidgetItem(query.value(1).toString());
        QTableWidgetItem *quantityItem = new QTableWidgetItem(query.value(2).toString());
        QTableWidgetItem *unitPriceItem = new QTableWidgetItem(query.value(3).toString());

        ui->tableWidgetTktstatus->insertRow(row);
        ui->tableWidgetTktstatus->setItem(row, 0, partIDItem);
        ui->tableWidgetTktstatus->setItem(row, 1, partNameItem);
        ui->tableWidgetTktstatus->setItem(row, 2, quantityItem);
        ui->tableWidgetTktstatus->setItem(row, 3, unitPriceItem);

        row++;
    }

    ui->tableWidgetTktstatus->update();
}

//-----------------------------------saving to a text file----------------------------------------------
void ticketStatus::saveTicketStatusToFile() {
    // Get the selected ticket ID
    int tktID = ui->comboBoxTktid_tktStatus->currentData().toInt();

    // Get the total cost
    QString totalCost = ui->totalCostValue->text();

    // Get the parts information from the table
    QString partsInfo;
    int rowCount = ui->tableWidgetTktstatus->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QString partID = ui->tableWidgetTktstatus->item(row, 0)->text();
        QString partName = ui->tableWidgetTktstatus->item(row, 1)->text();
        QString quantity = ui->tableWidgetTktstatus->item(row, 2)->text();
        QString unitPrice = ui->tableWidgetTktstatus->item(row, 3)->text();
        partsInfo += QString("%1\t%2\t%3\t%4\n").arg(partID, partName, quantity, unitPrice);
    }

    // Open a file for writing
    QString fileName = QString("Ticket_%1_Status.txt").arg(tktID);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Ticket ID: " << tktID << "\n\n";
        out << "Parts Information:\n";
        out << "PartID\tPartName\tQuantity\tUnitPrice\n";
        out << partsInfo << "\n";
        out << "Total Cost: " << totalCost << "\n";
        file.close();
        QMessageBox::information(this, "File Saved", "Ticket status saved to file successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to save ticket status to file.");
    }
}

