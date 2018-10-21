#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDockWidget>
#include <QFile>
#include <QDate>
#include <QFileDialog>
#include <QDebug>


#include "settingsdialog.h"
#include "typedefine.h"
#include "datashow.h"
#include "fastcmd.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    SettingsDialog *settings;
    QSerialPort *serial;
    DataShow *pDataShow;
    FastCmd  *pFastCmd;

    QDockWidget *m_dataShow;
    QDockWidget *m_pFastCmd;

    int i4FlagSave;
    QFile outFile;


private slots:
    void openSerialPort();
    void closeSerialPort();
    void readAll();
    void readData();
    void SendMsg();
    void saveFile();
    void onclose();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
