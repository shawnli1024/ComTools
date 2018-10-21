#ifndef DATASHOW_H
#define DATASHOW_H

#include <QDialog>

namespace Ui {
class DataShow;
}

class DataShow : public QDialog
{
    Q_OBJECT

public:
    explicit DataShow(QWidget *parent = 0);
    ~DataShow();
    void writeData(char data);
    //void writeData(const QByteArray &byteArray);
    char datasave;
    int flag;
    QByteArray GetData();

public slots:
    void on_pushButton_clear_clicked();
    void on_pushButton_pause_clicked();
    void on_pushButton_Clear_Send_clicked();
    void on_pushButton_Send_clicked();

signals:
    void onSendMsg();

private:
    Ui::DataShow *ui;
};

#endif // DATASHOW_H
