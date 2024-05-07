#include "mainwindow.h"
#include "databasemanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DatabaseManager::initialize(); // Initialize the database connection

    MainWindow w;
    w.show();
    return a.exec();
}
