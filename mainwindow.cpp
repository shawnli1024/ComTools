#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort(this);
    settings = new SettingsDialog;
    pDataShow = new DataShow(this);
    pFastCmd = new FastCmd(this);

    ui->actionDisconnect->setEnabled(false);
    ui->actionSaveFile->setEnabled(true);
    ui->actionSaveFile->setEnabled(false);

    i4FlagSave = 0;

    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionSetting, SIGNAL(triggered()), settings, SLOT(show()));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(pFastCmd,SIGNAL(onSendFastCmd()),this,SLOT(readAll()));
    connect(pDataShow,SIGNAL(onSendMsg()),this,SLOT(SendMsg()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(onclose()));
    connect(ui->actionSaveFile, SIGNAL(triggered()), this, SLOT(saveFile()));//保存文件


    m_dataShow = new QDockWidget("DataShow",this);
    m_dataShow->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_dataShow->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_dataShow->setWidget(pDataShow);

    m_pFastCmd = new QDockWidget("FastCmd",this);
    m_pFastCmd->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_pFastCmd->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_pFastCmd->setWidget(pFastCmd);


    this->setCentralWidget(m_dataShow);
    addDockWidget(Qt::RightDockWidgetArea, m_pFastCmd);
}

MainWindow::~MainWindow()
{
    if(1 == i4FlagSave)
    {
        outFile.close();
    }
    delete ui;
}

void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite))
    {

        ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                   .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                   .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));

        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionSetting->setEnabled(false);
        ui->actionSaveFile->setEnabled(true);

    }
    else
    {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        ui->statusBar->showMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if(1 == i4FlagSave)
    {
        outFile.close();
        i4FlagSave = 2;
    }
    serial->close();
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionSetting->setEnabled(true);
    ui->actionSaveFile->setEnabled(false);
}

void MainWindow::readData()
{
    i1 data;
    while(!serial->atEnd())
    {
        data = serial->read(1).at(0);
        if(pDataShow->isVisible())
        {
          pDataShow->writeData(data);
        }
        else
        {
            pDataShow->flag = 1;
            pDataShow->on_pushButton_pause_clicked();
            pDataShow->on_pushButton_clear_clicked();
        }
        if(1 == i4FlagSave)
        {
            outFile.write(&data,1);
        }
        else if(2 == i4FlagSave)
        {
            outFile.close();
        }
    }
}

void MainWindow::readAll()
{
    for(int i=0;i<80;i++)
    {
        if(pFastCmd->m_pWidgets[i]->flag)
        {
            QByteArray strData = pFastCmd->GetData(i);
            serial->write(strData);
            pFastCmd->m_pWidgets[i]->flag = 0;
        }
    }
}

void MainWindow::SendMsg()
{
    serial->write(pDataShow->GetData());
}
void MainWindow::saveFile()
{
   if(0 == i4FlagSave)
   {
       QString fileName;
       fileName.append(QString("%1_%2_%3")
                       .arg(settings->settings().name)
                       .arg(QDate::currentDate().toString("yyyyMMdd"))
                       .arg(QTime::currentTime().toString("HHmmss"))
                       );
       fileName = QFileDialog::getSaveFileName(this, tr("Save File"), fileName, tr("Data (*.dat *.txt)"));
       fileName += tr("Hex.dat");
       if(!fileName.isEmpty())
       {
           outFile.setFileName(fileName);
           if(!outFile.open(QIODevice::WriteOnly|QIODevice::Append))
           {
               qDebug()<<"打开文件失败"<<"\r\n";
               return;
           }
       }
       if(outFile.isOpen())
       {
           i4FlagSave = 1;  //标志位，为1时开始保存数据
           ui->actionSaveFile->setIcon(QIcon(":/images/SaveFile_Close.png"));
       }
   }
   else if(1 == i4FlagSave)
   {
        i4FlagSave =0;
        outFile.close();
        ui->actionSaveFile->setIcon(QIcon(":/images/SaveFile.png"));
   }
}

void MainWindow::onclose()
{
    if(1 == i4FlagSave)
    {
        outFile.close();
    }
    close();
}
