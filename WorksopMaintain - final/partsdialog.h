//Filename: partsdialog.h
#ifndef PARTSDIALOG_H
#define PARTSDIALOG_H

#include <QDialog>

namespace Ui {
class PartsDialog;
}

class PartsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PartsDialog(QWidget *parent = nullptr);
    ~PartsDialog();

private slots:
    void on_addPart_clicked();

    void on_cancelPart_clicked();
public: void setTicketID(int tktID);

signals:
    void partAdded();

private:
    Ui::PartsDialog *ui;
    int m_associatedTicketID; // Member variable to store the associated ticket ID
};

#endif // PARTSDIALOG_H
