#include "fastcmd.h"
#include "ui_fastcmd.h"
#include <QTextCodec>
#include <QMessageBox>

FastCmd::FastCmd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FastCmd)
{
    ui->setupUi(this);
    m_pTab = ui->tabWidget;
    m_SD = new SystemDefault();

    connectWidgets();
    readSettings();
    ui->radioButton_CRC->setChecked(true);
}

FastCmd::~FastCmd()
{
    saveSettings();
    delete ui;
}

void FastCmd::connectWidgets()
{
    QSignalMapper *pMapper = new QSignalMapper(this);
    connect(pMapper,SIGNAL(mapped(int)),this,SLOT(on_button(int)));
    for(int i=0;i<80;i++)
    {
       m_pWidgets[i] = new CmdWidgets;
       m_pWidgets[i]->pButton = new QPushButton;
       m_pWidgets[i]->pButton->setText(tr("发送"));
       m_pWidgets[i]->pButton->setMaximumSize(40,23);

       m_pWidgets[i]->pCheckBox = new QCheckBox;
       m_pWidgets[i]->pCheckBox->setText("Hex");

       m_pWidgets[i]->pEdit = new QLineEdit;
       m_pWidgets[i]->pEdit->setStyleSheet("background-color: rgb(255,255,255)");
       m_pWidgets[i]->pEdit->setMinimumWidth(300);

       connect(m_pWidgets[i]->pButton,SIGNAL(clicked()),pMapper,SLOT(map()));
       pMapper->setMapping(m_pWidgets[i]->pButton,i);
    }

    for(int j=0;j<5;j++)
    {
        QVBoxLayout *layoutH = new QVBoxLayout;
        for(int i=0;i<16;i++)
        {
            QHBoxLayout *layout = new QHBoxLayout;
            layout->addWidget(m_pWidgets[i+16*j]->pEdit);
            layout->addWidget(m_pWidgets[i+16*j]->pCheckBox);
            layout->addWidget(m_pWidgets[i+16*j]->pButton);
            layoutH->addLayout(layout);
        }
        m_pTab->widget(j)->setLayout(layoutH);

    }
    m_pTab->setCurrentIndex(0);
}

void FastCmd::on_button(int i)
{
    m_pWidgets[i]->flag = 1;
    emit onSendFastCmd();
}

QByteArray FastCmd::GetData(int i)
{
    QByteArray str = "\r\n";
    //QTextCodec *codec = QTextCodec::codecForName("GBK");
    //QByteArray byte;
    //char checkSum = 0;
    //QString strData0 = m_pWidgets[i]->pEdit->text();
    QByteArray strData = m_pWidgets[i]->pEdit->text().toLocal8Bit();
    if(m_pWidgets[i]->pCheckBox->isChecked())
    {
        strData = QByteArray::fromHex(strData);
        return strData;
    }
    else
    {
        /*
        for(int i=1;i<strData0.length()-1;i++)
        {
            checkSum ^= strData0.at(i).toLatin1();
        }

        byte.append(checkSum);
        strData.append(codec->toUnicode(byte.toHex()));
        */
        return strData+str;
    }
}

void FastCmd::readSettings()
{
    QSettings settings("HWA_QXJ","QXJ_Qt");
    for(int i=0;i<80;i++)
    {
        m_pWidgets[i]->pCheckBox->setChecked(settings.value(QString("Check%1").arg(i)).toBool());
        m_pWidgets[i]->pEdit->setText(settings.value(QString("Cmd%1").arg(i)).toString());
    }
}

void FastCmd::saveSettings()
{
    QSettings settings("HWA_QXJ","QXJ_Qt");
    for(int i=0;i<80;i++)
    {
        settings.setValue(QString("Check%1").arg(i),m_pWidgets[i]->pCheckBox->isChecked());
        settings.setValue(QString("Cmd%1").arg(i),m_pWidgets[i]->pEdit->text());
    }
}


