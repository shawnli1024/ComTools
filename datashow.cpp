#include "datashow.h"
#include "ui_datashow.h"
#include <QTextCodec>
DataShow::DataShow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataShow)
{
    ui->setupUi(this);
    datasave = 0;
    flag = 1;
    ui->pushButton_pause->setText("暂停");
    ui->checkBox_Hex->setChecked(true);
}

DataShow::~DataShow()
{
    delete ui;
}
void DataShow::writeData(char data)
{
    QByteArray bytedata;
    QString strTemp,strHex;
    QTextCodec *codec = QTextCodec::codecForName("GBK");

    if(1 == flag)  //1显示  0暂停
    {
        if(ui->checkBox_Hex->isChecked())  //暂时不显示16进制方式
        {
            bytedata.append(data);
            strTemp = codec->toUnicode(bytedata.toHex());            
            strHex += (strTemp.left(2) + " ");
            ui->textEdit_DataShow->moveCursor(QTextCursor::End);
            ui->textEdit_DataShow->insertPlainText(strHex);
        }
        else
        {
            if(0x0D == datasave && 0x0A ==data)  //  /r/n在这里换行了2次，再次判断当为0D0A时，不显示0A以达到只换行一次的效果
                return;
            QString strdata;
            strdata.append(data);
            ui->textEdit_DataShow->moveCursor(QTextCursor::End);
            ui->textEdit_DataShow->insertPlainText(strdata);
            datasave=data;
        }
    }
}

QByteArray DataShow::GetData()
{
    QByteArray str = "\r\n";
    QByteArray strData = ui->textEdit_Send->toPlainText().toLocal8Bit();//m_pWidgets[i]->pEdit->text().toLocal8Bit();
    if(ui->checkBox_Hex_Send->isChecked())
    {
        strData = QByteArray::fromHex(strData);
        return strData;
    }
    else
    {
        return strData+str;
    }
}


void DataShow::on_pushButton_clear_clicked()
{
    ui->textEdit_DataShow->clear();
}

void DataShow::on_pushButton_pause_clicked()
{
    if(0 == flag)
    {
        flag = 1;
        ui->pushButton_pause->setText("暂停");
    }
    else if(1 == flag)
    {
        flag = 0;
        ui->pushButton_pause->setText("显示");
    }
}

void DataShow::on_pushButton_Clear_Send_clicked()
{
    ui->textEdit_Send->clear();
}

void DataShow::on_pushButton_Send_clicked()
{
    emit onSendMsg();
}
