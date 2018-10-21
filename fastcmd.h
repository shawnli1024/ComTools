#ifndef FASTCMD_H
#define FASTCMD_H

#include <QDialog>
#include <QSettings>
#include <QLineEdit>
#include <QCheckBox>
#include <QSignalMapper>
#include <QTabWidget>
#include "systemdefault.h"
#include <QRadioButton>

typedef struct
{
    QCheckBox   *pCheckBox;
    QLineEdit   *pEdit;
    QPushButton *pButton;
    int flag = 0;
}CmdWidgets;

namespace Ui {
class FastCmd;
}

class FastCmd : public QDialog
{
    Q_OBJECT

public:
    explicit FastCmd(QWidget *parent = 0);
    ~FastCmd();
    QString CalcXorCheckSumHex(QString m_StrData);
    char GetValue(char Value);
    QString m_strSumHex;

private:
    Ui::FastCmd *ui;
    SystemDefault *m_SD;

public:

    QTabWidget  *m_pTab;
    CmdWidgets *m_pWidgets[80];
    QByteArray GetData(int i);

    void connectWidgets();
    void saveSettings();
    void readSettings();

signals:
    void onSendFastCmd();

private slots:
    void on_button(int i);
    void on_pushButton_CheckSum_clicked();

};

#endif // FASTCMD_H