/*
void FastCmd::UpdateCPM(const TCPM *pCPM)
{
    ui->label_CPM_out->setText(QString("%1").arg(" "));
    QString strFlag;
    if('C' == pCPM->i1Flag_CZ)
    {
        strFlag = "测试码";
    }
    else if('Z' == pCPM->i1Flag_CZ)
    {
        strFlag = "正式码";
    }
    else
    {
        strFlag = "错误";
    }
    ui->label_CPM_out->setText(QString("%1").arg(strFlag));
}
*/


void FastCmd::on_pushButton_CheckSum_clicked()
{
    char CheckSum = 0;
    QString strTemp;
    QByteArray byte;
    QString strdata = ui->textEdit_data->toPlainText();

    //if(ui->checkBox_Hex->isChecked())
    if(1)
    {
        strTemp = CalcXorCheckSumHex(strdata);
        if(1 == strTemp.length())
            strTemp.insert(0,"0");
    }
    else
    {
        for(int i=0; i<strdata.length();i++)
        {
            CheckSum ^= strdata.at(i).toLatin1();
        }

        QTextCodec *codec = QTextCodec::codecForName("GBK");
        byte.append(CheckSum);
        strTemp = codec->toUnicode(byte.toHex());
    }
    strTemp.insert(0,"0x");
    ui->label_CheckSum->setText(QString("%1").arg(strTemp));
}

QString FastCmd::CalcXorCheckSumHex(QString m_StrData)
{
    // TODO: Add your control notification handler code here
    char Value;
    char flag = 1;
    long sum = 0;

    unsigned int checkSum=0;  //--
    unsigned short checkSumCrc=0;
    char flag_num=0;
    m_strSumHex = "";

    int i4Num = 0;
    unsigned char i1buf[100*1024] = {0};

    for (int a=0; a<m_StrData.length(); a++)
    {
        Value = m_StrData.at(a).toLatin1();
        if (' '==Value)
        {
            if (2 == flag_num)
            {
                flag_num = 0;
            }
            else if(1 == flag_num)
            {
                QMessageBox::warning(this, tr("Error"),tr("数据不规范!如十六进制“F”,应写为“0F”!"));
                return "";
            }
            flag = 1;
            continue;
        }
        else
        {
            if (1 == flag)
            {
                if (GetValue(Value) == 20)
                    return "";
                sum += GetValue(Value)*16;
                flag++;
                if (0 == flag_num)
                {
                    flag_num = 1;
                }
            }
            else if(2 == flag)
            {
                if (GetValue(Value) == 20)
                     return "";
                sum += GetValue(Value);
                i1buf[i4Num] = sum;
                i4Num++;

                if(ui->radioButton_XOR->isChecked())
                {
                    checkSum ^= sum;
                }
                else if(ui->radioButton_Sum->isChecked())
                {
                    checkSum += sum;
                }
                //checkSum ^= sum;  //--
                sum = 0;  //--

                flag++;
                if (1 == flag_num)
                {
                    flag_num = 2;
                }
            }
            else
            {
                QMessageBox::warning(this, tr("Error"),tr("数据格式有误,请输入单字节十六进制数!"));
                return "";
            }
        }
    }
    if (1 == flag_num)
    {
        QMessageBox::warning(this, tr("Error"),tr("数据不规范!如十六进制“F”,应写为“0F”!"));
        return "";
    }
    if(ui->radioButton_CRC->isChecked())
    {
        m_SD->CRC16(i1buf, i4Num, &checkSumCrc);
        m_strSumHex =  QString("%1").arg(checkSumCrc,4,16,QLatin1Char('0'));
    }
    else
    {
        m_strSumHex =  QString("%1").arg(checkSum,4,16,QLatin1Char('0'));
    }
    //m_strSumHex =  itoa(checkSum, strHex, 16);    
    m_strSumHex = m_strSumHex.toUpper();
    return m_strSumHex;
}


char FastCmd::GetValue(char Value)
{
    char temp;
    if(Value>='0' && Value<='9')
    {
        temp = Value - '0';
    }
    else if (Value>='A' && Value<='F')
    {
        temp = Value - 'A' +10;
    }
    else if(Value>='a' && Value<='f')
    {
        temp = Value - 'a' +10;
    }
    else
    {
        QMessageBox::warning(this, tr("Error"),tr("数据有误！"));
        temp = 20;
    }
    return temp;
}
