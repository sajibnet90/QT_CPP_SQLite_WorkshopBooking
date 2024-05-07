//Filename: ticketstatus.h

#ifndef TICKETSTATUS_H
#define TICKETSTATUS_H

#include <QDialog>

namespace Ui {
class ticketStatus;
}

class ticketStatus : public QDialog
{
    Q_OBJECT

public:
    explicit ticketStatus(QWidget *parent = nullptr);
    ~ticketStatus();

void populateComboBoxWithTickets();
//void loadParts();


private slots:
    void on_cancelTktstatus_clicked();
    void on_addPartsbtn_clicked();
    void updateTable();
    void handlePartAdded(); //test signal emit

    void onTicketIDChanged(int index);
    void calculateTotalSlot();

public slots:
    void saveTicketStatusToFile();


private:
    Ui::ticketStatus *ui;
    int m_selectedTicketID; // Member variable to store the selected ticket ID
};


#endif // TICKETSTATUS_H
