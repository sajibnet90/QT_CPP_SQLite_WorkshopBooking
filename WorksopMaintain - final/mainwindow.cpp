// Filename: mainwindow.cpp

#include "mainwindow.h"
#include "ticketstatus.h"
#include "ui_mainwindow.h"
#include "employeewindow.h"
#include "createticket.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_showEmp_clicked()
{
    employeewindow* empWin = new employeewindow();
    empWin->show();
}


void MainWindow::on_actionExit_triggered()
{
    close();
}


void MainWindow::on_createTkt_clicked()
{
    createticket* newTkt = new createticket();
    newTkt->show();
}


void MainWindow::on_tktMng_clicked()
{
    ticketStatus* tktStatusWin = new ticketStatus();
    tktStatusWin->populateComboBoxWithTickets();
    tktStatusWin->show();
}

