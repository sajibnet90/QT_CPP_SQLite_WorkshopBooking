// Filename: databasemanager.cpp

#include "databasemanager.h"
#include <QSqlDatabase>
#include <QDebug>

QSqlDatabase DatabaseManager::DB_Connection;

void DatabaseManager::initialize() {
    DB_Connection = QSqlDatabase::addDatabase("QSQLITE");
    DB_Connection.setDatabaseName("workshopDB.db");
    if (!DB_Connection.open()) {
        qDebug() << "Error: unable to open database!";
    }
}

QSqlDatabase DatabaseManager::database() {
    return DB_Connection;
}
