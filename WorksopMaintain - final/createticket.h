#ifndef CREATETICKET_H
#define CREATETICKET_H

#include <QDialog>

namespace Ui {
class createticket;
}

class createticket : public QDialog
{
    Q_OBJECT

public:
    explicit createticket(QWidget *parent = nullptr);
    ~createticket();

private slots:
    void on_cancelTkt_clicked();
    void loadMechanicsIntoComboBox();

    void on_comboBox_currentIndexChanged(int index);

    void on_saveTkt_clicked();

    void handleNewTicket(int tktID); //slot for test


signals:
    void newTicketCreated(int tktID);//test signal


private:
    Ui::createticket *ui;
};

#endif // CREATETICKET_H
