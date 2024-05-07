// Filename: databasemanager.h

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager {
public:
    static void initialize();
    static QSqlDatabase database();

private:
    static QSqlDatabase DB_Connection;
};

#endif // DATABASEMANAGER_H
