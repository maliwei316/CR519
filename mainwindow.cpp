#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <bitsoperation.h>
#include <QTime>
#include <QFileDialog>
#include <QDir>
#include <clstooling.h>
#include <QDebug>
#include "myevent.h"
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTranslator>

extern bool loggingEnable;
extern quint8 loggingLevel;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->tableWidget_lastCycleData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //x先自适应宽度
    this->ui->tableWidget_lastCycleData->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列,不指定则所有列自适应
    //this->ui->tableWidget_lastCycleData->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    //this->ui->tableWidget_lastCycleData->horizontalHeader()->setSectionResizeMode(9, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列

    this->ui->tableView_partCycleData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所有列都扩展自适应宽度，填充充满整个屏幕宽度
    this->ui->tableView_partCycleData->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents );//根据列内容来定列宽
            //horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);//对第0列单独设置固定宽度
            //setColumnWidth(0, 45);//设置固定宽度
    this->ui->tableView_pointCycleData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所有列都扩展自适应宽度，填充充满整个屏幕宽度
    this->ui->tableView_pointCycleData->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents );//根据列内容来定列宽


    //welcome/startup page
    this->ui->stackedWidget_mainProgram->setCurrentIndex(9);
    //
    this->clsBarcode_left=new clsBarcode(this);
    this->clsBarcode_right=new clsBarcode(this);
    this->wp1=nullptr;
    this->tempTooling_editting=new clsTooling(this);
    this->tooling_current=new clsTooling(this);
    this->toolID_editing=1;
    this->tempTooling_editting->plcToolingInfo.toolingNO=1;
    this->machineInfoReady=false;
    //QString filename="HistoryData"+QString::number(QDateTime::currentDateTime().date().year())+".sqlite3";

    dbh_1.onInit("QSQLITE","HistoryDataDB_run","HistoryData"+QString::number(QDateTime::currentDateTime().date().year())+".sqlite3");

    this->ui->lineEdit_selectedDataBase->setText("HistoryData"+QString::number(QDateTime::currentDateTime().date().year())+".sqlite3");
    this->model_partData = new QSqlQueryModel(this);
    this->model_pointsData=new QSqlQueryModel(this);
    //barcode port combbox init
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos)
    {
       this->ui->comboBox_BarcodePort_Left->addItem(serialPortInfo.portName());
       this->ui->comboBox__BarcodePort_Right->addItem(serialPortInfo.portName());
    }
    //load machine info
    if(QFile::exists("machineInfo.txt"))
    {
        qDebug()<<"found machineInfo.txt";
        QFile loadFile("machineInfo.txt");
        if(loadFile.open(QIODevice::ReadOnly))
        {
             qDebug()<<"success to open machineInfo.txt";
            QDataStream in1(&loadFile);
           in1>>machineInfo1.machineName>>machineInfo1.PLC_IPAddress
                   >>machineInfo1.frequencyLimit_upper>>machineInfo1.frequencyLimit_lower;
           this->machineInfoReady=true;
           this->ui->spinbox_pointPara_freeRunFreq->setMinimum(machineInfo1.frequencyLimit_lower);
           this->ui->spinbox_pointPara_freeRunFreq->setMaximum(machineInfo1.frequencyLimit_upper);
           this->ui->spinbox_pointPara_freq_u_limit->setMinimum(machineInfo1.frequencyLimit_lower);
           this->ui->spinbox_pointPara_freq_u_limit->setMaximum(machineInfo1.frequencyLimit_upper);
           this->ui->spinbox_pointPara_freq_l_limit->setMinimum(machineInfo1.frequencyLimit_lower);
           this->ui->spinbox_pointPara_freq_l_limit->setMaximum(machineInfo1.frequencyLimit_upper);
            qDebug()<<tr("machineName:%1,PLC IP:%2,Frq_upper:%3,Frq_lower:%4,machineInfoReady:%5").arg(machineInfo1.machineName)
                      .arg(machineInfo1.PLC_IPAddress).arg(machineInfo1.frequencyLimit_upper)
                      .arg(machineInfo1.frequencyLimit_lower).arg(this->machineInfoReady);
        loadFile.close();
        }
    }

    this->logInStatus=false;
    //load tooling settings
    if(QFile::exists("toolingConfig/currentTooling.dc"))
    {
        qDebug()<<"found currentTooling.dc";
        this->tempTooling_editting->loadFromDisk("toolingConfig/currentTooling.dc");
        this->toolID_editing=this->tempTooling_editting->plcToolingInfo.toolingNO;
        this->ui->toolID_Editting->setValue(this->toolID_editing);
        //barcode init-left
        if(this->tempTooling_editting->leftBarcodeSettings.enable)
        {
            this->barcodeInit(this->clsBarcode_left,this->tempTooling_editting->leftBarcodeSettings);
        }
        else
        {
            qWarning()<<tr("left barcode is disabled");
            if(loggingEnable&&loggingLevel>0)
            {
                QString logcontents=tr("left barcode is disabled");
                emit this->logRequest(logcontents,200,0);
            }
        }
        //barcode init-right
        if(this->tempTooling_editting->rightBarcodeSettings.enable)
        {
            this->barcodeInit(this->clsBarcode_right,this->tempTooling_editting->rightBarcodeSettings);
        }
        else
        {
            qWarning()<<tr("right barcode is disabled");
            if(loggingEnable&&loggingLevel>0)
            {
                QString logcontents=tr("right barcode is disabled");
                emit this->logRequest(logcontents,200,0);
            }
        }
    }
    else
    {
        qWarning()<<tr("did not find currentTooling.dc");
        if(loggingEnable&&loggingLevel>0)
        {
            QString logcontents=tr("did not find currentTooling.dc");
            emit this->logRequest(logcontents,200,0);
        }
    }
    this->toolID_PLC=0;
    this->setPLCValueVisible(false);
    this->uploadingWholeSettingFromPLCInProcess=false;
    this->timer_mainWindow.setInterval(10000);
    connect(&this->timer_mainWindow,&QTimer::timeout,this,&MainWindow::OnTimer_mainWindow_Timeout);
    this->timer_mainWindow.start();

    connect(&dbh_1,&DB_Handler::logRequest,this,&MainWindow::execLogging);

    connect(this,&MainWindow::updateAlarmText,this,&MainWindow::onUpdateAlarmText);
    connect(this,&MainWindow::moveAlarmToHistory,this,&MainWindow::onMoveAlarmToHistory);
    connect(this,&MainWindow::logRequest,this,&MainWindow::execLogging);
    connect(this,&MainWindow::receivedPointCycleData,this,&MainWindow::onReceivedPointCycleData);
    connect(this,&MainWindow::parameterEditableStausChanged,this,&MainWindow::onParameterEditableStausChanged);

    connect(this->clsBarcode_left,&clsBarcode::readyRead,this,&MainWindow::getBarcode_left);
    connect(this->clsBarcode_left,&clsBarcode::errorOccurred,this,&MainWindow::handleBarcodeError);
    connect(this->clsBarcode_right,&clsBarcode::readyRead,this,&MainWindow::getBarcode_right);
    connect(this->clsBarcode_right,&clsBarcode::errorOccurred,this,&MainWindow::handleBarcodeError);

    if(QFile::exists("sysRegText.txt"))
    {
        QFile loadFile("sysRegText.txt");
        QString textLoaded;
        if(loadFile.open(QIODevice::ReadOnly))
        {
            QDataStream in1(&loadFile);

            if(loadFile.size()>0)
            {
               in1>>textLoaded>>this->systemRegisteredTextList;
            }

            if(this->systemRegisteredTextList.value(quint32(1001)).contains("home"))
                this->LanguageFlag=1;
            else
                this->LanguageFlag=0;
            QTranslator translator;
            bool loadLangFileResult;
            if(LanguageFlag ==0)
                   loadLangFileResult=translator.load("CR519_CN.qm");
            else if(LanguageFlag ==1)
                   loadLangFileResult=translator.load("CR519_EN.qm");
            qDebug()<<tr("load file result:%1,fileName:%2").arg(loadLangFileResult?"OK":"failed").arg(LanguageFlag==0?"CR519_CN.qm":"CR519_EN.qm");
            if(loadLangFileResult)
            {
                bool installTranslatorResult;
                installTranslatorResult=qApp->installTranslator(&translator);
                qDebug()<<tr("install translator result result:%1").arg(installTranslatorResult?"OK":"failed");
                if(installTranslatorResult)
                {
                    ui->retranslateUi(this);
                }
            }


        loadFile.close();
        }
    }

}

MainWindow::~MainWindow()
{
    delete ui;
    delete wp1;
    //tempTooling_editting->deleteLater();
    //tooling_current->deleteLater();

}
bool MainWindow::barcodeInit(clsBarcode* clsbarcode,const barcodeSetting& barcode_settings)
{
    clsbarcode->setPortName(barcode_settings.portName);
    QString baudstr=barcode_settings.baud;
    quint32 baud_quint32;
    if(baudstr=="4800")
    {
       baud_quint32=4800;
    }
    else if(baudstr=="19200")
    {
        baud_quint32=19200;
    }
    else if(baudstr=="38400")
    {
        baud_quint32=38400;
    }
    else if(baudstr=="57600")
    {
        baud_quint32=57600;
    }
    else if (baudstr=="115200")
    {
        baud_quint32=115200;
    }
    else
    {
       baud_quint32=9600;
    }
    clsbarcode->setBaudRate(baud_quint32);
    clsbarcode->setDataBits((QSerialPort::DataBits)this->tempTooling_editting->leftBarcodeSettings.dataBits);
    clsbarcode->setStopBits((QSerialPort::StopBits)this->tempTooling_editting->leftBarcodeSettings.stopBits);
    QString parity_str=barcode_settings.parity.toUpper();
    quint8  parity_uint8;
    if(parity_str=="EVEN")
    {
       parity_uint8=2;
    }
    else if (parity_str=="ODD")
    {
        parity_uint8=3;
    }
    else
    {
        parity_uint8=0;
    }
    clsbarcode->setParity((QSerialPort::Parity)parity_uint8);
    if(!clsbarcode->open(QIODevice::ReadOnly))
    {
        qWarning()<<tr("failed to open serial port for barcode,portName:%1,error:%2")
                    .arg(barcode_settings.portName)
                    .arg(clsbarcode->errorString());
        if(loggingEnable&&loggingLevel>0)
        {
            QString logcontents=tr("Time:%1,failed to open serial port for barcode,portName:%2,error:%3")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"))
                    .arg(barcode_settings.portName)
                    .arg(clsbarcode->errorString());
            emit this->logRequest(logcontents,200,0);
        }
        return false;
    }
    else
    {
        qDebug()<<tr("Success to open serial port for barcode,portName:%1")
                  .arg(barcode_settings.portName);
        if(loggingEnable&&loggingLevel>0)
        {
            QString logcontents=tr("Time:%1,Success to open serial port for barcode,portName:%2")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"))
                    .arg(barcode_settings.portName);
            emit this->logRequest(logcontents,205,0);
        }
        clsbarcode->setDataTerminalReady(true);

        return true;
    }

}
void MainWindow::execLogging(QString logContents,quint16 logID,quint8 logLevel)
{

    QString newLogItems=tr("logID:%1,LogLevel:%2,LogContents:%3")
                        .arg(logID).arg(logLevel).arg(logContents);
    this->recentRunningLogs.append(newLogItems);
    this->ui->textEdit_runningLog->append(newLogItems);
    if(this->recentRunningLogs.size()>500)
    {

        if(QFile::exists("runLog.txt"))
        {
            QFile file1("runLog.txt");
            if(file1.size()>1048576)
            {
                if(QFile::exists("runLog_old.txt"))
                   { QFile::remove("runLog_old.txt");}
                file1.rename("runLog_old.txt");
            }
        }

        QStringList first300Items=this->recentRunningLogs.mid(0,300);
        QStringList latestItems=this->recentRunningLogs.mid(301,this->recentRunningLogs.size()-1);

        QFile loadFile("runLog.txt");

             if (!loadFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
                 qWarning("Couldn't open  file when try to update runlog to disk");
                 this->ui->textEdit_runningLog->append("!!!Couldn't open  file when try to update runlog to disk");
                 return;
             }
             else
             {

                 qDebug()<<"file opened,file.size:"<<loadFile.size();
                 QTextStream Out1(&loadFile);
                 for(int i=0;i<first300Items.size();i++)
                 {
                     Out1<<first300Items.at(i)+"\n";

                 }

                 qDebug()<<"save to file execed, file.size()"<<loadFile.size();
                 loadFile.close();
                 this->recentRunningLogs.clear();
                 this->recentRunningLogs.append(latestItems);
                 QString logsTodisplay;
                 for(int i=0;i<this->recentRunningLogs.size();i++)
                 {
                     logsTodisplay.append(this->recentRunningLogs.at(i)+"\n");
                 }
                 this->ui->textEdit_runningLog->clear();
                 this->ui->textEdit_runningLog->setText(logsTodisplay);
             }
    }

}
void MainWindow::changePage(quint16 targetPageIndex)
{

    if(targetPageIndex==255)
    {

        if(this->pageInfo1.previousPage_Index_mainStackWidget>10)
        {
            this->pageInfo1.previousPage_Index_mainStackWidget=(this->plcVars.work_Mode==2)?1:4;
        }
        this->pageInfo1.targetPage_Index_mainStackWidget=this->pageInfo1.previousPage_Index_mainStackWidget;
    }
    else
    {
        this->pageInfo1.targetPage_Index_mainStackWidget=targetPageIndex;
    }
    if(loggingEnable&&loggingLevel>0)
        {
            QString logcontents=tr("Time:%1,page changed required,target page:%2")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"))
                    .arg(this->pageInfo1.targetPage_Index_mainStackWidget);
            emit this->logRequest(logcontents,35,0);
        }
    qDebug()<<"page changed required,target page:"<<this->pageInfo1.targetPage_Index_mainStackWidget;

    bool needLogOn=(this->pageInfo1.targetPage_Index_mainStackWidget==0)
            ||(this->pageInfo1.targetPage_Index_mainStackWidget==1)
            ||(this->pageInfo1.targetPage_Index_mainStackWidget==6);
    qDebug()<<tr("need log on:%1").arg(needLogOn?"true":"false");
    if((!this->logInStatus)&&needLogOn)
    {

       qDebug()<<"going to logOn page";
       this->ui->stackedWidget_mainProgram->setCurrentIndex(5);
       this->ui->label_password_wrong->setVisible(false);
       return;
    }
    else
    {
        if(loggingEnable&&loggingLevel>0)
            {
                QString logcontents=tr("Time:%1,going to target page")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"));
                emit this->logRequest(logcontents,35,0);
            }
        qDebug()<<"going to target page";
        this->ui->stackedWidget_mainProgram->setCurrentIndex(this->pageInfo1.targetPage_Index_mainStackWidget);
    }

}
void MainWindow::getBarcode_left()
{
    qDebug()<<"getBarcoede_Left executed";
    QEventLoop eventloop;
    QTimer::singleShot(20, &eventloop, SLOT(quit()));
    eventloop.exec();
    if(clsBarcode_left->error())
    {
        QMessageBox::information(this, tr("barcode scann error"), tr("error occured on serial port"));
        return;
    }
    this->barcode_to_use_left=QString(this->clsBarcode_left->readAll());
    if(this->barcode_to_use_left.size()<this->tempTooling_editting->leftBarcodeSettings.minLength)
    {
        if(this->barcode_to_use_left.size()>0)
        {
            QMessageBox::information(this, tr("barcode too short,portName:%1").arg(this->clsBarcode_left->portName()),
                                     tr("length of scanned barcode is less than specified minimum length:%1,length:%2,barcode:%3")
                                     .arg(this->tempTooling_editting->leftBarcodeSettings.minLength).arg(this->barcode_to_use_left.size()).arg(this->barcode_to_use_left));
            this->barcode_to_use_left.clear();
        }
        return;
    }
    qDebug()<<tr("left barcode, port:%1,new got barcode:%2")
           .arg(this->clsBarcode_left->portName())
           .arg(this->barcode_to_use_left);
    this->ui->lineEdit_scannedBarcode_left->setText(this->barcode_to_use_left);
    this->ui->lineEdit_barcode_left->setText(this->barcode_to_use_left);
}
void MainWindow::getBarcode_right()
{

    QEventLoop eventloop;
    QTimer::singleShot(20, &eventloop, SLOT(quit()));
    eventloop.exec();
    if(clsBarcode_right->error())
    {
        QMessageBox::information(this, tr("barcode scann error"), tr("error occured on serial port"));
        return;
    }
    this->barcode_to_use_right=QString(this->clsBarcode_right->readAll());
    if(this->barcode_to_use_right.size()<this->tempTooling_editting->leftBarcodeSettings.minLength)
    {
        if(this->barcode_to_use_right.size()>0)
        {
            QMessageBox::information(this, tr("barcode too short_right,portName:%1").arg(this->clsBarcode_right->portName()),
                                     tr("length of scanned barcode is less than specified minimum length:%1,barcode length:%2,barcode:%3")
                                     .arg(this->tempTooling_editting->leftBarcodeSettings.minLength).arg(this->barcode_to_use_right.size()).arg(this->barcode_to_use_right));
            this->barcode_to_use_right.clear();
        }

        return;
    }
    qDebug()<<tr("right barcode, port:%1,new got barcode:%2")
           .arg(this->clsBarcode_right->portName())
           .arg(this->barcode_to_use_right);
    this->ui->lineEdit_scannedBarcode_right->setText(this->barcode_to_use_right);
    this->ui->lineEdit_barcode_right->setText(this->barcode_to_use_right);
}
void MainWindow::lockUnlockPLCFromHMI(bool lockFlag)
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x81;//commandNO low byte,129
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=lockFlag?0x01:0x00;//00==unlock,01==lock
    dataToTcpCommObj[7]=0x00;//reserve
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}
void MainWindow::handleBarcodeError(QSerialPort::SerialPortError error)
 {
     qDebug()<<tr("barcode error, error:%1")
            .arg(error);

 }
void MainWindow::moveCycleDataToHistory()
{

    //contruct CSV headers
    qDebug()<<"movecycleData to history executed";
    QStringList cycleDataStrList_header;
    cycleDataStrList_header<<tr("dateTime")<<tr("barcode")<<tr("partWeldResult")<<tr("toolID")<<tr("toolName")<<tr("location");
    QString pointNOAndName;
    for(int i=1;i<=16;i++)
    {
        pointNOAndName=tr("-Point#%1-%2")
                .arg(i)
                .arg(this->tempTooling_editting->pointNameMapping.at(i));
        cycleDataStrList_header<<tr("pointWeldResult%1").arg(pointNOAndName)
                               <<tr("amplitude(\%)%1").arg(pointNOAndName)
                               <<tr("pressure(Kpa)%1").arg(pointNOAndName)
                               <<tr("weldTime(ms)%1").arg(pointNOAndName)
                               <<tr("peakPower%1").arg(pointNOAndName)
                               <<tr("weldEnergy%1").arg(pointNOAndName)
                               <<tr("holdTime%1").arg(pointNOAndName);
    }

    //It's important to init stringList before access it by "[]"
    QStringList emptyStrList;
    for(int i=0;i<cycleDataStrList_header.size();i++)
    {
        emptyStrList<<" ";
    }

    //handle cycle data of left part
    qDebug()<<tr("handling part cycle data, point qty of left part:%1,point qty of right part:%2")
              .arg(this->cycleData_leftPart.pointsDataMap.size()).arg(this->cycleData_rightPart.pointsDataMap.size());
    if(this->cycleData_leftPart.pointsDataMap.size()>0&&(!this->cycleData_leftPart.partBarcode.isEmpty()))
    {
        //construct cycle data with part data
        QStringList lastCycleDataStrList_left(emptyStrList);
        lastCycleDataStrList_left[0]=this->cycleData_leftPart.partDateTime;
        lastCycleDataStrList_left[1]=this->cycleData_leftPart.partBarcode;
        QString weldResult_str;
        if(this->cycleData_leftPart.partWeldResult==0)
        {
            weldResult_str="unWelded";

        }
        else if(this->cycleData_leftPart.partWeldResult==1)
        {
            weldResult_str="good";

        }
        else if(this->cycleData_leftPart.partWeldResult==2)
        {
             weldResult_str="suspect";

        }
        else if(this->cycleData_leftPart.partWeldResult==3)
        {
            weldResult_str="bad";

        }
        lastCycleDataStrList_left[2]=weldResult_str;
        lastCycleDataStrList_left[3]=QString::number(this->plcVars.toolID_sensor_detected);
        lastCycleDataStrList_left[4]=this->tempTooling_editting->toolingName;
        lastCycleDataStrList_left[5]="LEFT";
        //handle point data,prepare data for CSV,store point data to database
        QList<quint8> keys=this->cycleData_leftPart.pointsDataMap.keys();
        quint8 key;
        QString sqlquery;

        for(int j=0;j<keys.size();j++)
        {
            //prepare data fields to save to CSV
            key=keys.at(j);
            QString pointWeldResultStr;
            quint8 pointWeldResultInt=this->cycleData_leftPart.pointsDataMap.value(key).weldResult;
            if(pointWeldResultInt==0)
            {
                pointWeldResultStr="unWelded";

            }
            else if(pointWeldResultInt==1)
            {
                pointWeldResultStr="good";

            }
            else if(pointWeldResultInt==2)
            {
                pointWeldResultStr="suspect";

            }
            else if(pointWeldResultInt==3)
            {
                pointWeldResultStr="bad";

            }

            lastCycleDataStrList_left[(key-1)*7+6]=pointWeldResultStr;
            lastCycleDataStrList_left[(key-1)*7+7]=QString::number(this->cycleData_leftPart.pointsDataMap.value(key).amplitude);
            lastCycleDataStrList_left[(key-1)*7+8]=QString::number(this->cycleData_leftPart.pointsDataMap.value(key).thrusterDownPressure);
            lastCycleDataStrList_left[(key-1)*7+9]=QString::number(this->cycleData_leftPart.pointsDataMap.value(key).weldTime);
            lastCycleDataStrList_left[(key-1)*7+10]=QString::number(this->cycleData_leftPart.pointsDataMap.value(key).peakPower);
            lastCycleDataStrList_left[(key-1)*7+11]=QString::number(this->cycleData_leftPart.pointsDataMap.value(key).weldEnergy);
            lastCycleDataStrList_left[(key-1)*7+12]=QString::number(this->cycleData_leftPart.pointsDataMap.value(key).holdTime);
            //save point cycle data to table "pointHistoryCycleData" in database
            QString pointName=this->tempTooling_editting->pointNameMapping.at(this->cycleData_leftPart.pointsDataMap.value(key).pointNO);
            if(pointName.isEmpty())
            {
                pointName="Point"+QString::number(this->cycleData_leftPart.pointsDataMap.value(key).pointNO);
            }

            sqlquery=QObject::tr("insert or replace into %1(Barcode,pointNO,pointName,pointWeldResult,amplitude,pressure,weldTime,peakPower,weldEnergy,holdTime) values(%2,%3,%4,%5,%6,%7,%8,%9,%10,%11)")
                                .arg("pointHistoryCycleData").arg("\""+this->cycleData_leftPart.partBarcode+"\"")
                                .arg(this->cycleData_leftPart.pointsDataMap.value(key).pointNO)
                                .arg("\""+pointName+"\"")
                                .arg("\""+pointWeldResultStr+"\"")
                                .arg(this->cycleData_leftPart.pointsDataMap.value(key).amplitude)
                                .arg(this->cycleData_leftPart.pointsDataMap.value(key).thrusterDownPressure).arg(this->cycleData_leftPart.pointsDataMap.value(key).weldTime)
                                .arg(this->cycleData_leftPart.pointsDataMap.value(key).peakPower).arg(this->cycleData_leftPart.pointsDataMap.value(key).weldEnergy)
                                .arg(this->cycleData_leftPart.pointsDataMap.value(key).holdTime);
            qDebug()<<"before save point data to database,sql:"<<sqlquery;
            dbh_1.writeDatabase(sqlquery);
        }
        //save part cycle data to table "partHistoryCycleData" in database
        sqlquery=QObject::tr("insert or replace into %1(partBarcode,partWeldResult,toolID,toolName,location) values(%2,%3,%4,%5,\"LEFT\")")
                            .arg("partHistoryCycleData").arg("\""+this->cycleData_leftPart.partBarcode+"\"")
                            .arg("\""+weldResult_str+"\"").arg(this->plcVars.toolID_sensor_detected).arg("\""+this->tempTooling_editting->toolingName+"\"");
                            //.arg("\""+"LEFT"+"\"");

        qDebug()<<"before save part data to database,sql:"<<sqlquery;
        dbh_1.writeDatabase(sqlquery);
        //save last cycle data to CSV file
        if(true)
        {
           QDir dir1;
           if(!dir1.exists("cycleData/lastCycle"))
           {

              dir1.mkpath("cycleData/lastCycle");
           }

           QString fileName_CommonPart=tr("Left-%1-%2-%3").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"))
                   .arg(weldResult_str).arg(this->cycleData_leftPart.partBarcode);
           QString fileName_temp=tr("%1%2.temp").arg("cycleData/").arg(fileName_CommonPart);
           QString fileName_csv=tr("%1%2.CSV").arg("cycleData/lastCycle/").arg(fileName_CommonPart);
           QFile tempFile(fileName_temp);
           if(tempFile.open(QIODevice::WriteOnly))
           {
               QTextStream out1(&tempFile);
               for(int k1=0;k1<cycleDataStrList_header.size();k1++)
               {
                   out1<<cycleDataStrList_header.at(k1)<<',';
               }
               out1<<'\n';
               for(int k2=0;k2<lastCycleDataStrList_left.size();k2++)
               {
                   out1<<lastCycleDataStrList_left.at(k2)<<',';
               }
               out1<<'\n';
               tempFile.rename(fileName_csv);
               tempFile.close();
           }
           else
           {
               if(loggingEnable&&loggingLevel>0)
               {
                   QString logcontents=tr("failed to save left cycle data to CSV, can not open file,file name:%1").arg(fileName_temp);
                   emit this->logRequest(logcontents,200,0);
               }
           }

        }

    }
    // handle cycle data of right part
    if(this->cycleData_rightPart.pointsDataMap.size()>0&&(!this->cycleData_rightPart.partBarcode.isEmpty()))
    {
        //construct cycle data with part data
        QStringList lastCycleDataStrList_right(emptyStrList);
        lastCycleDataStrList_right[0]=this->cycleData_rightPart.partDateTime;
        lastCycleDataStrList_right[1]=this->cycleData_rightPart.partBarcode;
        QString weldResult_str;
        if(this->cycleData_rightPart.partWeldResult==0)
        {
            weldResult_str="unWelded";

        }
        else if(this->cycleData_rightPart.partWeldResult==1)
        {
            weldResult_str="good";

        }
        else if(this->cycleData_rightPart.partWeldResult==2)
        {
             weldResult_str="suspect";

        }
        else if(this->cycleData_rightPart.partWeldResult==3)
        {
            weldResult_str="bad";

        }
        lastCycleDataStrList_right[2]=weldResult_str;
        lastCycleDataStrList_right[3]=QString::number(this->plcVars.toolID_sensor_detected);
        lastCycleDataStrList_right[4]=this->tempTooling_editting->toolingName;
        lastCycleDataStrList_right[5]="RIGHT";
        //handle point data,prepare data for CSV,store point data to database
        QList<quint8> keys=this->cycleData_rightPart.pointsDataMap.keys();
        quint8 key;

        QString sqlquery;

        for(int j=0;j<keys.size();j++)
        {
            //prepare data fields to save to CSV
            key=keys.at(j);
            QString pointWeldResultStr;
            quint8 pointWeldResultInt=this->cycleData_rightPart.pointsDataMap.value(key).weldResult;
            if(pointWeldResultInt==0)
            {
                pointWeldResultStr="unWelded";

            }
            else if(pointWeldResultInt==1)
            {
                pointWeldResultStr="good";

            }
            else if(pointWeldResultInt==2)
            {
                pointWeldResultStr="suspect";

            }
            else if(pointWeldResultInt==3)
            {
                pointWeldResultStr="bad";

            }

            lastCycleDataStrList_right[(key-1)*7+6]=pointWeldResultStr;
            lastCycleDataStrList_right[(key-1)*7+7]=QString::number(this->cycleData_rightPart.pointsDataMap.value(key).amplitude);
            lastCycleDataStrList_right[(key-1)*7+8]=QString::number(this->cycleData_rightPart.pointsDataMap.value(key).thrusterDownPressure);
            lastCycleDataStrList_right[(key-1)*7+9]=QString::number(this->cycleData_rightPart.pointsDataMap.value(key).weldTime);
            lastCycleDataStrList_right[(key-1)*7+10]=QString::number(this->cycleData_rightPart.pointsDataMap.value(key).peakPower);
            lastCycleDataStrList_right[(key-1)*7+11]=QString::number(this->cycleData_rightPart.pointsDataMap.value(key).weldEnergy);
            lastCycleDataStrList_right[(key-1)*7+12]=QString::number(this->cycleData_rightPart.pointsDataMap.value(key).holdTime);
            //save point cycle data to table "pointHistoryCycleData" in database
            QString pointName=this->tempTooling_editting->pointNameMapping.at(this->cycleData_rightPart.pointsDataMap.value(key).pointNO);
            if(pointName.isEmpty())
            {
                pointName="Point"+QString::number(this->cycleData_rightPart.pointsDataMap.value(key).pointNO);
            }

            sqlquery=QObject::tr("insert or replace into %1(Barcode,pointNO,pointName,pointWeldResult,amplitude,pressure,weldTime,peakPower,weldEnergy,holdTime) "
                                         "values(%2,%3,%4,%5,%6,%7,%8,%9,%10,%11)")
                                .arg("pointHistoryCycleData").arg("\""+this->cycleData_rightPart.partBarcode+"\"")
                                .arg(this->cycleData_rightPart.pointsDataMap.value(key).pointNO)
                                .arg("\""+pointName+"\"")
                                .arg("\""+pointWeldResultStr+"\"")
                                .arg(this->cycleData_rightPart.pointsDataMap.value(key).amplitude)
                                .arg(this->cycleData_rightPart.pointsDataMap.value(key).thrusterDownPressure).arg(this->cycleData_rightPart.pointsDataMap.value(key).weldTime)
                                .arg(this->cycleData_rightPart.pointsDataMap.value(key).peakPower).arg(this->cycleData_rightPart.pointsDataMap.value(key).weldEnergy)
                                .arg(this->cycleData_rightPart.pointsDataMap.value(key).holdTime);

            dbh_1.writeDatabase(sqlquery);
        }
        //save part cycle data to table "partHistoryCycleData" in database
        sqlquery=QObject::tr("insert or replace into %1(partBarcode,partWeldResult,toolID,toolName,location) "
                                     "values(%2,%3,%4,%5,\"RIGHT\")")
                            .arg("partHistoryCycleData").arg("\""+this->cycleData_rightPart.partBarcode+"\"")
                            .arg("\""+weldResult_str+"\"").arg(this->plcVars.toolID_sensor_detected).arg("\""+this->tempTooling_editting->toolingName+"\"");
                            //.arg("\""+"RIGHT"+"\"");

        dbh_1.writeDatabase(sqlquery);
        //save last cycle data to CSV file
        if(true)
        {

           QString fileName_CommonPart=tr("Right-%1-%2-%3").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"))
                   .arg(weldResult_str).arg(this->cycleData_rightPart.partBarcode);
           QString fileName_temp=tr("%1%2.temp").arg("cycleData/").arg(fileName_CommonPart);
           QString fileName_csv=tr("%1%2.CSV").arg("cycleData/lastCycle/").arg(fileName_CommonPart);
           QFile tempFile(fileName_temp);
           if(tempFile.open(QIODevice::WriteOnly|QIODevice::Text))
           {
               QTextStream out1(&tempFile);
               for(int k1=0;k1<cycleDataStrList_header.size();k1++)
               {
                   out1<<cycleDataStrList_header.at(k1)<<',';
               }
               out1<<'\n';
               for(int k2=0;k2<lastCycleDataStrList_right.size();k2++)
               {
                   out1<<lastCycleDataStrList_right.at(k2)<<',';
               }
               out1<<'\n';
               tempFile.rename(fileName_csv);
               tempFile.close();
           }
           else
           {
               if(loggingEnable&&loggingLevel>0)
               {
                   QString logcontents=tr("failed to save right cycle data to CSV, can not open file,file name:%1").arg(fileName_temp);
                   emit this->logRequest(logcontents,200,0);
               }
           }

        }

    }

}
void MainWindow::OnPLCItemsChanged_Modbus(QVariantList changedItems)
{
    plcItem item;
    for(int i=0;i<changedItems.size();i++)
    {

        item=changedItems.at(i).value<plcItem>();
        //qDebug()<<tr("receivedItems from Modbus,area:%1, itemID:%2,index:%3")
                  //.arg(item.itemGroup_area).arg(item.itemID()).arg(i);
        QCoreApplication::postEvent(this,new myEvent_updateDisplay((QEvent::Type)5011,item));
    }

}
void MainWindow::customEvent(QEvent *e)
{
    switch (e->type())
    {
    case 5011://update plc items on window
        updatePlcItemDisplayEventHandler(e);
        break;

    default:
        break;
    }

}
void MainWindow::updatePlcItemDisplayEventHandler(QEvent *e)
{
    myEvent_updateDisplay *customEvent = static_cast<myEvent_updateDisplay *>(e);
    this->updatePLCItem(customEvent->myItem);

}
void MainWindow::updatePLCItem(plcItem item)
{

    enum plcAera {DI=0x81, DO=0x82, M=0x83, DB=0x84};
    QString tableName;
    quint32 itemID=item.itemID();

if(item.itemGroup_area==plcAera::DI)
{
    tableName="PLC_DI";
    this->modbusConnectionStatus_502=true;

    switch (itemID)
    {
        //IB0
        case 528384000://I0.0
        {
            this->switchItemOnOff(this->ui->LED_I00,item.currentValue.wordVar?true:false);
            this->switchItemOnOff(this->ui->LED_I00_0_LightCurtainOK,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384001://I0.1
        {
            this->switchItemOnOff(this->ui->LED_I01,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384002://I0.2
        {
            this->switchItemOnOff(this->ui->LED_I02,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384003://I0.3
        {
            this->switchItemOnOff(this->ui->LED_I03,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384004://I0.4
        {
            this->switchItemOnOff(this->ui->LED_I04,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384005://I0.5
        {
            this->switchItemOnOff(this->ui->LED_I05,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384006://I0.6
        {
            this->switchItemOnOff(this->ui->LED_I06,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384007://I0.7
        {
            this->switchItemOnOff(this->ui->LED_I07,item.currentValue.wordVar?true:false);
            break;
        }
        //IB1
        case 528384008://I1.0
        {
            this->switchItemOnOff(this->ui->LED_I10,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384009://I1.1
        {
            this->switchItemOnOff(this->ui->LED_I11,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384010://I1.2
        {
            this->switchItemOnOff(this->ui->LED_I12,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384011://I1.3
        {
            this->switchItemOnOff(this->ui->LED_I13,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384012://I1.4
        {
            this->switchItemOnOff(this->ui->LED_I14,item.currentValue.wordVar?true:false);
            this->switchItemOnOff(this->ui->LED_I1_4ESTOP_DoorsOK,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384013://I1.5
        {
            this->switchItemOnOff(this->ui->LED_I15,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384014://I1.6
        {
            this->switchItemOnOff(this->ui->LED_I16,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384015://I1.7
        {
            this->switchItemOnOff(this->ui->LED_I17,item.currentValue.wordVar?true:false);
            break;
        }
        //IB2
        case 528384016://I2.0
        {
            this->switchItemOnOff(this->ui->LED_I20,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384017://I2.1
        {
            this->switchItemOnOff(this->ui->LED_I21,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384018://I2.2
        {
            this->switchItemOnOff(this->ui->LED_I22,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384019://I2.3
        {
            this->switchItemOnOff(this->ui->LED_I23,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384020://I2.4
        {
            this->switchItemOnOff(this->ui->LED_I24,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384021://I2.5
        {
            this->switchItemOnOff(this->ui->LED_I25,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384022://I2.6
        {
            this->switchItemOnOff(this->ui->LED_I26,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384023://I2.7
        {
            this->switchItemOnOff(this->ui->LED_I27,item.currentValue.wordVar?true:false);
            break;
        }
        //IB3
        case 528384024://I3.0
        {
            this->switchItemOnOff(this->ui->LED_I30,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384025://I3.1
        {
            this->switchItemOnOff(this->ui->LED_I31,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384026://I3.2
        {
            this->switchItemOnOff(this->ui->LED_I32,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384027://I3.3
        {
            this->switchItemOnOff(this->ui->LED_I33,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384028://I3.4
        {
            this->switchItemOnOff(this->ui->LED_I34,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384029://I3.5
        {
            this->switchItemOnOff(this->ui->LED_I35,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384030://I3.6
        {
            this->switchItemOnOff(this->ui->LED_I36,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384031://I3.7
        {
            this->switchItemOnOff(this->ui->LED_I37,item.currentValue.wordVar?true:false);
            break;
        }
        //IB4
        case 528384032://I4.0
        {
            this->switchItemOnOff(this->ui->LED_I40,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384033://I4.1
        {
            this->switchItemOnOff(this->ui->LED_I41,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384034://I4.2
        {
            this->switchItemOnOff(this->ui->LED_I42,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384035://I4.3
        {
            this->switchItemOnOff(this->ui->LED_I43,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384036://I4.4
        {
            this->switchItemOnOff(this->ui->LED_I44,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384037://I4.5
        {
            this->switchItemOnOff(this->ui->LED_I45,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384038://I4.6
        {
            this->switchItemOnOff(this->ui->LED_I46,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384039://I4.7
        {
            this->switchItemOnOff(this->ui->LED_I47,item.currentValue.wordVar?true:false);
            break;
        }
        //IB5
        case 528384040://I5.0
        {
            this->switchItemOnOff(this->ui->LED_I50,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384041://I5.1
        {
            this->switchItemOnOff(this->ui->LED_I51,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384042://I5.2
        {
            this->switchItemOnOff(this->ui->LED_I52,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384043://I5.3
        {
            this->switchItemOnOff(this->ui->LED_I53,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384044://I5.4
        {
            this->switchItemOnOff(this->ui->LED_I54,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384045://I5.5
        {
            this->switchItemOnOff(this->ui->LED_I55,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384046://I5.6
        {
            this->switchItemOnOff(this->ui->LED_I56,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384047://I5.7
        {
            this->switchItemOnOff(this->ui->LED_I57,item.currentValue.wordVar?true:false);
            break;
        }
        //IB6
        case 528384048://I6.0
        {
            this->switchItemOnOff(this->ui->LED_I60,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384049://I6.1
        {
            this->switchItemOnOff(this->ui->LED_I61,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384050://I6.2
        {
            this->switchItemOnOff(this->ui->LED_I62,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384051://I6.3
        {
            this->switchItemOnOff(this->ui->LED_I63,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384052://I6.4
        {
            this->switchItemOnOff(this->ui->LED_I64,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384053://I6.5
        {
            this->switchItemOnOff(this->ui->LED_I65,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384054://I6.6
        {
            this->switchItemOnOff(this->ui->LED_I66,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384055://I6.7
        {
            this->switchItemOnOff(this->ui->LED_I67,item.currentValue.wordVar?true:false);
            break;
        }
        //IB7
        case 528384056://I7.0
        {
            this->switchItemOnOff(this->ui->LED_I70,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384057://I7.1
        {
            this->switchItemOnOff(this->ui->LED_I71,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384058://I7.2
        {
            this->switchItemOnOff(this->ui->LED_I72,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384059://I7.3
        {
            this->switchItemOnOff(this->ui->LED_I73,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384060://I7.4
        {
            this->switchItemOnOff(this->ui->LED_I74,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384061://I7.5
        {
            this->switchItemOnOff(this->ui->LED_I75,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384062://I7.6
        {
            this->switchItemOnOff(this->ui->LED_I76,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384063://I7.7
        {
            this->switchItemOnOff(this->ui->LED_I77,item.currentValue.wordVar?true:false);
            break;
        }
        //IB8
        case 528384064://I8.0
        {
            this->switchItemOnOff(this->ui->LED_I80,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384065://I8.1
        {
            this->switchItemOnOff(this->ui->LED_I81,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384066://I8.2
        {
            this->switchItemOnOff(this->ui->LED_I82,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384067://I8.3
        {
            this->switchItemOnOff(this->ui->LED_I83,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384068://I8.4
        {
            this->switchItemOnOff(this->ui->LED_I84,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384069://I8.5
        {
            this->switchItemOnOff(this->ui->LED_I85,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384070://I8.6
        {
            this->switchItemOnOff(this->ui->LED_I86,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384071://I8.7
        {
            this->switchItemOnOff(this->ui->LED_I87,item.currentValue.wordVar?true:false);
            break;
        }
        //IB9
        case 528384072://I9.0
        {
            this->switchItemOnOff(this->ui->LED_I90,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384073://I9.1
        {
            this->switchItemOnOff(this->ui->LED_I91,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384074://I9.2
        {
            this->switchItemOnOff(this->ui->LED_I92,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384075://I9.3
        {
            this->switchItemOnOff(this->ui->LED_I93,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384076://I9.4
        {
            this->switchItemOnOff(this->ui->LED_I94,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384077://I9.5
        {
            this->switchItemOnOff(this->ui->LED_I95,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384078://I9.6
        {
            this->switchItemOnOff(this->ui->LED_I96,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384079://I9.7
        {
            this->switchItemOnOff(this->ui->LED_I97,item.currentValue.wordVar?true:false);
            break;
        }
        //IB10
        case 528384080://I10.0
        {
            this->switchItemOnOff(this->ui->LED_I90,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384081://I10.1
        {
            this->switchItemOnOff(this->ui->LED_I91,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384082://I10.2
        {
            this->switchItemOnOff(this->ui->LED_I92,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384083://I10.3
        {
            this->switchItemOnOff(this->ui->LED_I93,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384084://I10.4
        {
            this->switchItemOnOff(this->ui->LED_I94,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384085://I10.5
        {
            this->switchItemOnOff(this->ui->LED_I95,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384086://I10.6
        {
            this->switchItemOnOff(this->ui->LED_I96,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384087://I10.7
        {
            this->switchItemOnOff(this->ui->LED_I97,item.currentValue.wordVar?true:false);
            break;
        }
        //IB11~IB13
        case 528384088://I11.0
        {
            this->switchItemOnOff(this->ui->LED_I110_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384089://I11.1
        {
            this->switchItemOnOff(this->ui->LED_I111_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384090://I11.2
        {
            this->switchItemOnOff(this->ui->LED_I112_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384091://I11.3
        {
            this->switchItemOnOff(this->ui->LED_I113_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384092://I11.4
        {
            this->switchItemOnOff(this->ui->LED_I114_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384093://I11.5
        {
            this->switchItemOnOff(this->ui->LED_I115_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384094://I11.6
        {
            this->switchItemOnOff(this->ui->LED_I116_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384095://I11.7
        {
            this->switchItemOnOff(this->ui->LED_I117_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384096://I12.0
        {
            this->switchItemOnOff(this->ui->LED_I120_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384097://I12.1
        {
            this->switchItemOnOff(this->ui->LED_I121_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384098://I12.2
        {
            this->switchItemOnOff(this->ui->LED_I122_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384099://I12.3
        {
            this->switchItemOnOff(this->ui->LED_I123_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384100://I12.4
        {
            this->switchItemOnOff(this->ui->LED_I124_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384101://I12.5
        {
            this->switchItemOnOff(this->ui->LED_I125_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384102://I12.6
        {
            this->switchItemOnOff(this->ui->LED_I126_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384103://I12.7
        {
            this->switchItemOnOff(this->ui->LED_I127_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384104://I13.0
        {
            //this->switchItemOnOff(this->ui->LED_I130,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384105://I13.1
        {
            //this->switchItemOnOff(this->ui->LED_I130,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384106://I13.2
        {
            //this->switchItemOnOff(this->ui->LED_I130,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384107://I13.3
        {
            //this->switchItemOnOff(this->ui->LED_I130,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384108://I13.4
        {
            //this->switchItemOnOff(this->ui->LED_I130,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384109://I13.5
        {
            this->switchItemOnOff(this->ui->LED_I135_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384110://I13.6
        {
            this->switchItemOnOff(this->ui->LED_I136_2,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384111://I13.7
        {
            this->switchItemOnOff(this->ui->LED_I137,item.currentValue.wordVar?true:false);
            break;
        }

        //IB14
        case 528384112://I14.0
        {
            this->switchItemOnOff(this->ui->LED_I140,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384113://I14.1
        {
            this->switchItemOnOff(this->ui->LED_I141,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384114://I14.2
        {
            this->switchItemOnOff(this->ui->LED_I142,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384115://I14.3
        {
            this->switchItemOnOff(this->ui->LED_I143,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384116://I14.4
        {
            this->switchItemOnOff(this->ui->LED_I144,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384117://I14.5
        {
            this->switchItemOnOff(this->ui->LED_I45,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384118://I14.6
        {
            this->switchItemOnOff(this->ui->LED_I146,item.currentValue.wordVar?true:false);
            break;
        }
        case 528384119://I14.7
        {
            this->switchItemOnOff(this->ui->LED_I147,item.currentValue.wordVar?true:false);
            break;
        }
        default:
                break;
    }
}
else if(item.itemGroup_area==plcAera::DO)
{
    tableName="PLC_DO";
    this->modbusConnectionStatus_502=true;
    switch (itemID)
    {
        //QB0~QB3
        case 532480000://Q0.0
        {
            this->switchItemOnOff(this->ui->LED_Q00,item.currentValue.wordVar?true:false);
            break;
        }

        case 532480001://Q0.1
        {
            this->switchItemOnOff(this->ui->LED_Q01,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480002://Q0.2
        {
            this->switchItemOnOff(this->ui->LED_Q02,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480003://Q0.3
        {
            this->switchItemOnOff(this->ui->LED_Q03,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480004://Q0.4
        {
            this->switchItemOnOff(this->ui->LED_Q04,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480005://Q0.5
        {
            this->switchItemOnOff(this->ui->LED_Q05,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480006://Q0.6
        {
            this->switchItemOnOff(this->ui->LED_Q06,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480007://Q0.7
        {
            this->switchItemOnOff(this->ui->LED_Q07,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480008://Q1.0
        {
            this->switchItemOnOff(this->ui->LED_Q10,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480009://Q1.1
        {
            this->switchItemOnOff(this->ui->LED_Q11,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480010://Q1.2
        {
            this->switchItemOnOff(this->ui->LED_Q12,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480011://Q1.3
        {
            this->switchItemOnOff(this->ui->LED_Q13,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480012://Q1.4
        {
            this->switchItemOnOff(this->ui->LED_Q14,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480013://Q1.5
        {
            this->switchItemOnOff(this->ui->LED_Q15,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480014://Q1.6
        {
            this->switchItemOnOff(this->ui->LED_Q16,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480015://Q1.7
        {
            this->switchItemOnOff(this->ui->LED_Q17,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480016://Q2.0
        {
            this->switchItemOnOff(this->ui->LED_Q20,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480017://Q2.1
        {
            this->switchItemOnOff(this->ui->LED_Q21,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480018://Q2.2
        {
            this->switchItemOnOff(this->ui->LED_Q22,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480019://Q2.3
        {
            this->switchItemOnOff(this->ui->LED_Q23,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480020://Q2.4
        {
            this->switchItemOnOff(this->ui->LED_Q24,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480021://Q2.5
        {
            this->switchItemOnOff(this->ui->LED_Q25,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480022://Q2.6
        {
            this->switchItemOnOff(this->ui->LED_Q26,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480023://Q2.7
        {
            this->switchItemOnOff(this->ui->LED_Q27,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480024://Q3.0
        {
            this->switchItemOnOff(this->ui->LED_Q30,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480025://Q3.1
        {
            this->switchItemOnOff(this->ui->LED_Q31,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480026://Q3.2
        {
            this->switchItemOnOff(this->ui->LED_Q32,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480027://Q3.3
        {
            this->switchItemOnOff(this->ui->LED_Q33,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480028://Q3.4
        {
            this->switchItemOnOff(this->ui->LED_Q34,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480029://Q3.5
        {
            this->switchItemOnOff(this->ui->LED_Q35,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480030://Q3.6
        {
            this->switchItemOnOff(this->ui->LED_Q36,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480031://Q3.7
        {
            this->switchItemOnOff(this->ui->LED_Q37,item.currentValue.wordVar?true:false);
            break;
        }
        //QB4
        case 532480032://Q4.0
        {
            this->switchItemOnOff(this->ui->LED_Q40,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480033://Q4.1
        {
            this->switchItemOnOff(this->ui->LED_Q41,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480034://Q4.2
        {
            this->switchItemOnOff(this->ui->LED_Q42,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480035://Q4.3
        {
            this->switchItemOnOff(this->ui->LED_Q43,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480036://Q4.4
        {
            this->switchItemOnOff(this->ui->LED_Q44,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480037://Q4.5
        {
            this->switchItemOnOff(this->ui->LED_Q45,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480038://Q4.6
        {
            this->switchItemOnOff(this->ui->LED_Q46,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480039://Q4.7
        {
            this->switchItemOnOff(this->ui->LED_Q47,item.currentValue.wordVar?true:false);
            break;
        }
        //QB5
        case 532480040://Q5.0
        {
            this->switchItemOnOff(this->ui->LED_Q50,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480041://Q5.1
        {
            this->switchItemOnOff(this->ui->LED_Q51,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480042://Q5.2
        {
            this->switchItemOnOff(this->ui->LED_Q52,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480043://Q5.3
        {
            this->switchItemOnOff(this->ui->LED_Q53,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480044://Q5.4
        {
            this->switchItemOnOff(this->ui->LED_Q54,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480045://Q5.5
        {
            this->switchItemOnOff(this->ui->LED_Q55,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480046://Q5.6
        {
            this->switchItemOnOff(this->ui->LED_Q56,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480047://Q5.7
        {
            this->switchItemOnOff(this->ui->LED_Q57,item.currentValue.wordVar?true:false);
            break;
        }
        //QB6
        case 532480048://Q6.0
        {
            this->switchItemOnOff(this->ui->LED_Q60,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480049://Q6.1
        {
            this->switchItemOnOff(this->ui->LED_Q61,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480050://Q6.2
        {
            this->switchItemOnOff(this->ui->LED_Q62,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480051://Q6.3
        {
            this->switchItemOnOff(this->ui->LED_Q63,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480052://Q6.4
        {
            this->switchItemOnOff(this->ui->LED_Q64,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480053://Q6.5
        {
            this->switchItemOnOff(this->ui->LED_Q65,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480054://Q6.6
        {
            this->switchItemOnOff(this->ui->LED_Q66,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480055://Q6.7
        {
            this->switchItemOnOff(this->ui->LED_Q67,item.currentValue.wordVar?true:false);
            break;
        }
        //QB7
        case 532480056://Q7.0
        {
            this->switchItemOnOff(this->ui->LED_Q70,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480057://Q7.1
        {
            this->switchItemOnOff(this->ui->LED_Q71,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480058://Q7.2
        {
            this->switchItemOnOff(this->ui->LED_Q72,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480059://Q7.3
        {
            this->switchItemOnOff(this->ui->LED_Q73,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480060://Q7.4
        {
            this->switchItemOnOff(this->ui->LED_Q74,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480061://Q7.5
        {
            this->switchItemOnOff(this->ui->LED_Q75,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480062://Q7.6
        {
            this->switchItemOnOff(this->ui->LED_Q76,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480063://Q7.7
        {
            this->switchItemOnOff(this->ui->LED_Q77,item.currentValue.wordVar?true:false);
            break;
        }
        //QB8
        case 532480064://Q8.0
        {
            this->switchItemOnOff(this->ui->LED_Q80,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480065://Q8.1
        {
            this->switchItemOnOff(this->ui->LED_Q81,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480066://Q8.2
        {
            this->switchItemOnOff(this->ui->LED_Q82,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480067://Q8.3
        {
            this->switchItemOnOff(this->ui->LED_Q83,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480068://Q8.4
        {
            this->switchItemOnOff(this->ui->LED_Q84,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480069://Q8.5
        {
            this->switchItemOnOff(this->ui->LED_Q85,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480070://Q8.6
        {
            this->switchItemOnOff(this->ui->LED_Q86,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480071://Q8.7
        {
            this->switchItemOnOff(this->ui->LED_Q87,item.currentValue.wordVar?true:false);
            break;
        }
        //QB9
        case 532480072://Q9.0
        {
            this->switchItemOnOff(this->ui->LED_Q90,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480073://Q9.1
        {
            this->switchItemOnOff(this->ui->LED_Q91,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480074://Q9.2
        {
            this->switchItemOnOff(this->ui->LED_Q92,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480075://Q9.3
        {
            this->switchItemOnOff(this->ui->LED_Q93,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480076://Q9.4
        {
            this->switchItemOnOff(this->ui->LED_Q94,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480077://Q9.5
        {
            this->switchItemOnOff(this->ui->LED_Q95,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480078://Q9.6
        {
            this->switchItemOnOff(this->ui->LED_Q96,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480079://Q9.7
        {
            this->switchItemOnOff(this->ui->LED_Q97,item.currentValue.wordVar?true:false);
            break;
        }
        //QB10
        case 532480080://Q10.0
        {
            this->switchItemOnOff(this->ui->LED_Q100,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480081://Q10.1
        {
            this->switchItemOnOff(this->ui->LED_Q101,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480082://Q10.2
        {
            this->switchItemOnOff(this->ui->LED_Q102,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480083://Q10.3
        {
            this->switchItemOnOff(this->ui->LED_Q103,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480084://Q10.4
        {
            this->switchItemOnOff(this->ui->LED_Q104,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480085://Q10.5
        {
            this->switchItemOnOff(this->ui->LED_Q105,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480086://Q10.6
        {
            this->switchItemOnOff(this->ui->LED_Q106,item.currentValue.wordVar?true:false);
            break;
        }
        case 532480087://Q10.7
        {
            this->switchItemOnOff(this->ui->LED_Q107,item.currentValue.wordVar?true:false);
            break;
        }

        default:
            break;
    }
}
else if(item.itemGroup_area==plcAera::M)
{
    tableName="PLC_M";
    this->modbusConnectionStatus_502=true;
    switch (itemID)
    {
        //MW10
        case 536576080://MW10
        {
            this->plcVars.currentPointNO_gen1=item.currentValue.wordVar/256;
            this->plcVars.currentPointNO_gen2=item.currentValue.wordVar%256;
            this->ui->spinBox_currentPoint_Gen2->setValue(this->plcVars.currentPointNO_gen2);
            this->ui->spinBox_currentPoint_Gen1->setValue(this->plcVars.currentPointNO_gen1);
            break;
        }
        //MW12
        case 536576096://MW12
        {
            this->plcVars.currentPointNO_gen3=item.currentValue.wordVar/256;
            this->plcVars.currentPointNO_gen4=item.currentValue.wordVar%256;
            this->ui->spinBox_currentPoint_Gen3->setValue(this->plcVars.currentPointNO_gen3);
            this->ui->spinBox_currentPoint_Gen4->setValue(this->plcVars.currentPointNO_gen4);
            break;
        }

        //MW16
        case 536576128://MW16,toolID
        {

            this->plcVars.toolID_sensor_detected=(quint8)(item.currentValue.wordVar%256);
            this->ui->spinBox_toolID_fromPLC_runscreen->setValue(this->plcVars.toolID_sensor_detected);
            this->ui->lineEdit_toolName_runScreen->setText(this->tempTooling_editting->toolingName);
            if(this->toolID_PLC!=this->plcVars.toolID_sensor_detected)
            {
                this->toolID_PLC=this->plcVars.toolID_sensor_detected;
                this->ui->toolID_fromPLC->setValue(this->toolID_PLC);
            }
            this->plcVars.toolID_active_inPLC=(quint8)(item.currentValue.wordVar/256);
            break;
        }
        //MW18,indicating MSG
        case 536576144://MW18
        {
            quint16 indicatingMsg;
            indicatingMsg=(quint16)item.currentValue.wordVar;
            //qDebug()<<"got indicating MSG:"<<indicatingMsg;

            //qDebug()<<"got indicating MSG:"<<this->systemRegisteredTextList.value(1000+indicatingMsg);
            this->ui->label_IndicatingMSG->setText(QString::number(indicatingMsg)+":"+this->systemRegisteredTextList.value(1000+indicatingMsg));
            //this->ui->label_IndicatingMSG_2->setText(this->systemRegisteredTextList.value(1000+indicatingMsg));

            if(this->ui->LED_indicatingMsg->isVisible())
            {
                this->ui->LED_indicatingMsg->setVisible(false);
            }
            else
            {
              this->ui->LED_indicatingMsg->setVisible(true);
            }
            break;
        }
        //MW20
        case 536576160://MW20,ultrasonic status
        {

            this->plcVars.systemReady=item.currentValue.bitsVar.b2;
            this->switchItemOnOff(this->ui->LED_RDY_NEW_CYCLE,item.currentValue.bitsVar.b6?true:false);
            this->switchItemOnOff(this->ui->LED_US_Gen1,item.currentValue.bitsVar.b8?true:false);
            this->switchItemOnOff(this->ui->LED_US_Gen2,item.currentValue.bitsVar.b9?true:false);
            this->switchItemOnOff(this->ui->LED_US_Gen3,item.currentValue.bitsVar.b10?true:false);
            this->switchItemOnOff(this->ui->LED_US_Gen4,item.currentValue.bitsVar.b11?true:false);
            //update IO table,M20.0,2,4,6
            this->switchItemOnOff(this->ui->LED_M200,item.currentValue.bitsVar.b0?true:false);
            this->switchItemOnOff(this->ui->LED_M202,item.currentValue.bitsVar.b2?true:false);
            this->switchItemOnOff(this->ui->LED_M204,item.currentValue.bitsVar.b4?true:false);
            this->switchItemOnOff(this->ui->LED_M206,item.currentValue.bitsVar.b6?true:false);

            break;
        }
        //MW22
        case 536576176://MW22,important flags
        {

            //update IO table
            this->switchItemOnOff(this->ui->LED_M220,item.currentValue.bitsVar.b0?true:false);
            this->switchItemOnOff(this->ui->LED_M221,item.currentValue.bitsVar.b1?true:false);
            this->switchItemOnOff(this->ui->LED_M222,item.currentValue.bitsVar.b2?true:false);
            this->switchItemOnOff(this->ui->LED_M223,item.currentValue.bitsVar.b3?true:false);
            this->switchItemOnOff(this->ui->LED_M224,item.currentValue.bitsVar.b4?true:false);
            this->switchItemOnOff(this->ui->LED_M226,item.currentValue.bitsVar.b6?true:false);
            this->switchItemOnOff(this->ui->LED_M227,item.currentValue.bitsVar.b7?true:false);
            this->switchItemOnOff(this->ui->LED_M231,item.currentValue.bitsVar.b9?true:false);

            this->switchItemOnOff(this->ui->LED_M232,item.currentValue.bitsVar.b10?true:false);
            this->switchItemOnOff(this->ui->LED_M233,item.currentValue.bitsVar.b11?true:false);
            this->switchItemOnOff(this->ui->LED_M234,item.currentValue.bitsVar.b12?true:false);
            this->switchItemOnOff(this->ui->LED_M235,item.currentValue.bitsVar.b13?true:false);
            this->switchItemOnOff(this->ui->LED_M23_5_allThrusterRetracted,item.currentValue.bitsVar.b13?true:false);
            this->switchItemOnOff(this->ui->LED_M236,item.currentValue.bitsVar.b14?true:false);
            this->switchItemOnOff(this->ui->LED_M23_6_allPartHolderSafe,item.currentValue.bitsVar.b14?true:false);
            this->switchItemOnOff(this->ui->LED_M237,item.currentValue.bitsVar.b15?true:false);

            break;
        }
        //MW24
        case 536576192://MW24,current stepNO
        {
            //if not autoMode,break
            if(this->plcVars.work_Mode!=3)
                break;
            quint8 receivedStepNO=(quint8)(item.currentValue.wordVar/256);
            //step 0, update barcode display if new barcode scanned,lock/unlock welder accordingly
            if(receivedStepNO==0)
            {
                //step 0, if barcodes are empty and barcode scan not enable,auto generate barcodes
                if((!this->tempTooling_editting->leftBarcodeSettings.enable)&&this->barcode_to_use_left.isEmpty()&&this->barcode_in_use_left.isEmpty())
                {
                    this->barcode_to_use_left=this->clsBarcode_left->autoGenerateBarcode("AUTOLEFT");
                    this->ui->lineEdit_barcode_left->setText(this->barcode_to_use_left);
                }
                if((!this->tempTooling_editting->rightBarcodeSettings.enable)&&this->barcode_to_use_right.isEmpty()&&this->barcode_in_use_right.isEmpty())
                {
                    this->barcode_to_use_right=this->clsBarcode_left->autoGenerateBarcode("AUTORIGHT");
                    this->ui->lineEdit_barcode_right->setText(this->barcode_to_use_right);
                }
                //step 0, update barcode as long as new barcode scanned/generated
                if(!this->barcode_to_use_left.isEmpty())
                {
                    this->barcode_in_use_left=this->barcode_to_use_left;
                    this->barcode_to_use_left.clear();
                    this->ui->lineEdit_barcode_left->clear();
                    this->ui->lineEdit_barcode_left_inuse->setText(this->barcode_in_use_left);
                }
                if(!this->barcode_to_use_right.isEmpty())
                {
                    this->barcode_in_use_right=this->barcode_to_use_right;
                    this->barcode_to_use_right.clear();
                    this->ui->lineEdit_barcode_right->clear();
                    this->ui->lineEdit_barcode_right_inuse->setText(this->barcode_in_use_right);
                }
                //step 0, barcode_In_use is empty,lock PLC if not locked

                if((!this->plcVars.PLC_lockedByHMI)&&(this->barcode_in_use_left.isEmpty()||this->barcode_in_use_left.isEmpty()))
                {
                    this->lockUnlockPLCFromHMI(true);
                }
                //step 0, barcode_in_use is not empty,unlock PLC if locked
                if((this->plcVars.PLC_lockedByHMI)&&(!this->barcode_in_use_left.isEmpty())&&(!this->barcode_in_use_left.isEmpty()))
                {
                    this->lockUnlockPLCFromHMI(false);
                }

            }
            //step>0, barcode_to_use is empty,lock PLC if not locked
            if(receivedStepNO>0)
            {
                if((!this->plcVars.PLC_lockedByHMI)&&(this->barcode_to_use_left.isEmpty()||this->barcode_to_use_right.isEmpty()))
                {
                    this->lockUnlockPLCFromHMI(true);
                }
            }
            //stepNO changed
            if(receivedStepNO!=this->plcVars.currentStepNO)
            {
                //stepNO from 0 to non-zero, do the following things
                if(receivedStepNO>0&&this->plcVars.currentStepNO==0)
                {

                    if(!this->plcVars.somePointWelded)
                    {
                        this->ui->tableWidget_lastCycleData->clearContents();
                    }
                    //reset point cycle data status
                    this->pointCycleDataStatus_p1_p32.DWordVar=0;
                }
                //step NO from non-zero value to zero, do the following things
                if(receivedStepNO==0)
                {
                    //moveCycleDataToHistory
                    if(this->cycleData_leftPart.partWeldResult>0||this->cycleData_rightPart.partWeldResult>0)
                    {
                      this->moveCycleDataToHistory();
                    }

                    //barcodes clear
                    this->barcode_in_use_left.clear();
                    this->barcode_in_use_right.clear();
                    this->ui->lineEdit_barcode_left_inuse->clear();
                    this->ui->lineEdit_barcode_right_inuse->clear();
                    this->cycleData_leftPart.partWeldResult=0;
                    this->cycleData_rightPart.partWeldResult=0;
                    //if barcode not enabled, generate barcode automatically
                    if(!this->tempTooling_editting->leftBarcodeSettings.enable)
                    {
                        this->barcode_to_use_left=this->clsBarcode_left->autoGenerateBarcode("AUTOLEFT");

                    }
                    if(!this->tempTooling_editting->rightBarcodeSettings.enable)
                    {
                        this->barcode_to_use_right=this->clsBarcode_right->autoGenerateBarcode("AUTORIGHT");
                    }
                    //update the point bypass status
                    QByteArray dataToTcpCommObj;
                    dataToTcpCommObj[0]=0x00;//length high byte
                    dataToTcpCommObj[1]=0x0C;//length low byte
                    dataToTcpCommObj[2]=0x00;//commandNO high byte
                    dataToTcpCommObj[3]=0x7F;//commandNO low byte,127
                    dataToTcpCommObj[4]=0x00;//reserve byte
                    dataToTcpCommObj[5]=0x00;//reserve byte
                    dataToTcpCommObj[6]=0x02;//01==set,02==get
                    dataToTcpCommObj[7]=0x00;//retain flag
                    dataToTcpCommObj[8]=0x00;//point 1~8
                    dataToTcpCommObj[9]=0x00;//point 9~16
                    dataToTcpCommObj[10]=0x00;//reserve
                    dataToTcpCommObj[11]=0x00;//reserve
                    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
                        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
                }
                //sync stepNO
                this->plcVars.currentStepNO=receivedStepNO;
            }

            this->ui->spinBox_run_liveStepNO->setValue(this->plcVars.currentStepNO);


            break;
        }

        //MW26, tRCV_MSG_count
        case 536576208://MW26,count of PLC received Msg
        {
            quint16 Msg_count;
            Msg_count=(quint16)item.currentValue.wordVar;
            //qDebug()<<"TRCV_MSG_Count:"<<Msg_count;

            if(this->plcVars.work_Mode==2)
            {
                QString msg_str=tr("%1").arg((quint16)Msg_count);
                this->ui->lineEdit_MSG_Count->setText(msg_str);
            }

            break;
        }
        case 536576240://MW30,work mode,locked flag
        {

            quint8 currentWorkMode;
            if(item.currentValue.bitsVar.b2)
                currentWorkMode=2;
            else if(item.currentValue.bitsVar.b3)
                currentWorkMode=3;
            if(this->plcVars.work_Mode!=currentWorkMode||this->ui->stackedWidget_mainProgram->currentIndex()==9)
            {
                qDebug()<<tr("received PLC workMode:%1,PC stored workMode:%2,current Page Index:%3")
                          .arg(currentWorkMode).arg((this->plcVars.work_Mode)).arg(this->ui->stackedWidget_mainProgram->currentIndex());
                this->changePage(currentWorkMode==2?1:4);
                this->plcVars.work_Mode=currentWorkMode;
                this->logInStatus=false;
                this->parameterEditable=false;
                emit parameterEditableStausChanged(false);
            }
            this->switchItemOnOff(this->ui->LED_ManualMode,item.currentValue.bitsVar.b2?true:false);
            //work mode-pause,btn_start pressed,U/S in process
            this->switchItemOnOff(this->ui->LED_M304,item.currentValue.bitsVar.b4?true:false);
            this->switchItemOnOff(this->ui->LED_M305,item.currentValue.bitsVar.b5?true:false);
            this->switchItemOnOff(this->ui->LED_M307,item.currentValue.bitsVar.b7?true:false);
            //somePoint welded flag
            this->plcVars.somePointWelded=item.currentValue.bitsVar.b9;
            this->switchItemOnOff(this->ui->LED_M311,item.currentValue.bitsVar.b9?true:false);
            //PLC_locked_byHMI flag
            this->plcVars.PLC_lockedByHMI=item.currentValue.bitsVar.b11;
            this->switchItemOnOff(this->ui->LED_M313,item.currentValue.bitsVar.b11?true:false);
            break;
        }
        case 536576256://MW32,part counter-total
        {

            this->ui->spinBox_partCounter_total->setValue(item.currentValue.wordVar);
            break;
        }
        case 536576272://MW34,part counter-good
        {

            this->ui->spinBox_partCounter_good->setValue(item.currentValue.wordVar);
            break;
        }
        case 536576288://MW36,part counter-bad
        {

            this->ui->spinBox_partCounter_bad->setValue(item.currentValue.wordVar);
            break;
        }
        case 536576304://MW38,part counter-suspect
        {

            this->ui->spinBox_partCounter_suspect->setValue(item.currentValue.wordVar);
            break;
        }
        case 536576336://MW42,pointDataReadyForHMI,point17~point32
        {
            //after received the point data ready bits in dwords, compare with that in local
            //request the corresponding point cycle data if they are different
            //everytime new point data comes,pointCycleDataStatus_p1_p32 will be updated
            dWordBytes requestPointData_dword;
            requestPointData_dword.wordsVar.W1=pointCycleDataStatus_p1_p32.wordsVar.W0^BigLittleSwap16(item.currentValue.wordVar);
            requestPointData_dword.wordsVar.W0=0;
            this->requestPointCycleDataFromPLC(requestPointData_dword);
            this->ui->spinBox_DataSync->setValue(requestPointData_dword.DWordVar);

    //        qDebug()<<tr("local stored point data ready status,W0:%1,W1:%2")
    //                .arg(this->pointCycleDataStatus_p1_p32.wordsVar.W0)
    //                .arg(this->pointCycleDataStatus_p1_p32.wordsVar.W1);
    //        qDebug()<<tr("PLC side  point data ready status,W1:%1").arg(item.currentValue.wordVar);
            break;
        }
        case 536576352://MW44,pointDataReadyForHMI,point1~point16
        {
            //after received the point data ready bits in dwords, compare with that in local
            //request the corresponding point cycle data if they are different
            //everytime new point data comes,pointCycleDataStatus_p1_p32 will be updated
            dWordBytes requestPointData_dword;
            requestPointData_dword.wordsVar.W0=pointCycleDataStatus_p1_p32.wordsVar.W0^BigLittleSwap16(item.currentValue.wordVar);
            requestPointData_dword.wordsVar.W1=0;
            this->requestPointCycleDataFromPLC(requestPointData_dword);
    //        qDebug()<<tr("local stored point data ready status,W0:%1,W1:%2")
    //                .arg(this->pointCycleDataStatus_p1_p32.wordsVar.W0)
    //                .arg(this->pointCycleDataStatus_p1_p32.wordsVar.W1);
    //        qDebug()<<tr("PLC side  point data ready status,W0:%1").arg(item.currentValue.wordVar);
            break;
        }
        case 536576368://MW46,pointDataRequsetFromHMI,point1~point16
        {

            ;
            break;
        }
        case 536576384://MW48,pointDataRequsetFromHMI,point17~point32
        {

            ;
            break;
        }
        case 536576400://MW50,servo status
        {

            this->switchItemOnOff(this->ui->LED_M50_0_servoReady,item.getBitFromWord(8));
            this->switchItemOnOff(this->ui->LED_M50_1_servoPositioningDone,item.getBitFromWord(9));
            this->switchItemOnOff(this->ui->LED_M50_2_servoHomed,item.getBitFromWord(10));
            this->switchItemOnOff(this->ui->LED_M50_3_InLoadingAera,item.getBitFromWord(11));
            this->switchItemOnOff(this->ui->LED_M50_4_servoMDIMode,item.getBitFromWord(12));
            this->switchItemOnOff(this->ui->LED_M50_7_servoAnyFault,item.getBitFromWord(15),true);
            break;
        }
        case 536576416://MW52,gen_1&&gen_2 status
        {

            ;
            this->switchItemOnOff(this->ui->LED_M52_0_InCycleGen1,item.getBitFromWord(8));
            this->switchItemOnOff(this->ui->LED_M52_1_LinkGoodGen_1,item.getBitFromWord(9));
            this->switchItemOnOff(this->ui->LED_M52_2_OnlineGen_1,item.getBitFromWord(10));
            this->switchItemOnOff(this->ui->LED_M52_4_RDYGen_1,item.getBitFromWord(12));
            this->switchItemOnOff(this->ui->LED_M52_5_USActive_Gen_1,item.getBitFromWord(13));
            this->switchItemOnOff(this->ui->LED_M52_7_AnyFault_Gen_1,item.getBitFromWord(15),true);

            this->switchItemOnOff(this->ui->LED_M53_0_InCycleGen_2,item.getBitFromWord(0));
            this->switchItemOnOff(this->ui->LED_M53_1_LinkGoodGen_2,item.getBitFromWord(1));
            this->switchItemOnOff(this->ui->LED_M53_2_OnlineGen_2,item.getBitFromWord(2));
            this->switchItemOnOff(this->ui->LED_M53_4_RDYGen_2,item.getBitFromWord(4));
            this->switchItemOnOff(this->ui->LED_M53_5_USActive_Gen_2,item.getBitFromWord(5));
            this->switchItemOnOff(this->ui->LED_M53_7_AnyFault_Gen_2,item.getBitFromWord(7),true);
            break;
        }
        case 536576432://MW54,gen_3&&gen_4 status
        {

            ;
            this->switchItemOnOff(this->ui->LED_M54_0_InCycleGen_3,item.getBitFromWord(8));
            this->switchItemOnOff(this->ui->LED_M54_1_LinkGoodGen_3,item.getBitFromWord(9));
            this->switchItemOnOff(this->ui->LED_M54_2_OnlineGen_3,item.getBitFromWord(10));
            this->switchItemOnOff(this->ui->LED_M54_4_RDYGen_3,item.getBitFromWord(12));
            this->switchItemOnOff(this->ui->LED_M54_5_USActive_Gen_3,item.getBitFromWord(13));
            this->switchItemOnOff(this->ui->LED_M54_7_AnyFault_Gen_3,item.getBitFromWord(15),true);

            this->switchItemOnOff(this->ui->LED_M55_0_InCycleGen_4,item.getBitFromWord(0));
            this->switchItemOnOff(this->ui->LED_M55_1_LinkGoodGen_4,item.getBitFromWord(1));
            this->switchItemOnOff(this->ui->LED_M55_2_OnlineGen_4,item.getBitFromWord(2));
            this->switchItemOnOff(this->ui->LED_M55_4_RDYGen_4,item.getBitFromWord(4));
            this->switchItemOnOff(this->ui->LED_M55_5_USActive_Gen_4,item.getBitFromWord(5));
            this->switchItemOnOff(this->ui->LED_M55_7_AnyFault_Gen_4,item.getBitFromWord(7),true);
            break;
            break;
        }
        case 536576448://MW56
        {
            break;
        }
        case 536576464://MW58
        {
            break;
        }
        case 536576480://MW60，iTV1
        {
            //(data1.thrusterDownPressure-228)*500/(3868-228)
            this->ui->spinBox_QW94_iTV01->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576496://MW62，iTV2
        {
            //this->ui->spinBox_QW92_iTV02->setValue(BigLittleSwap16(item.currentValue.wordVar));
            this->ui->spinBox_QW92_iTV02->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576512://MW64，iTV3
        {
            this->ui->spinBox_QW90_iTV03->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576528://MW66，iTV4
        {
            this->ui->spinBox_QW88_iTV04->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576544://MW68，iTV5
        {
            this->ui->spinBox_QW86_iTV05->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576560://MW70，iTV6
        {
            this->ui->spinBox_QW84_iTV06->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576576://MW72，iTV7
        {
            this->ui->spinBox_QW82_iTV07->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576592://MW74，iTV8
        {
            this->ui->spinBox_QW80_iTV08->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576608://MW76，iTV9
        {
            this->ui->spinBox_QW78_iTV09->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576624://MW78，iTV10
        {
            this->ui->spinBox_QW76_iTV10->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576640://MW80，iTV11
        {
            this->ui->spinBox_QW74_iTV11->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576656://MW82，iTV12
        {
            this->ui->spinBox_QW72_iTV12->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576672://MW84，iTV13
        {
            this->ui->spinBox_QW70_iTV13->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576688://MW86，iTV14
        {
            this->ui->spinBox_QW68_iTV14->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576704://MW88，iTV15
        {
            this->ui->spinBox_QW66_iTV15->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        case 536576720://MW90，iTV16
        {
            this->ui->spinBox_QW64_iTV16->setValue((BigLittleSwap16(item.currentValue.wordVar)-228)*500/(3868-228));
            break;
        }
        default:
            break;
    }
}
else if(item.itemGroup_area==plcAera::DB)
{
    tableName="PLC_DB";
    this->modbusConnectionStatus_503=true;
    //DW0,alarm word,
    switch (itemID) {
    case 540672000://dw0,gen_1 alarm
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672016://dw2,gen_2 alarm
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672032://dw3,gen3 alarm
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672048://dw4,gen4 alarm
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672064:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672080:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672096:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672112:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672128:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672144:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672160:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672176:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672192:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672208:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672224:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672240:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672256:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672272:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 54067288:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672304:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672320:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672336:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672352:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672368:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672384:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672400:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672416:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672432:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672448:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    case 540672464:
    {
        if(currentAlarms.isEmpty()&&item.currentValue.wordVar==0)
            break;
        else
        {
            this->handleAlarm(item);
            break;
        }
    }
    default:
        break;
    }

}

}
void MainWindow::switchItemOnOff(QLabel* targetLabel, bool onOff, bool alarmFlag)
{

    if(onOff)
    {
        if(!alarmFlag)
        {
           targetLabel->setPixmap(QPixmap(":/img/ledon1010.png"));
        }
        else
        {
            targetLabel->setPixmap(QPixmap(":/img/breakpoint.png"));
        }

    }
    else
    {
        targetLabel->setPixmap(QPixmap(":/img/ledoff1010.png"));
    }

}
void MainWindow::setPixmapForLabel(QLabel* label,QString imageSource)
{
    qDebug()<<"set pixmap for label executed,image source"<<imageSource;
    QImage Image;
    Image.load(imageSource);
    QPixmap pixmap = QPixmap::fromImage(Image);

    int width =this->ui->toolingPicLabel->width();
    int height =this->ui->toolingPicLabel->height();
    //QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 饱满填充
    QPixmap fitpixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
    label->setPixmap(fitpixmap);
}
void MainWindow::handleAlarm(plcItem item)
{
    //qDebug()<<tr("handling alarm,itemID:%1").arg(item.itemID());
    quint32 itemID=item.itemID();
    quint32 alarmID_plcItem;
    for(int i=0;i<16;i++)
    {
        alarmID_plcItem=itemID+i;
        bool alarmExist=this->currentAlarms.contains(alarmID_plcItem);
        bool bitValue=item.getBitFromWord(i);

        if(bitValue)
        {
            if(!alarmExist)//if bitValue==ture while the corresponding alarm not in alarm list, then add it
            {
                currentAlarms[alarmID_plcItem].AlarmID=alarmID_plcItem;
                currentAlarms[alarmID_plcItem].comeTime=QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss");
                QString alarmText_temp=this->systemRegisteredTextList[alarmID_plcItem];
                currentAlarms[alarmID_plcItem].alarmText=alarmText_temp;
            }
        }
        else
        {
            if(alarmExist)//if bitValue is false,while the corresponding alarm is still in alarm list, then move it to history
            {
                //currentAlarms[alarmID_plcItem].moveToHistory("./HistoryAlarm/Log_alarm.txt",this->ui->textEdit_2);
                currentAlarms[alarmID_plcItem].leaveTime=QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss");
                emit this->moveAlarmToHistory(currentAlarms[alarmID_plcItem]);
                currentAlarms.remove(alarmID_plcItem);
            }
        }
    }

    QString alarmText_current="";
    //gen error, display the current point of this generator as well
    if(itemID>=540672000&&itemID<=540672048)//gen1~gen4 alarm
    {
        //if alarm exist&&alarm not in the alarm list, then delay to make sure
        // current pointNO can be updated before display to GUI
        alarmID_plcItem=itemID+15;
        if(item.currentValue.wordVar!=0)
        {
            if(!currentAlarms.contains(alarmID_plcItem))
            {
                QEventLoop eventloop;
                //The current pointNO of generator need time to update,so here we put a time delay
                QTimer::singleShot(1200, &eventloop, SLOT(quit()));
                eventloop.exec();
            }
        }
        //get pointNO and genNO
        quint8 genNO,currentPointNO;
        switch (itemID) {
        case 540672000:
            genNO=1;
            currentPointNO=this->plcVars.currentPointNO_gen1;
            break;
        case 540672016:
            genNO=2;
            currentPointNO=this->plcVars.currentPointNO_gen2;
            break;
        case 540672032:
            genNO=3;
            currentPointNO=this->plcVars.currentPointNO_gen3;
            break;
        case 540672048:
            genNO=4;
            currentPointNO=this->plcVars.currentPointNO_gen4;
            break;
        default:
            break;
        }
        //if alarm exist&&alarm not in the alarm list, then add it to alarm list
        // if alarm gone while alarm still in the alarm list,then remove it from the alarm list
        if(item.currentValue.wordVar!=0)
        {
           if(!currentAlarms.contains(alarmID_plcItem))
           {
               currentAlarms[alarmID_plcItem].AlarmID=alarmID_plcItem;
               currentAlarms[alarmID_plcItem].comeTime=QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss");

               QString alarmText_temp=tr("generator related error,gen_%1,current pointNO:%2").arg(genNO).arg(currentPointNO);
               currentAlarms[alarmID_plcItem].alarmText=alarmText_temp;
           }
        }
        else
        {
            if(currentAlarms.contains(alarmID_plcItem))
            {
                currentAlarms[alarmID_plcItem].leaveTime=QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss");
                emit this->moveAlarmToHistory(currentAlarms[alarmID_plcItem]);
                currentAlarms.remove(alarmID_plcItem);
            }
        }
    }

    if(!this->currentAlarms.isEmpty())
    {
        QList<quint32> keys=this->currentAlarms.keys();
        for(int j=0;j<keys.size();j++)
        {
            QString text1;
            text1=tr("AlarmID:%1,comeTime:%2,alarmDesc.:%3 \n").arg(this->currentAlarms[keys.at(j)].AlarmID)
                    .arg(this->currentAlarms[keys.at(j)].comeTime)
                    .arg(this->currentAlarms[keys.at(j)].alarmText);
            alarmText_current.append(text1);
        }
    }
    emit this->updateAlarmText(alarmText_current);
}
void MainWindow::onUpdateAlarmText(QString text)
{
   this->ui->textEdit_currentAlarms->setText(text);
}
void MainWindow::onMoveAlarmToHistory(alarmItem alarm)
{
    QString alarmTextToFile;
    alarmTextToFile=tr("AlarmID:%1,comeTime:%2,leaveTime:%3,alarmDesc.:%4,\n")
            .arg(alarm.AlarmID)
            .arg(alarm.comeTime)
            .arg(alarm.leaveTime)
            .arg(alarm.alarmText);
    this->ui->textEdit_historyAlarm->append(alarmTextToFile);
    QFile file1("historyAlarm.txt");
    //limit the size of the log file:log_dispaly.txt
    if(file1.size()>1048576)
    {
        if(QFile::exists("historyAlarm_old.txt"))
           { QFile::remove("historyAlarm_old.txt");}
        file1.rename("historyAlarm_old.txt");
        //generate an empty "historyAlarm.txt"
        QFile file2("historyAlarm.txt");
        file2.open(QIODevice::WriteOnly|QIODevice::Append);
        file2.close();
    }

    if(file1.open(QIODevice::Append|QIODevice::Text))
    {
        QTextStream out1(&file1);

        out1<<alarmTextToFile;
    }
    file1.close();
}
void MainWindow::requestPointCycleDataFromPLC(dWordBytes pointsBits_dword)
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0E;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x82;//commandNO low byte,130
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    //bit_X==1,mean the corresponding point's data is desired
    dataToTcpCommObj[6]=pointsBits_dword.bytesVar.B3;//point1~8
    dataToTcpCommObj[7]=pointsBits_dword.bytesVar.B2;//point9~16
    dataToTcpCommObj[8]=pointsBits_dword.bytesVar.B1;//point17~24
    dataToTcpCommObj[9]=pointsBits_dword.bytesVar.B0;//point25~32
    dataToTcpCommObj[10]=0x00;//reserve byte
    dataToTcpCommObj[11]=0x00;//reserve byte
    dataToTcpCommObj[12]=0x00;//reserve byte
    dataToTcpCommObj[13]=0x00;//reserve byte

    if(this->tcpConnectionStatus_send)
    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}
void MainWindow::onReceivedPointCycleData(pointCycleData data1)
{
    qDebug()<<"on received point cycle data executed";
    if(data1.pointNO==0)
        return;
    //update point cycle data status
    this->pointCycleDataStatus_p1_p32.DWordVar=this->pointCycleDataStatus_p1_p32.DWordVar|(1u<<(data1.pointNO-1));

    if(this->plcVars.work_Mode==2)//manual mode
    {

    }
    else if (this->plcVars.work_Mode==3) //auto mode
    {

        QString pointName=this->tempTooling_editting->pointNameMapping[data1.pointNO];
        QString barcodeSide=this->tempTooling_editting->pointBarcodeMapping.value(data1.pointNO);
        QString dateTime1=QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        QString weldResult_str;
        QTableWidgetItem *weldResultItem= new QTableWidgetItem;
        if(data1.weldResult==0)
        {
            weldResult_str="unWelded";
            weldResultItem->setBackgroundColor(QColor("gray"));
            weldResultItem->setText("unWelded");
        }
        else if(data1.weldResult==1)
        {
            weldResult_str="good";
            weldResultItem->setBackgroundColor(QColor("green"));
            weldResultItem->setText("good");
        }
        else if(data1.weldResult==2)
        {
             weldResult_str="suspect";
             weldResultItem->setBackgroundColor(QColor("yellow"));
             weldResultItem->setText("suspect");
        }
        else if(data1.weldResult==3)
        {
            weldResult_str="bad";
            weldResultItem->setBackgroundColor(QColor("red"));
            weldResultItem->setText("bad");
        }
        //store point data to part data
        if(barcodeSide.contains(tr("Left"),Qt::CaseInsensitive)||barcodeSide.contains("左"))
        {
            qDebug()<<"store point data to part date executed,left,pointNO:"<<data1.pointNO;
            this->cycleData_leftPart.partDateTime=dateTime1;
            if(this->cycleData_leftPart.partBarcode.isEmpty())
                this->cycleData_leftPart.partBarcode=this->barcode_in_use_left;
            if(data1.weldResult>this->cycleData_leftPart.partWeldResult)
                this->cycleData_leftPart.partWeldResult=data1.weldResult;
            this->cycleData_leftPart.pointsDataMap[data1.pointNO]=data1;
        }
        else if(barcodeSide.contains(tr("Right"),Qt::CaseInsensitive))
        {
            qDebug()<<"store point data to part date executed,right,pointNO:"<<data1.pointNO;
            this->cycleData_rightPart.partDateTime=dateTime1;
            if(this->cycleData_rightPart.partBarcode.isEmpty())
                this->cycleData_rightPart.partBarcode=this->barcode_in_use_right;
            if(data1.weldResult>this->cycleData_rightPart.partWeldResult)
                this->cycleData_rightPart.partWeldResult=data1.weldResult;
            this->cycleData_rightPart.pointsDataMap[data1.pointNO]=data1;
        }
        else
        {
            qDebug()<<tr("barcode side:%1,contains(\"left\"):%2")
                      .arg(barcodeSide).arg(barcodeSide.contains(tr("Left"),Qt::CaseInsensitive)||barcodeSide.contains("左")?"true":"false");
        }
        //update display(tableWidget) on RunScreen
        this->ui->tableWidget_lastCycleData->setItem(data1.pointNO-1,0,new QTableWidgetItem(pointName));
        this->ui->tableWidget_lastCycleData->setItem(data1.pointNO-1,1,weldResultItem);
        this->ui->tableWidget_lastCycleData->setItem(data1.pointNO-1,2,new QTableWidgetItem(QString::number(data1.amplitude)));
        this->ui->tableWidget_lastCycleData->setItem(data1.pointNO-1,3,new QTableWidgetItem(QString::number(data1.thrusterDownPressure)));
        this->ui->tableWidget_lastCycleData->setItem(data1.pointNO-1,4,new QTableWidgetItem(QString::number(data1.weldTime)));
        this->ui->tableWidget_lastCycleData->setItem(data1.pointNO-1,5,new QTableWidgetItem(QString::number(data1.peakPower)));
        this->ui->tableWidget_lastCycleData->setItem(data1.pointNO-1,6,new QTableWidgetItem(QString::number(data1.weldEnergy)));
        this->ui->tableWidget_lastCycleData->setItem(data1.pointNO-1,7,new QTableWidgetItem(QString::number(data1.holdTime)));
        this->ui->tableWidget_lastCycleData->setItem(data1.pointNO-1,8,new QTableWidgetItem(dateTime1));
        this->ui->tableWidget_lastCycleData->setItem(data1.pointNO-1,9,
                  new QTableWidgetItem(barcodeSide.contains(tr("Left"),Qt::CaseInsensitive)||barcodeSide.contains("左")?this->barcode_in_use_left:this->barcode_in_use_right));
        weldResultItem=nullptr;
    }

}
void MainWindow::OnTimer_mainWindow_Timeout()
{
    //qDebug()<<tr("OnTimer_mainWindow_Timeout() executed,machineInfoReady:%1").arg(this->machineInfoReady);
    if(!this->machineInfoReady)
    {
        this->changePage(8);
    }

    //qDebug()<<tr("checking tcp comm connection status,current status:%1").
              //arg(this->checking_tcpConnectionStatus?"OFFLINE":"ONLINE");
    //if checking is true, mean there's no response from partner within specfied time, so need set connection status
    //to false;if partner reply within specified time, then the checking flag will be reset by the reply

    if(this->checking_tcpConnectionStatus)
    {
        this->tcpConnectionStatus_receive=false;
        this->tcpConnectionStatus_send=false;
        this->setPLCValueVisible(false);

    }
    else
    {

      this->checking_tcpConnectionStatus=true;

    }
    emit this->checkTcpConnectionStatus();
    //if checking is true, mean there's no response from partner within specfied time, so need set connection status
    //to false;if partner reply within specified time, then the checking flag will be reset by the reply
    if(this->checking_modbusConnectionStatus)
    {
        this->modbusConnectionStatus_502=false;
        this->modbusConnectionStatus_503=false;
        this->modbusConnectionStatus_504=false;
    }

    else
    {
      this->checking_modbusConnectionStatus=true;

    }
    emit this->checkModbusConnectionStatus();
    //toolID conflict popUp MsgBox control
    if(this->plcVars.toolID_active_inPLC!=this->plcVars.toolID_sensor_detected&&this->plcVars.toolID_active_inPLC>0)
    if(!this->isShowing_toolIDConflictPopUpDialog&&!this->ignoreToolIDConflict)
    {
        this->isShowing_toolIDConflictPopUpDialog=true;
        QCheckBox *cb=new QCheckBox(tr("No longer prompt"));
        cb->setChecked(false);
        QMessageBox msgBox;
        QString fileName=tr("toolingConfig/toolingConfig_%1.dc").arg(this->plcVars.toolID_sensor_detected);
        QString infoText=tr("sensored ID:%1,Active ID:%2,do you want to load the parameter set stored in %3")
                .arg(this->plcVars.toolID_sensor_detected).arg(this->plcVars.toolID_active_inPLC).arg(fileName);
         msgBox.setText("toolID conflict.");
         msgBox.setInformativeText(infoText);
         msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
         msgBox.setDefaultButton(QMessageBox::Yes);
         msgBox.setCheckBox(cb);
         int ret = msgBox.exec();
         switch (ret) {
             case QMessageBox::Yes:
             {
                //Yes was clicked
               if(this->tempTooling_editting->loadFromDisk(fileName))
               {
                  this->startDownloadUploadWholeSettings(1);
               }
               else
               {
                   QMessageBox::information(this,tr("failed to load setup"),tr("can not load desired setup,it may not exist"),QMessageBox::Ok);
               }
               break;
             }
             case QMessageBox::No:
             {
                // No was clicked
                if(cb->isChecked())
                    this->ignoreToolIDConflict=true;
                break;
             }
              default:
                 // should never be reached
                 break;
           }
         this->isShowing_toolIDConflictPopUpDialog=false;
    }

}
void MainWindow::OnTcpCommConnectionStateChanged(QAbstractSocket::SocketState state,quint8 ConnectionID)
{

    //qDebug()<<tr("received new state, state:%1,ConnectionID:%2").arg(state).arg(ConnectionID);
    switch (state)
    {
        case QAbstractSocket::ConnectedState:

            if(ConnectionID==1)
                {
                    this->tcpConnectionStatus_send=true;
                }

            else if (ConnectionID==2)
            {
                this->tcpConnectionStatus_receive=true;
            }
            break;
        default:

            if(ConnectionID==1)
                    this->tcpConnectionStatus_send=false;
                else if (ConnectionID==2)
                {
                    this->tcpConnectionStatus_receive=false;
                }
            break;

    }
    if(!(this->tcpConnectionStatus_receive&&this->tcpConnectionStatus_send))
    {
        this->checking_tcpConnectionStatus=false;
        //this->ui->OnOffLine_Label->setText("OFFLINE");
        this->setPLCValueVisible(false);
    }

    else
    {
        //set label text to online
        //this->ui->OnOffLine_Label->setText("ONLINE");
        if(this->toolID_editing==this->toolID_PLC&&this->toolID_editing>0)
        this->setPLCValueVisible(true);
    }

}
void MainWindow::receiveDataFromTCPCommObj(QByteArray dataFromTcpCommObj)
{
    int dataFromTcpCommObj_size=dataFromTcpCommObj.size();
    if(dataFromTcpCommObj_size<6)
    {
       qDebug()<<"subData size <6";
       return;
    }
    if(dataFromTcpCommObj_size!=(quint8)dataFromTcpCommObj.at(0)*256+dataFromTcpCommObj.at(1))
    {
       qDebug()<<tr("subData size not equal with length in byte array,subDatasize:%1,size of data from tcpcomobj:%2").arg(dataFromTcpCommObj_size);

       quint16 command=(quint8)dataFromTcpCommObj.at(2)*256+dataFromTcpCommObj.at(3);
       qDebug()<<"question command NO:"<<command;
        return;
    }

    quint16 command=(quint8)dataFromTcpCommObj.at(2)*256+dataFromTcpCommObj.at(3);
    //qDebug()<<"command NO:"<<command;
    QByteArray dataLoad;
    if(dataFromTcpCommObj_size>6)
    {
      dataLoad=dataFromTcpCommObj.mid(6,dataFromTcpCommObj_size-1);
    }
    else
    {
      dataLoad.clear();
    }

    switch (command)
    {

    case 1://received gen enable/disable status,update the display
    {

        bytebits bb1;
        bb1.byteChar=dataLoad[0];
        qDebug()<<tr("CommandNO:%1,got gen enable/disable info,gen Enable status:%2").arg(command).arg(QString::number((int)dataLoad[0],2));
        for(int i=1;i<=4;i++)
        {
           this->tooling_current->plcToolingInfo.generator_enable[i]=bb1.bits.getBit(i);
        }
        //this->tooling_current->plcToolingInfo.generator_enable[1]=bb1.bits.b1;
        //this->tooling_current->plcToolingInfo.generator_enable[2]=bb1.bits.b2;
        //this->tooling_current->plcToolingInfo.generator_enable[3]=bb1.bits.b3;
        //this->tooling_current->plcToolingInfo.generator_enable[4]=bb1.bits.b4;

        this->ui->CV_GenEnable_1->setChecked(bb1.bits.b1);
        this->ui->CV_GenEnable_2->setChecked(bb1.bits.b2);
        this->ui->CV_GenEnable_3->setChecked(bb1.bits.b3);
        this->ui->CV_GenEnable_4->setChecked(bb1.bits.b4);
        if(this->uploadingWholeSettingFromPLCInProcess)
        {
            for(int i=1;i<=4;i++)
            {
               this->tempTooling_editting->plcToolingInfo.generator_enable[i]=bb1.bits.getBit(i);
            }
            //this->tempTooling_editting->plcToolingInfo.generator_enable[1]=bb1.bits.b1;
            //this->tempTooling_editting->plcToolingInfo.generator_enable[2]=bb1.bits.b2;
            //this->tempTooling_editting->plcToolingInfo.generator_enable[3]=bb1.bits.b3;
            //this->tempTooling_editting->plcToolingInfo.generator_enable[4]=bb1.bits.b4;
            this->ui->checkBox_GenEnable_1->setChecked(bb1.bits.b1);
            this->ui->checkBox_GenEnable_2->setChecked(bb1.bits.b2);
            this->ui->checkBox_GenEnable_3->setChecked(bb1.bits.b3);
            this->ui->checkBox_GenEnable_4->setChecked(bb1.bits.b4);
        }

    }

    case 2://received gen realtime data, update display area accorddingly
    {
        quint8 receivedPointNO=dataLoad[1];
        quint8 receivedGenNO=dataLoad[0];
        dWordBytes dw1;
        dw1.bytesVar.B0=dataLoad[2];
        dw1.bytesVar.B1=dataLoad[3];
        dw1.bytesVar.B2=dataLoad[4];
        dw1.bytesVar.B3=dataLoad[5];
        quint32 realTimeFreqency=dw1.DWordVar;
        quint8 realTimeAmp=dataLoad[8];
        quint16 realTimePower=((quint8)dataLoad[6])*256+((quint8)dataLoad[7]);
        quint8 channelNO=(quint8)dataLoad[9]+1;
        switch (receivedGenNO)
        {
        case 1:
            this->plcVars.currentPointNO_gen1=receivedPointNO;
            this->ui->spbox_realTimePage_channel->setValue(channelNO);
            this->ui->spbox_realTimePage_currentPointNO->setValue(receivedPointNO);
            this->ui->spbox_realTimePage_liveAmplitude->setValue(realTimeAmp);
            this->ui->spbox_realTimePage_liveFrequency->setValue(realTimeFreqency);
            this->ui->spbox_realTimePage_livePower->setValue(realTimePower);
            break;
        case 2:
            this->plcVars.currentPointNO_gen2=receivedPointNO;
            this->ui->spbox_realTimePage_channel_2->setValue(channelNO);
            this->ui->spbox_realTimePage_currentPointNO_2->setValue(receivedPointNO);
            this->ui->spbox_realTimePage_liveAmplitude_2->setValue(realTimeAmp);
            this->ui->spbox_realTimePage_liveFrequency_2->setValue(realTimeFreqency);
            this->ui->spbox_realTimePage_livePower_2->setValue(realTimePower);
            break;
        case 3:
            this->plcVars.currentPointNO_gen3=receivedPointNO;
            this->ui->spbox_realTimePage_channel_3->setValue(channelNO);
            this->ui->spbox_realTimePage_currentPointNO_3->setValue(receivedPointNO);
            this->ui->spbox_realTimePage_liveAmplitude_3->setValue(realTimeAmp);
            this->ui->spbox_realTimePage_liveFrequency_3->setValue(realTimeFreqency);
            this->ui->spbox_realTimePage_livePower_3->setValue(realTimePower);
            break;
        case 4:
            this->plcVars.currentPointNO_gen4=receivedPointNO;
            this->ui->spbox_realTimePage_channel_4->setValue(channelNO);
            this->ui->spbox_realTimePage_currentPointNO_4->setValue(receivedPointNO);
            this->ui->spbox_realTimePage_liveAmplitude_4->setValue(realTimeAmp);
            this->ui->spbox_realTimePage_liveFrequency_4->setValue(realTimeFreqency);
            this->ui->spbox_realTimePage_livePower_4->setValue(realTimePower);
            break;
        default:
            break;
        }
        if(receivedPointNO==this->ui->spinBox_PointNO_pointPara->value())
        {
            this->ui->UStest_genNO->setValue(receivedGenNO);
            this->ui->CV_pointPara_realTime_Freq->setValue(realTimeFreqency);
            this->ui->CV_pointPara_realTime_Amp->setValue(realTimeAmp);
            this->ui->CV_pointPara_realTime_Power->setValue(realTimePower);
            this->ui->usTest_Channel->setValue(channelNO);
        }

    }
    break;
    case 3://received servo realtime info , update display area
    {
        dWordBytes dw1;
        //qDebug()<<"received servo realtime info";
        dw1.bytesVar.B0=dataLoad[2];
        dw1.bytesVar.B1=dataLoad[3];
        dw1.bytesVar.B2=dataLoad[4];
        dw1.bytesVar.B3=dataLoad[5];
        this->ui->weldByManual_tartgetPos->setText(QString::number((qint32)dw1.DWordVar));
        dw1.bytesVar.B0=dataLoad[6];
        dw1.bytesVar.B1=dataLoad[7];
        dw1.bytesVar.B2=dataLoad[8];
        dw1.bytesVar.B3=dataLoad[9];
        QString realtimeSpeed=QString::number((qint16)dw1.DWordVar);
        this->ui->weldByManual_realtimeSpeed->setText(realtimeSpeed);
        this->ui->servo_realTimeSpeed->setText(realtimeSpeed);
        this->ui->servo_realTimeSpeed_runScreen->setText(realtimeSpeed);
        this->ui->realTimePage_realtimeSpeed->setText(realtimeSpeed);
        dw1.bytesVar.B0=dataLoad[10];
        dw1.bytesVar.B1=dataLoad[11];
        dw1.bytesVar.B2=dataLoad[12];
        dw1.bytesVar.B3=dataLoad[13];
        QString realtimePOS=QString::number((qint32)dw1.DWordVar);
        this->ui->weldByManual_realtimePos->setText(realtimePOS);
        this->ui->servo_realTimePos->setText(realtimePOS);
        this->ui->servo_realTimePos_runScreen->setText(realtimePOS);
        this->ui->realTimePage_realtimePos->setText(realtimePOS);
        this->ui->weldByManual_currentStationNO->setValue(dataLoad[0]);
        this->ui->weldByManual_targetStationNO->setValue(dataLoad[1]);

    }
        break;
    case 5://received sensor bypass info,update coresponding display area
    {
        quint8 valveNO=(quint8) dataLoad[2];
        quint8 sensorgroup=(quint8) dataLoad[1];
        if(sensorgroup==1)//received partSensor info
        {
            this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.byteChar=dataLoad[3];
            this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.byteChar=dataLoad[4];
            this->ui->CV_PartSensor_1->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b1);
            this->ui->CV_PartSensor_2->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b2);
            this->ui->CV_PartSensor_3->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b3);
            this->ui->CV_PartSensor_4->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b4);
            this->ui->CV_PartSensor_5->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b5);
            this->ui->CV_PartSensor_6->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b6);
            this->ui->CV_PartSensor_7->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b1);
            this->ui->CV_PartSensor_8->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b2);
            this->ui->CV_PartSensor_9->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b3);
            this->ui->CV_PartSensor_10->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b4);
            this->ui->CV_PartSensor_11->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b5);
            this->ui->CV_PartSensor_12->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b6);
            if(uploadingWholeSettingFromPLCInProcess)
            {
                this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.byteChar=dataLoad[3];
                this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.byteChar=dataLoad[4];
                this->ui->checkBox_PartSensor_1->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b1);
                this->ui->checkBox_PartSensor_2->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b2);
                this->ui->checkBox_PartSensor_3->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b3);
                this->ui->checkBox_PartSensor_4->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b4);
                this->ui->checkBox_PartSensor_5->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b5);
                this->ui->checkBox_PartSensor_6->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b6);
                this->ui->checkBox_PartSensor_7->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b1);
                this->ui->checkBox_PartSensor_8->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b2);
                this->ui->checkBox_PartSensor_9->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b3);
                this->ui->checkBox_PartSensor_10->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b4);
                this->ui->checkBox_PartSensor_11->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b5);
                this->ui->checkBox_PartSensor_12->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b6);
            }
        }
        else if(sensorgroup==2)//received valve sensor info
        {
            qDebug()<<"received valve sensor bypass info,valveNO:"<<valveNO;
            this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.byteChar=dataLoad[3];
            qDebug()<<"extend sensors status:"<<quint8(dataLoad[3]);
            this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.byteChar=dataLoad[4];
            qDebug()<<"extend sensors status:"<<quint8(dataLoad[4]);
            if(valveNO==this->ui->sensorBypass_valveNO->value())
            {
                this->ui->CV_ValveExtendSensorBypass_1->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b1);
                this->ui->CV_ValveExtendSensorBypass_2->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b2);
                this->ui->CV_ValveExtendSensorBypass_3->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b3);
                this->ui->CV_ValveExtendSensorBypass_4->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b4);
                this->ui->CV_ValveExtendSensorBypass_5->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b5);
                this->ui->CV_ValveExtendSensorBypass_6->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b6);
                this->ui->CV_ValveRetractSensorBypass_1->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b1);
                this->ui->CV_ValveRetractSensorBypass_2->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b2);
                this->ui->CV_ValveRetractSensorBypass_3->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b3);
                this->ui->CV_ValveRetractSensorBypass_4->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b4);
                this->ui->CV_ValveRetractSensorBypass_5->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b5);
                this->ui->CV_ValveRetractSensorBypass_6->setChecked
                        (this->tooling_current->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b6);
            }

            if(uploadingWholeSettingFromPLCInProcess)
            {
                this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.byteChar=dataLoad[3];
                this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.byteChar=dataLoad[4];
                if(valveNO==this->ui->sensorBypass_valveNO->value())
                {
                    this->ui->checkBox_ValveExtendSensorBypass_1->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b1);
                    this->ui->checkBox_ValveExtendSensorBypass_2->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b2);
                    this->ui->checkBox_ValveExtendSensorBypass_3->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b3);
                    this->ui->checkBox_ValveExtendSensorBypass_4->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b4);
                    this->ui->checkBox_ValveExtendSensorBypass_5->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b5);
                    this->ui->checkBox_ValveExtendSensorBypass_6->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].extendBypass.bits.b6);
                    this->ui->checkBox_ValveRetractSensorBypass_1->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b1);
                    this->ui->checkBox_ValveRetractSensorBypass_2->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b2);
                    this->ui->checkBox_ValveRetractSensorBypass_3->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b3);
                    this->ui->checkBox_ValveRetractSensorBypass_4->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b4);
                    this->ui->checkBox_ValveRetractSensorBypass_5->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b5);
                    this->ui->checkBox_ValveRetractSensorBypass_6->setChecked
                            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[valveNO].retractBypass.bits.b6);
                }

            }
        }
        break;
    }

    case 6://get thruster config info from PLC, then update the display value
    {
        qDebug()<<tr("CommandNO:%1,get thruster config info, thruster NO:%2").arg(command).arg((quint8)dataLoad[0]);
        quint8 thrusterNOFromPLC;
        thrusterNOFromPLC=dataLoad[0];
        //update display if thruster NO is desired
        if(thrusterNOFromPLC==this->ui->spinBox_thrusterNO->value())
        {
            this->ui->CV_thrusterEnable->setValue(dataLoad[3]);
            this->ui->CV_GenNO->setValue((quint8)dataLoad[1]);
            this->ui->CV_Channel->setValue((quint8)dataLoad[2]+1);
            if(this->uploadingWholeSettingFromPLCInProcess)
            {
                this->ui->spinBox_thrusterEnable->setValue(dataLoad[3]);
                this->ui->spinBox_GenNO->setValue((quint8)dataLoad[1]);
                this->ui->spinBox_channel->setValue((quint8)dataLoad[2]+1);
            }
        }
        //store any received thruster info into tooling info on background
        this->tooling_current->plcToolingInfo.thruster_List[thrusterNOFromPLC].enable=dataLoad[3];
        this->tooling_current->plcToolingInfo.thruster_List[thrusterNOFromPLC].GenNO=dataLoad[1];
        this->tooling_current->plcToolingInfo.thruster_List[thrusterNOFromPLC].ChannelNO=(quint8)dataLoad[2]+1;
        if(this->uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.thruster_List[thrusterNOFromPLC].enable=dataLoad[3];
            this->tempTooling_editting->plcToolingInfo.thruster_List[thrusterNOFromPLC].GenNO=dataLoad[1];
            this->tempTooling_editting->plcToolingInfo.thruster_List[thrusterNOFromPLC].ChannelNO=(quint8)dataLoad[2]+1;
        }
        break;
    }
//    case 7://get weld point config info from PLC, then update the display value
//    {
//        quint8 weldPointNOFromPLC;
//        weldPointNOFromPLC=dataLoad[0];
//        //update display if point NO is desired
//        if(weldPointNOFromPLC==this->ui->spinBox_PointNO_pointPara->value())
//        {
//            this->ui->CV_weldPointEnable->setValue(dataLoad[3]);
//            this->ui->CV_thrusterNO_WPC->setValue(dataLoad[1]);
//            this->ui->CV_stepNO_WPC->setValue(dataLoad[2]);
//            if(this->uploadingWholeSettingFromPLCInProcess)
//            {
//                this->ui->spinBox_weldPointEnable->setValue(dataLoad[3]);
//                this->ui->spinBox_thrusterNOWPC->setValue(dataLoad[1]);
//                this->ui->spinBox_stepNO_WPC->setValue(dataLoad[2]);
//            }
//        }
//        //store any received point info into tooling info on background
//        this->tooling_current->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].enable=dataLoad[3];
//        this->tooling_current->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].ThrusterNO=dataLoad[1];
//        this->tooling_current->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].stepNO=dataLoad[2];
//        if(this->uploadingWholeSettingFromPLCInProcess)
//        {
//            this->tempTooling_editting->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].enable=dataLoad[3];
//            this->tempTooling_editting->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].ThrusterNO=dataLoad[1];
//            this->tempTooling_editting->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].stepNO=dataLoad[2];
//        }
//        break;
//    }
    case 8://get step/group station info from PLC, then update the display value
    {
       quint8 stepNOFromPLC;
       stepNOFromPLC=dataLoad[0];
       qDebug()<<"stepNOFromPLC"<<stepNOFromPLC<<"stationNO"<<(quint8)dataLoad[1];
       //update display if step NO is desired
       if(stepNOFromPLC==this->ui->spinBox_stepNO->value())
       {
           this->ui->CV_stationNO->setValue(dataLoad[1]);
           if(this->uploadingWholeSettingFromPLCInProcess)
           {
             this->ui->spinBox_stationNO->setValue(dataLoad[1]);
           }
       }
       //store any received point info into tooling info on background
       this->tooling_current->plcToolingInfo.stepStationConnection[stepNOFromPLC]=dataLoad[1];
       if(this->uploadingWholeSettingFromPLCInProcess)
       {
         this->tempTooling_editting->plcToolingInfo.stepStationConnection[stepNOFromPLC]=dataLoad[1];
       }
        break;
    }
    case 9://get weld point config&&u/s para info from PLC, then update the display value
    {
       quint8 pointNOFromPLC;
       pointNOFromPLC=dataLoad[0];
       qDebug()<<tr("CommandNO:%1,got weld point config&&US para info, point NO:%2").arg(command).arg((quint8)dataLoad[0]);
           this->wp1=(weldPoint*)dataLoad.data();
           weldPoint wp2=*wp1;
           //swap high and low bytes to adapt Siemens PLC's store method
           wp2.ultrasonicPara.Ramp_up_Time20to1250=BigLittleSwap16(wp2.ultrasonicPara.Ramp_up_Time20to1250);//B4-B5
           wp2.ultrasonicPara.Ramp_Down_TIme0to250=BigLittleSwap16(wp2.ultrasonicPara.Ramp_Down_TIme0to250);//B6-B7
           wp2.ultrasonicPara.Weld_Time0to30000=BigLittleSwap16(wp2.ultrasonicPara.Weld_Time0to30000);//B8-B9
           wp2.ultrasonicPara.Peak_Power=BigLittleSwap16(wp2.ultrasonicPara.Peak_Power);//B10-B11
           wp2.ultrasonicPara.Energy0to65535=BigLittleSwap16(wp2.ultrasonicPara.Energy0to65535);//B12-B13
           wp2.ultrasonicPara.Hold_TIme0to30000=BigLittleSwap16(wp2.ultrasonicPara.Hold_TIme0to30000);//B14-B15
           wp2.ultrasonicPara.AfterBurstdelay0to999=BigLittleSwap16(wp2.ultrasonicPara.AfterBurstdelay0to999);//B16-B17
           wp2.ultrasonicPara.Afterburst_Duration0t999=BigLittleSwap16(wp2.ultrasonicPara.Afterburst_Duration0t999);//B18-B19
           wp2.ultrasonicPara.Suspect_MaxTime_Limits=BigLittleSwap16(wp2.ultrasonicPara.Suspect_MaxTime_Limits);//B22-B23
           wp2.ultrasonicPara.suspect_MinTime_Limits=BigLittleSwap16(wp2.ultrasonicPara.suspect_MinTime_Limits);//B24-B25
           wp2.ultrasonicPara.Suspect_MaxPower_Limits=BigLittleSwap16(wp2.ultrasonicPara.Suspect_MaxPower_Limits);//B26-B27
           wp2.ultrasonicPara.Suspect_MinPower_Limits=BigLittleSwap16(wp2.ultrasonicPara.Suspect_MinPower_Limits);//B28-B29
           wp2.ultrasonicPara.Suspect_MaxEnergy_Limits=BigLittleSwap16(wp2.ultrasonicPara.Suspect_MaxEnergy_Limits);//B30-B31
           wp2.ultrasonicPara.Suspect_MinEnergy_Limits=BigLittleSwap16(wp2.ultrasonicPara.Suspect_MinEnergy_Limits);//B32-B33
           wp2.ultrasonicPara.BadPart_MaxTime_limits=BigLittleSwap16(wp2.ultrasonicPara.BadPart_MaxTime_limits);//B34-B35
           wp2.ultrasonicPara.Badpart_MinTime_Limits=BigLittleSwap16(wp2.ultrasonicPara.Badpart_MinTime_Limits);//B36-B37
           wp2.ultrasonicPara.BadPart_MaxPower_Limits=BigLittleSwap16(wp2.ultrasonicPara.BadPart_MaxPower_Limits);//B38-B39
           wp2.ultrasonicPara.BadPart_MinPower_Limits=BigLittleSwap16(wp2.ultrasonicPara.BadPart_MinPower_Limits);//B40-B41
           wp2.ultrasonicPara.BadPart_MaxEnergy_Limits=BigLittleSwap16(wp2.ultrasonicPara.BadPart_MaxEnergy_Limits);//B42-B43
           wp2.ultrasonicPara.BadPart_MinEnergy_Limits=BigLittleSwap16(wp2.ultrasonicPara.BadPart_MinEnergy_Limits);//B44-B45
           wp2.ultrasonicPara.Free_Run_Frequency=BigLittleSwap32(wp2.ultrasonicPara.Free_Run_Frequency);//B50~B53
           wp2.ultrasonicPara.Freq_Upper_Limit=BigLittleSwap32(wp2.ultrasonicPara.Freq_Upper_Limit);//B54~B57
           wp2.ultrasonicPara.Freq_Lower_Limit=BigLittleSwap32(wp2.ultrasonicPara.Freq_Lower_Limit);//B58~B61
           wp2.ultrasonicPara.Trigger_Power=BigLittleSwap16(wp2.ultrasonicPara.Trigger_Power);//B66-B67
           wp2.ultrasonicPara.Trigger_Timeout=BigLittleSwap16(wp2.ultrasonicPara.Trigger_Timeout);//B68-B69
           //wp2.thrusterPressure_down=BigLittleSwap16(wp2.thrusterPressure_down);
           wp2.thrusterPressure_down=(wp2.thrusterPressure_down-228)*500/(3868-228);

           //update display if point NO is desired
           if(pointNOFromPLC==this->ui->spinBox_PointNO_pointPara->value())
           {
              this->updateWeldPoitDisplay_Current(wp2);
               if(uploadingWholeSettingFromPLCInProcess)
               {
                    this->updateWeldPoitDisplay_Editting(wp2);
                   qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");
               }

           }
           //store any received point info into tooling info on background
           this->tooling_current->plcToolingInfo.weldPoint_List[pointNOFromPLC]=wp2;
           if(uploadingWholeSettingFromPLCInProcess)
           {
               qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");
               this->tempTooling_editting->plcToolingInfo.weldPoint_List[pointNOFromPLC]=wp2;

           }
           wp1=nullptr;
        break;
    }
    case 10://get station info from PLC , then update the display value
    {
        qDebug()<<tr("CommandNO:%1,got station info, station NO:%2").arg(command).arg((quint8)dataLoad[0]);

        dWordBytes dw1;
        quint8 stationNOFromPLC;
        stationNOFromPLC=dataLoad[0];
        //update display if station NO is desired
        if(stationNOFromPLC==this->ui->stationPara_stationNO->value())
        {
            this->ui->CV_stationPara_speedType->setCurrentIndex((int)dataLoad[1]);
            if(uploadingWholeSettingFromPLCInProcess)
            {
                this->ui->stationPara_speedType->setCurrentIndex((int)dataLoad[1]);
                qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

            }
            dw1.bytesVar.B0=dataLoad[2];
            dw1.bytesVar.B1=dataLoad[3];
            dw1.bytesVar.B2=dataLoad[4];
            dw1.bytesVar.B3=dataLoad[5];
            this->ui->CV_stationPara_setPoint->setValue((qint32)dw1.DWordVar);
            if(uploadingWholeSettingFromPLCInProcess)
            {
                this->ui->stationPara_setPoint->setValue((qint32)dw1.DWordVar);
                qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");


            }
            dw1.bytesVar.B0=dataLoad[6];
            dw1.bytesVar.B1=dataLoad[7];
            dw1.bytesVar.B2=dataLoad[8];
            dw1.bytesVar.B3=dataLoad[9];
            this->ui->CV_stationPara_upperLimit->setValue((qint32)dw1.DWordVar);
            if(uploadingWholeSettingFromPLCInProcess)
            {
                this->ui->stationPara_upperLimit->setValue((qint32)dw1.DWordVar);
                qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

            }
            dw1.bytesVar.B0=dataLoad[10];
            dw1.bytesVar.B1=dataLoad[11];
            dw1.bytesVar.B2=dataLoad[12];
            dw1.bytesVar.B3=dataLoad[13];
            this->ui->CV_stationPara_lowerLimit->setValue((qint32)dw1.DWordVar);
            if(uploadingWholeSettingFromPLCInProcess)
            {
                this->ui->stationPara_lowerLimit->setValue((qint32)dw1.DWordVar);
                qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");


            }

        }
        //store received station info into tooling info on background
        this->tooling_current->plcToolingInfo.station_List[stationNOFromPLC].speedType=(int)dataLoad[1];
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.station_List[stationNOFromPLC].speedType=(int)dataLoad[1];
            qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

        }
        dw1.bytesVar.B0=dataLoad[2];
        dw1.bytesVar.B1=dataLoad[3];
        dw1.bytesVar.B2=dataLoad[4];
        dw1.bytesVar.B3=dataLoad[5];
        this->tooling_current->plcToolingInfo.station_List[stationNOFromPLC].pos_setPoint=(qint32)dw1.DWordVar;
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.station_List[stationNOFromPLC].pos_setPoint=(qint32)dw1.DWordVar;
            qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

        }
        dw1.bytesVar.B0=dataLoad[6];
        dw1.bytesVar.B1=dataLoad[7];
        dw1.bytesVar.B2=dataLoad[8];
        dw1.bytesVar.B3=dataLoad[9];
        this->tooling_current->plcToolingInfo.station_List[stationNOFromPLC].pos_upperLimit=(qint32)dw1.DWordVar;
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.station_List[stationNOFromPLC].pos_upperLimit=(qint32)dw1.DWordVar;
            qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

        }
        dw1.bytesVar.B0=dataLoad[10];
        dw1.bytesVar.B1=dataLoad[11];
        dw1.bytesVar.B2=dataLoad[12];
        dw1.bytesVar.B3=dataLoad[13];
        this->tooling_current->plcToolingInfo.station_List[stationNOFromPLC].pos_lowerLimit=(qint32)dw1.DWordVar;
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.station_List[stationNOFromPLC].pos_lowerLimit=(qint32)dw1.DWordVar;
            qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

        }

    }
        break;
    case 11://get part Clamper(valve) info from PLC, then update the display value
    {
       quint8 valveNOFromPLC;
       valveNOFromPLC=dataLoad[0];
       //qDebug()<<"stepNOFromPLC"<<stepNOFromPLC<<"dataload[0]"<<dataLoad[0]<<"dataload[1]"<<dataLoad[1];
       //update display if the valve NO is desired
       if(valveNOFromPLC==this->ui->spinBox_valveNO->value())
       {
           this->ui->CV_valveStartStep->setValue(dataLoad[1]);
           this->ui->CV_valveEndStep->setValue(dataLoad[2]);
           this->ui->CV_valveActionConfirm->setValue(dataLoad[3]);
           this->ui->CV_valveEnableBypass->setValue(dataLoad[4]);
           this->ui->comboBox_ValveType->setCurrentIndex(dataLoad[5]);
           if(uploadingWholeSettingFromPLCInProcess)
           {
               this->ui->spinBox_valveEnable->setValue(dataLoad[4]);
               this->ui->comboBox_valveType->setCurrentIndex(dataLoad[5]);
               this->ui->spinBox_valveActionConfirm->setValue(dataLoad[3]);
               this->ui->spinBox_valveStartStep->setValue(dataLoad[1]);
               this->ui->spinBox_valveEndStep->setValue(dataLoad[2]);
               qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");


           }
       }
       //store received valve info into tooling info on background
       this->tooling_current->plcToolingInfo.pneumaticValvelist[valveNOFromPLC].startStep=dataLoad[1];
       this->tooling_current->plcToolingInfo.pneumaticValvelist[valveNOFromPLC].stopStep=dataLoad[2];
       this->tooling_current->plcToolingInfo.pneumaticValvelist[valveNOFromPLC].actionConfirm=dataLoad[3];
       this->tooling_current->plcToolingInfo.pneumaticValvelist[valveNOFromPLC].enable=dataLoad[4];
       this->tooling_current->plcToolingInfo.pneumaticValvelist[valveNOFromPLC].valveType=dataLoad[5];
       if(uploadingWholeSettingFromPLCInProcess)
       {
           this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[valveNOFromPLC].startStep=dataLoad[1];
           this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[valveNOFromPLC].stopStep=dataLoad[2];
           this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[valveNOFromPLC].actionConfirm=dataLoad[3];
           this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[valveNOFromPLC].enable=dataLoad[4];
           this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[valveNOFromPLC].valveType=dataLoad[5];
           qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

       }
       break;
    }
    case 12://get the servo speed setting and home direction setting on PLC,then update the dispaly value
    {

        qDebug()<<tr("CommandNO:%1,got servo info, speed settings").arg(command);

        dWordBytes dw1;
        qint32 dw_int_v1;
        dw1.bytesVar.B0=dataLoad[0];
        dw1.bytesVar.B1=dataLoad[1];
        dw1.bytesVar.B2=dataLoad[2];
        dw1.bytesVar.B3=dataLoad[3];
        dw_int_v1=(qint32)dw1.DWordVar/5;
        this->ui->CV_stationPara_HighSpeed->setValue(dw_int_v1);
        this->tooling_current->plcToolingInfo.servoSpeed_high=dw_int_v1;
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->ui->stationPara_HighSpeed->setValue(dw_int_v1);
            this->tempTooling_editting->plcToolingInfo.servoSpeed_high=dw_int_v1;
            qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

        }

        dw1.bytesVar.B0=dataLoad[4];
        dw1.bytesVar.B1=dataLoad[5];
        dw1.bytesVar.B2=dataLoad[6];
        dw1.bytesVar.B3=dataLoad[7];
        dw_int_v1=(qint32)dw1.DWordVar/5;
        this->ui->CV_stationPara_MediumSpeed->setValue(dw_int_v1);
        this->tooling_current->plcToolingInfo.servoSpeed_medium=dw_int_v1;
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->ui->stationPara_MediumSpeed->setValue(dw_int_v1);
            this->tempTooling_editting->plcToolingInfo.servoSpeed_medium=dw_int_v1;
            qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

        }
        dw1.bytesVar.B0=dataLoad[8];
        dw1.bytesVar.B1=dataLoad[9];
        dw1.bytesVar.B2=dataLoad[10];
        dw1.bytesVar.B3=dataLoad[11];

        dw_int_v1=(qint32)dw1.DWordVar/5;
        this->ui->CV_stationPara_lowSpeed->setValue(dw_int_v1);
        this->tooling_current->plcToolingInfo.servoSpeed_low=dw_int_v1;
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->ui->stationPara_LowSpeed->setValue(dw_int_v1);
            this->tempTooling_editting->plcToolingInfo.servoSpeed_low=dw_int_v1;
            qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

        }
        dw1.bytesVar.B0=dataLoad[12];
        dw1.bytesVar.B1=dataLoad[13];
        dw1.bytesVar.B2=dataLoad[14];
        dw1.bytesVar.B3=dataLoad[15];
        this->ui->CV_stationPara_defaultSpeed->setValue((qint32)dw1.DWordVar/5);
        dw_int_v1=(qint32)dw1.DWordVar/5;
        this->ui->CV_stationPara_defaultSpeed->setValue(dw_int_v1);
        //this->tooling_current->plcToolingInfo.servoSpeed_=dw_int_v1;

        this->ui->stationPara_DefaultSpeed->setValue(dw_int_v1);
        //this->tempTooling_editting->plcToolingInfo.servoSpeed_=dw_int_v1;

        this->ui->CV_servoPara_CWCCW->setCurrentIndex((int)dataLoad[16]);
        this->tooling_current->plcToolingInfo.servoHomingDir=(int)dataLoad[16];
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->ui->servoPara_CWCCW->setCurrentIndex((int)dataLoad[16]);
            this->tempTooling_editting->plcToolingInfo.servoHomingDir=(int)dataLoad[16];
            qDebug()<<tr("uploadingWholeSettingFromPLCInProcess:%1").arg(this->uploadingWholeSettingFromPLCInProcess?"true":"false");

        }
    }
        break;
    case 13://received  info from PLC for weld by manual
    {
        quint8 pointNOFromPLC;
        pointNOFromPLC=dataLoad[0];
        if(pointNOFromPLC==this->ui->WeldByManual_PointNO->value())
        {
            this->ui->weldByManual_currentStationNO->setValue((int)dataLoad[2]);
            this->ui->weldByManual_stepNO->setValue((int)dataLoad[1]);
            this->ui->weldByManual_targetStationNO->setValue((int)dataLoad[3]);
            dWordBytes dw1;
            dw1.bytesVar.B0=dataLoad[4];
            dw1.bytesVar.B1=dataLoad[5];
            dw1.bytesVar.B2=dataLoad[6];
            dw1.bytesVar.B3=dataLoad[7];
            this->ui->weldByManual_tartgetPos->setText(QString::number((qint32) dw1.DWordVar));
            dw1.bytesVar.B0=dataLoad[8];
            dw1.bytesVar.B1=dataLoad[9];
            dw1.bytesVar.B2=dataLoad[10];
            dw1.bytesVar.B3=dataLoad[11];
            this->ui->weldByManual_realtimePos->setText(QString::number((qint32) dw1.DWordVar));
            if(dataLoad[2]==dataLoad[3])
            {
                this->ui->weldByManual_Down->setEnabled(true);
                this->ui->weldByManual_runWeld->setEnabled(true);
            }
        }
    }
        break;
    case 15://received toolID from PLC
    {


        this->toolID_PLC=(quint8)dataLoad[2];
        qDebug()<<"toolID from PLC:"<<this->toolID_PLC;
        this->ui->toolID_fromPLC->setValue(this->toolID_PLC);
        this->tooling_current->plcToolingInfo.toolingNO=this->toolID_PLC;
        break;
    }
    case 16://received point bypass  info from PLC
    {
        qDebug()<<"received point bypass info from PLC";
        this->ui->CV_PointBypass_retain->setChecked(dataLoad[1]?true:false);
        bytebits bb1;
        bb1.byteChar=dataLoad[2];
        this->ui->CV_PointBypass_point_1->setChecked(bb1.bits.b0);
        this->ui->CV_PointBypass_point_2->setChecked(bb1.bits.b1);
        this->ui->CV_PointBypass_point_3->setChecked(bb1.bits.b2);
        this->ui->CV_PointBypass_point_4->setChecked(bb1.bits.b3);
        this->ui->CV_PointBypass_point_5->setChecked(bb1.bits.b4);
        this->ui->CV_PointBypass_point_6->setChecked(bb1.bits.b5);
        this->ui->CV_PointBypass_point_7->setChecked(bb1.bits.b6);
        this->ui->CV_PointBypass_point_8->setChecked(bb1.bits.b7);
        bb1.byteChar=dataLoad[3];
        this->ui->CV_PointBypass_point_9->setChecked(bb1.bits.b0);
        this->ui->CV_PointBypass_point_10->setChecked(bb1.bits.b1);
        this->ui->CV_PointBypass_point_11->setChecked(bb1.bits.b2);
        this->ui->CV_PointBypass_point_12->setChecked(bb1.bits.b3);
        this->ui->CV_PointBypass_point_13->setChecked(bb1.bits.b4);
        this->ui->CV_PointBypass_point_14->setChecked(bb1.bits.b5);
        this->ui->CV_PointBypass_point_15->setChecked(bb1.bits.b6);
        this->ui->CV_PointBypass_point_16->setChecked(bb1.bits.b7);
        break;
    }
    case 17://received feedback during dowloading/uploading whole settings
    {
        quint8 index=dataLoad[0];
        quint8 direction=dataLoad[1];
        qDebug()<<tr("get feed back during downloading uploading whole settings,direction:%1,index:%2")
                  .arg(direction).arg(index);
        this->ptrprogressDialog->setValue(index);
        QEventLoop eventloop;
        QTimer::singleShot(300, &eventloop, SLOT(quit()));
        eventloop.exec();
        this->execDownloadUploadWholeSettings(index+1,direction);
        break;
    }
    case 20://received cycle data from PLC
    {

        pointCycleData data1;
        data1.pointNO=dataLoad[0];
        data1.weldResult=dataLoad[1];
        data1.weldTime=((quint8)dataLoad[2])*256+((quint8)dataLoad[3]);        
        data1.peakPower=((quint8)dataLoad[4])*256+((quint8)dataLoad[5]);        
        data1.weldEnergy=((quint8)dataLoad[6])*256+((quint8)dataLoad[7]);        
        data1.thrusterDownPressure=((quint8)dataLoad[8])*256+((quint8)dataLoad[9]);
        data1.thrusterDownPressure=(data1.thrusterDownPressure-228)*500/(3868-228);

        data1.genNO=dataLoad[10];
        data1.channelCode=dataLoad[11];
        data1.holdTime=(quint8)dataLoad[12]*256+(quint8)dataLoad[13];
        data1.amplitude=dataLoad[14];
        qDebug()<<tr("received point cycle data,pointNO:%1,time:%2,power:%3,energy:%4").arg(data1.pointNO)
                  .arg(data1.weldTime).arg(data1.peakPower).arg(data1.weldEnergy);
        emit receivedPointCycleData(data1);
        break;
    }

    case 23://received filmfeeder Para from PLC
    {
        this->tooling_current->plcToolingInfo.feeder.enable=dataLoad[0]?1:0;
        this->tooling_current->plcToolingInfo.feeder.direction=dataLoad[1]?1:0;
        this->ui->CV_filmFeeder_Enable->setValue(this->tooling_current->plcToolingInfo.feeder.enable);
        this->ui->CV_filmFeeder_Dir->setCurrentIndex(this->tooling_current->plcToolingInfo.feeder.direction);
        if(uploadingWholeSettingFromPLCInProcess)
        {
           this->tempTooling_editting->plcToolingInfo.feeder.enable=
                    this->tooling_current->plcToolingInfo.feeder.enable;
           this->tempTooling_editting->plcToolingInfo.feeder.direction=
                    this->tooling_current->plcToolingInfo.feeder.direction;
           this->ui->spinBox_filmFeeder_enable->setValue
                    (this->tempTooling_editting->plcToolingInfo.feeder.enable);
           this->ui->comboBox_filmFeeder_Dir->setCurrentIndex
                    (this->tempTooling_editting->plcToolingInfo.feeder.direction);
        }
        dWordBytes dw1;
        qint32 dw_int_v1;
        dw1.bytesVar.B0=dataLoad[2];
        dw1.bytesVar.B1=dataLoad[3];
        dw1.bytesVar.B2=dataLoad[4];
        dw1.bytesVar.B3=dataLoad[5];
        dw_int_v1=(qint32)dw1.DWordVar;
        this->tooling_current->plcToolingInfo.feeder.speed=dw_int_v1;
        this->ui->CV_filmFeeder_speed->setValue(dw_int_v1);
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.feeder.speed=dw_int_v1;
            this->ui->spinBox_filmFeeder_speed->setValue(dw_int_v1);
        }

        dw1.bytesVar.B0=dataLoad[6];
        dw1.bytesVar.B1=dataLoad[7];
        dw1.bytesVar.B2=dataLoad[8];
        dw1.bytesVar.B3=dataLoad[9];
        dw_int_v1=(qint32)dw1.DWordVar;
        this->tooling_current->plcToolingInfo.feeder.distance_1=dw_int_v1;
        this->ui->CV_filmFeeder_distance->setValue(dw_int_v1);
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.feeder.distance_1=dw_int_v1;
            this->ui->spinBox_filmFeeder_distance->setValue(dw_int_v1);
        }

        dw1.bytesVar.B0=dataLoad[10];
        dw1.bytesVar.B1=dataLoad[11];
        dw1.bytesVar.B2=dataLoad[12];
        dw1.bytesVar.B3=dataLoad[13];
        dw_int_v1=(qint32)dw1.DWordVar;
        this->tooling_current->plcToolingInfo.feeder.distance_2=dw_int_v1;
        this->ui->CV_filmFeeder_distance_2->setValue(dw_int_v1);
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.feeder.distance_2=dw_int_v1;
            this->ui->spinBox_filmFeeder_distance_2->setValue(dw_int_v1);
        }
        this->tooling_current->plcToolingInfo.feeder.interval=(quint8)dataLoad[14];
        this->ui->CV_filmFeeder_Interval->setValue(this->tooling_current->plcToolingInfo.feeder.interval);
        if(uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.feeder.interval=
                    this->tooling_current->plcToolingInfo.feeder.interval;
            this->ui->spinBox_filmFeeder_Interval->setValue
                    (this->tempTooling_editting->plcToolingInfo.feeder.interval);
        }
        break;
    }
    case 100://change HMI page NO

        break;

    default:
        qDebug()<<"Command not match , no assigned hanlder";
        break;
    }


}
void MainWindow::setPLCValueVisible(bool flag)
{
    if(flag)
    {

        this->ui->btn_uploadFromPLC->setVisible(true);
        this->ui->btn_downLoadToPLC->setVisible(true);
        this->ui->CV_GenEnable_1->setVisible(true);
        this->ui->CV_GenEnable_2->setVisible(true);
        this->ui->CV_GenEnable_3->setVisible(true);
        this->ui->CV_GenEnable_4->setVisible(true);
        this->ui->btn_setGenEnable->setVisible(true);
        this->ui->btn_undoGenEnable->setVisible(true);
        this->ui->btn_getGenEnableStatus->setVisible(true);
        this->ui->PLC_Value_Label->setVisible(true);
        this->ui->PLC_Value_Label_trigByPower->setVisible(true);
        this->ui->btn_QueryThrusterConfig_2->setVisible(true);
        this->ui->PLC_Value_Label_2->setVisible(true);
        this->ui->btn_setThrusterConfig->setVisible(true);
        this->ui->btn_undoThrusterConfig->setVisible(true);
        this->ui->CV_thrusterEnable->setVisible(true);
        this->ui->CV_GenNO->setVisible(true);
        this->ui->CV_Channel->setVisible(true);

        this->ui->btn_getPointPara->setVisible(true);
        this->ui->btn_setPointPara->setVisible(true);

        this->ui->groupBox_PLC_Value_limits->setVisible(true);
        this->ui->PLC_Value_Label_3->setVisible(true);
        this->ui->CV_weldPointEnable->setVisible(true);
        this->ui->CV_thrusterNO_WPC->setVisible(true);
        this->ui->CV_stepNO_WPC->setVisible(true);


        this->ui->PLC_Value_Label_4->setVisible(true);
        this->ui->CV_pointPara_amp->setVisible(true);
        this->ui->CV_pointPara_trigAmp->setVisible(true);
        this->ui->CV_pointPara_trigPower->setVisible(true);
        this->ui->CV_pointPara_trigTimeout->setVisible(true);
        this->ui->CV_pointPara_weldTime->setVisible(true);
        this->ui->CV_pointPara_weldEnergy->setVisible(true);
        this->ui->CV_pointPara_peakPower->setVisible(true);

        this->ui->CV_pointPara_holdTime->setVisible(true);
        this->ui->CV_pointPara_trigBypower->setVisible(true);
        this->ui->PLC_Value_Label_5->setVisible(true);
        this->ui->CV_pointPara_freeRunFreq->setVisible(true);
        this->ui->CV_pointPara_freq_u_limit->setVisible(true);
        this->ui->CV_pointPara_freq_l_limit->setVisible(true);
        this->ui->TEST_Widgit->setVisible(true);
        this->ui->PLC_Value_Label_6->setVisible(true);
        this->ui->CV_pointPara_rampupTime->setVisible(true);
        this->ui->CV_pointPara_rampdownTime->setVisible(true);

        this->ui->CV_pointPara_afterburst_duration->setVisible(true);
        this->ui->CV_pointPara_afterburstDelay->setVisible(true);
        this->ui->CV_pointPara_downPressure->setVisible(true);
        //this->ui->CV_pointPara_upPressure->setVisible(true);
        this->ui->groupBox_PLC_Value_limits->setVisible(true);
        //enable/disable servo  operarion
        this->ui->tab_stepStation->setTabEnabled(0,true);
        this->ui->btn_QuerystationPara->setVisible(true);
        this->ui->stationConfig_servo_GOTO->setVisible(true);
        this->ui->btn_setStationPara->setVisible(true);
        this->ui->btn_undoStationPara->setVisible(true);

        this->ui->PLC_Value_Label_7->setVisible(true);
        this->ui->CV_stationPara_upperLimit->setVisible(true);
        this->ui->CV_stationPara_lowerLimit->setVisible(true);
        this->ui->CV_stationPara_setPoint->setVisible(true);
        this->ui->CV_stationPara_speedType->setVisible(true);
        this->ui->PLC_Value_Label_8->setVisible(true);
        this->ui->CV_stationPara_HighSpeed->setVisible(true);
        this->ui->CV_stationPara_MediumSpeed->setVisible(true);
        this->ui->CV_stationPara_lowSpeed->setVisible(true);
        this->ui->CV_stationPara_defaultSpeed->setVisible(true);
        this->ui->btn_QueryServoPara->setVisible(true);

        this->ui->CV_servoPara_CWCCW->setVisible(true);
        this->ui->btn_QueryStepStation->setVisible(true);
        this->ui->btn_undoStepsStationEdit->setVisible(true);
        this->ui->btn_setStationToStep->setVisible(true);
        this->ui->PLC_Value_Label_9->setVisible(true);
        this->ui->CV_stationNO->setVisible(true);


        this->ui->btn_QueryValveConfig->setVisible(true);
        this->ui->btn_setValveConfig->setVisible(true);
        this->ui->btn_undoValveConfig->setVisible(true);

        this->ui->PLC_Value_Label_10->setVisible(true);
        this->ui->CV_valveEnableBypass->setVisible(true);
        this->ui->comboBox_ValveType->setVisible(true);
        this->ui->CV_valveActionConfirm->setVisible(true);
        this->ui->CV_valveStartStep->setVisible(true);
        this->ui->CV_valveEndStep->setVisible(true);
        this->ui->btn_setServoPara->setVisible(true);
        this->ui->btn_undoServoSetting->setVisible(true);
        this->ui->btn_PLC2Editting_WeldPoint->setVisible(true);

        this->ui->btn_refresh_FilmFeeder->setVisible(true);
        this->ui->btn_Edit2PLC_FilmFeeder->setVisible(true);
        this->ui->btn_PLC2Edit_FilmFeeder->setVisible(true);
        this->ui->CV_filmFeeder_Enable->setVisible(true);
        this->ui->CV_filmFeeder_speed->setVisible(true);
        this->ui->CV_filmFeeder_distance->setVisible(true);
        this->ui->CV_filmFeeder_distance_2->setVisible(true);
        this->ui->CV_filmFeeder_Interval->setVisible(true);
        this->ui->CV_filmFeeder_Dir->setVisible(true);

        this->ui->btn_filmfeeder_JOG1->setVisible(true);
        this->ui->btn_filmfeeder_JOG2->setVisible(true);
        this->ui->btn_filmfeeder_RunRelative->setVisible(true);
        this->ui->btn_filmfeeder_RunSpeed->setVisible(true);

        this->ui->btn_refresh_PartSensor->setVisible(true);
        this->ui->btn_Edit2PLC_PartSensor->setVisible(true);
        this->ui->btn_PLC2Edit_PartSensor->setVisible(true);
        this->ui->groupBox_PartSensor_PLC->setVisible(true);
        this->ui->btn_refresh_valveSensor->setVisible(true);
        this->ui->btn_Edit2PLC_valveSensor->setVisible(true);
        this->ui->btn_PLC2Edit_valveSensor->setVisible(true);
        this->ui->groupBox_ValveSensor_PLC->setVisible(true);
    }
    else
    {
        this->ui->btn_uploadFromPLC->setVisible(false);
        this->ui->btn_downLoadToPLC->setVisible(false);
        this->ui->CV_GenEnable_1->setVisible(false);
        this->ui->CV_GenEnable_2->setVisible(false);
        this->ui->CV_GenEnable_3->setVisible(false);
        this->ui->CV_GenEnable_4->setVisible(false);
        this->ui->btn_setGenEnable->setVisible(false);
        this->ui->btn_undoGenEnable->setVisible(false);
        this->ui->btn_getGenEnableStatus->setVisible(false);
        this->ui->PLC_Value_Label->setVisible(false);
        this->ui->btn_QueryThrusterConfig_2->setVisible(false);
        this->ui->PLC_Value_Label_2->setVisible(false);
        this->ui->btn_setThrusterConfig->setVisible(false);
        this->ui->btn_undoThrusterConfig->setVisible(false);
        this->ui->CV_thrusterEnable->setVisible(false);
        this->ui->CV_GenNO->setVisible(false);
        this->ui->CV_Channel->setVisible(false);

        this->ui->btn_getPointPara->setVisible(false);
        this->ui->btn_setPointPara->setVisible(false);

        this->ui->groupBox_PLC_Value_limits->setVisible(false);
        this->ui->PLC_Value_Label_3->setVisible(false);
        this->ui->CV_weldPointEnable->setVisible(false);
        this->ui->CV_thrusterNO_WPC->setVisible(false);
        this->ui->CV_stepNO_WPC->setVisible(false);
        this->ui->PLC_Value_Label_4->setVisible(false);
        this->ui->CV_pointPara_amp->setVisible(false);
        this->ui->CV_pointPara_trigAmp->setVisible(false);
        this->ui->CV_pointPara_trigPower->setVisible(false);
        this->ui->CV_pointPara_trigTimeout->setVisible(false);
        this->ui->CV_pointPara_weldTime->setVisible(false);
        this->ui->CV_pointPara_weldEnergy->setVisible(false);
        this->ui->CV_pointPara_peakPower->setVisible(false);

        this->ui->CV_pointPara_holdTime->setVisible(false);
        this->ui->CV_pointPara_trigBypower->setVisible(false);
        this->ui->PLC_Value_Label_5->setVisible(false);
        this->ui->CV_pointPara_freeRunFreq->setVisible(false);
        this->ui->CV_pointPara_freq_u_limit->setVisible(false);
        this->ui->CV_pointPara_freq_l_limit->setVisible(false);
        this->ui->TEST_Widgit->setVisible(false);
        this->ui->PLC_Value_Label_6->setVisible(false);
        this->ui->CV_pointPara_rampupTime->setVisible(false);
        this->ui->CV_pointPara_rampdownTime->setVisible(false);

        this->ui->CV_pointPara_afterburst_duration->setVisible(false);
        this->ui->CV_pointPara_afterburstDelay->setVisible(false);
        this->ui->CV_pointPara_downPressure->setVisible(false);
        //this->ui->CV_pointPara_upPressure->setVisible(false);
        this->ui->groupBox_PLC_Value_limits->setVisible(false);
        //enable/disable servo  operarion
        this->ui->tab_stepStation->setTabEnabled(0,false);
        this->ui->btn_QuerystationPara->setVisible(false);
        this->ui->stationConfig_servo_GOTO->setVisible(false);
        this->ui->btn_setStationPara->setVisible(false);
        this->ui->btn_undoStationPara->setVisible(false);
        this->ui->btn_QueryServoPara->setVisible(false);

        this->ui->PLC_Value_Label_7->setVisible(false);
        this->ui->CV_stationPara_upperLimit->setVisible(false);
        this->ui->CV_stationPara_lowerLimit->setVisible(false);
        this->ui->CV_stationPara_setPoint->setVisible(false);
        this->ui->CV_stationPara_speedType->setVisible(false);
        this->ui->PLC_Value_Label_8->setVisible(false);
        this->ui->CV_stationPara_HighSpeed->setVisible(false);
        this->ui->CV_stationPara_MediumSpeed->setVisible(false);
        this->ui->CV_stationPara_lowSpeed->setVisible(false);
        this->ui->CV_stationPara_defaultSpeed->setVisible(false);

        this->ui->CV_servoPara_CWCCW->setVisible(false);
        this->ui->btn_QueryStepStation->setVisible(false);
        this->ui->btn_undoStepsStationEdit->setVisible(false);
        this->ui->btn_setStationToStep->setVisible(false);
        this->ui->PLC_Value_Label_9->setVisible(false);
        this->ui->CV_stationNO->setVisible(false);

        this->ui->btn_QueryValveConfig->setVisible(false);
        this->ui->btn_setValveConfig->setVisible(false);
        this->ui->btn_undoValveConfig->setVisible(false);
        this->ui->PLC_Value_Label_trigByPower->setVisible(false);
        this->ui->PLC_Value_Label_10->setVisible(false);
        this->ui->CV_valveEnableBypass->setVisible(false);
        this->ui->comboBox_ValveType->setVisible(false);
        this->ui->CV_valveActionConfirm->setVisible(false);
        this->ui->CV_valveStartStep->setVisible(false);
        this->ui->CV_valveEndStep->setVisible(false);
        this->ui->btn_setServoPara->setVisible(false);
        this->ui->btn_undoServoSetting->setVisible(false);
        this->ui->btn_PLC2Editting_WeldPoint->setVisible(false);

        this->ui->btn_refresh_FilmFeeder->setVisible(false);
        this->ui->btn_Edit2PLC_FilmFeeder->setVisible(false);
        this->ui->btn_PLC2Edit_FilmFeeder->setVisible(false);
        this->ui->CV_filmFeeder_Enable->setVisible(false);
        this->ui->CV_filmFeeder_speed->setVisible(false);
        this->ui->CV_filmFeeder_distance->setVisible(false);
        this->ui->CV_filmFeeder_distance_2->setVisible(false);
        this->ui->CV_filmFeeder_Interval->setVisible(false);
        this->ui->CV_filmFeeder_Dir->setVisible(false);

        this->ui->btn_filmfeeder_JOG1->setVisible(false);
        this->ui->btn_filmfeeder_JOG2->setVisible(false);
        this->ui->btn_filmfeeder_RunRelative->setVisible(false);
        this->ui->btn_filmfeeder_RunSpeed->setVisible(false);

        this->ui->btn_refresh_PartSensor->setVisible(false);
        this->ui->btn_Edit2PLC_PartSensor->setVisible(false);
        this->ui->btn_PLC2Edit_PartSensor->setVisible(false);
        this->ui->groupBox_PartSensor_PLC->setVisible(false);
        this->ui->btn_refresh_valveSensor->setVisible(false);
        this->ui->btn_Edit2PLC_valveSensor->setVisible(false);
        this->ui->btn_PLC2Edit_valveSensor->setVisible(false);
        this->ui->groupBox_ValveSensor_PLC->setVisible(false);

    }

}
void MainWindow::updateWeldPoitDisplay_Editting(const weldPoint& wp2)
{


   this->ui->spinBox_pointPara_afterburstDelay->setValue(wp2.ultrasonicPara.AfterBurstdelay0to999);
   this->ui->spinBox_pointPara_afterburst_duration->setValue(wp2.ultrasonicPara.Afterburst_Duration0t999);
   this->ui->spinBox_pointPara_amp->setValue(wp2.ultrasonicPara.Amplitude20to100);
   this->ui->spinBox_pointPara_downPressure->setValue(wp2.thrusterPressure_down);
    this->ui->spinbox_pointPara_freeRunFreq->setValue(wp2.ultrasonicPara.Free_Run_Frequency);
    this->ui->spinbox_pointPara_freq_l_limit->setValue(wp2.ultrasonicPara.Freq_Lower_Limit);
    this->ui->spinbox_pointPara_freq_u_limit->setValue(wp2.ultrasonicPara.Freq_Upper_Limit);
    this->ui->spinBox_pointPara_holdTime->setValue(wp2.ultrasonicPara.Hold_TIme0to30000);
    this->ui->spinBox_pointPara_peakPower->setValue(wp2.ultrasonicPara.Peak_Power);
    this->ui->spinBox_pointPara_rampdownTime->setValue(wp2.ultrasonicPara.Ramp_Down_TIme0to250);
    this->ui->spinBox_pointPara_rampupTime->setValue(wp2.ultrasonicPara.Ramp_up_Time20to1250);
    this->ui->spinBox_pointPara_trigAmp->setValue(wp2.ultrasonicPara.Trigger_Amplitude);
    this->ui->spinBox_pointPara_trigByPower->setValue(wp2.ultrasonicPara.B64_enableTrigByPower.bits.b0?1:0);
    this->ui->spinBox_pointPara_trigPower->setValue(wp2.ultrasonicPara.Trigger_Power);
    this->ui->spinBox_pointPara_trigTimeout->setValue(wp2.ultrasonicPara.Trigger_Timeout);
    //this->ui->spinBox_pointPara_upPressure->setValue(wp2.thrusterPressure_up);
    this->ui->spinBox_pointPara_weldEnergy->setValue(wp2.ultrasonicPara.Energy0to65535);
    this->ui->spinBox_pointPara_weldTime->setValue(wp2.ultrasonicPara.Weld_Time0to30000);
    this->ui->spinBox_timeLowerLimit_BAD->setValue(wp2.ultrasonicPara.Badpart_MinTime_Limits);
    this->ui->spinBox_timeLowerLimit_Suspect->setValue(wp2.ultrasonicPara.suspect_MinTime_Limits);
    this->ui->spinBox_timeUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxTime_limits);
    this->ui->spinBox_timeUpperLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MaxTime_Limits);
    this->ui->spinBox_powerLowerLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MinPower_Limits);
    this->ui->spinBox_powerLowerLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MinPower_Limits);
    this->ui->spinBox_powerUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxPower_Limits);
    this->ui->spinBox_powerUpperLimit_Suspect_2->setValue(wp2.ultrasonicPara.Suspect_MaxPower_Limits);
    this->ui->spinBox_energyLowerLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MinEnergy_Limits);
    this->ui->spinBox_energyLowerLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MinEnergy_Limits);
    this->ui->spinBox_energyUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxEnergy_Limits);
    this->ui->spinBox_energyUpperLimit_Suspect_2->setValue(wp2.ultrasonicPara.Suspect_MaxEnergy_Limits);
    this->ui->checkBox_LimitsEnable_BAD->setCheckState(wp2.ultrasonicPara.B21_EnableSuspectBadByte.bits.b1?Qt::Checked:Qt::Unchecked);
    this->ui->checkBox_LimitsEnable_SUSPECT->setCheckState(wp2.ultrasonicPara.B21_EnableSuspectBadByte.bits.b0?Qt::Checked:Qt::Unchecked);
    this->ui->spinBox_weldPointEnable->setValue(wp2.enable?1:0);
    this->ui->spinBox_thrusterNOWPC->setValue(wp2.ThrusterNO);
    this->ui->spinBox_stepNO_WPC->setValue(wp2.stepNO);

}
void MainWindow::updateWeldPoitDisplay_Current(const weldPoint& wp2)
{

   this->ui->CV_pointPara_afterburstDelay->setValue(wp2.ultrasonicPara.AfterBurstdelay0to999);
   this->ui->CV_pointPara_afterburst_duration->setValue(wp2.ultrasonicPara.Afterburst_Duration0t999);
   this->ui->CV_pointPara_amp->setValue(wp2.ultrasonicPara.Amplitude20to100);
   this->ui->CV_pointPara_downPressure->setValue(wp2.thrusterPressure_down);
   this->ui->CV_pointPara_freeRunFreq->setValue(wp2.ultrasonicPara.Free_Run_Frequency);
   this->ui->CV_pointPara_freq_l_limit->setValue(wp2.ultrasonicPara.Freq_Lower_Limit);
   this->ui->CV_pointPara_freq_u_limit->setValue(wp2.ultrasonicPara.Freq_Upper_Limit);
   this->ui->CV_pointPara_holdTime->setValue(wp2.ultrasonicPara.Hold_TIme0to30000);
   this->ui->CV_pointPara_peakPower->setValue(wp2.ultrasonicPara.Peak_Power);
   this->ui->CV_pointPara_rampdownTime->setValue(wp2.ultrasonicPara.Ramp_Down_TIme0to250);
   this->ui->CV_pointPara_rampupTime->setValue(wp2.ultrasonicPara.Ramp_up_Time20to1250);
   this->ui->CV_pointPara_trigAmp->setValue(wp2.ultrasonicPara.Trigger_Amplitude);
   this->ui->CV_pointPara_trigBypower->setValue(wp2.ultrasonicPara.B64_enableTrigByPower.bits.b0?1:0);
   this->ui->CV_pointPara_trigPower->setValue(wp2.ultrasonicPara.Trigger_Power);
   this->ui->CV_pointPara_trigTimeout->setValue(wp2.ultrasonicPara.Trigger_Timeout);
   //this->ui->CV_pointPara_upPressure->setValue(wp2.thrusterPressure_up);
   this->ui->CV_pointPara_weldEnergy->setValue(wp2.ultrasonicPara.Energy0to65535);
   this->ui->CV_pointPara_weldTime->setValue(wp2.ultrasonicPara.Weld_Time0to30000);
   this->ui->CV_timeLowerLimit_BAD->setValue(wp2.ultrasonicPara.Badpart_MinTime_Limits);
   this->ui->CV_timeLowerLimit_Suspect->setValue(wp2.ultrasonicPara.suspect_MinTime_Limits);
   this->ui->CV_timeUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxTime_limits);
   this->ui->CV_timeUpperLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MaxTime_Limits);
   this->ui->CV_powerLowerLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MinPower_Limits);
   this->ui->CV_powerLowerLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MinPower_Limits);
   this->ui->CV_powerUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxPower_Limits);
   this->ui->CV_powerUpperLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MaxPower_Limits);
   this->ui->CV_energyLowerLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MinEnergy_Limits);
   this->ui->CV_energyLowerLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MinEnergy_Limits);
   this->ui->CV_energyUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxEnergy_Limits);
   this->ui->CV_energyUpperLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MaxEnergy_Limits);
   this->ui->CV_pointPara_LimitsEnable_BAD->setCheckState(wp2.ultrasonicPara.B21_EnableSuspectBadByte.bits.b1?Qt::Checked:Qt::Unchecked);
   this->ui->CV_pointPara_LimitsEnable_SUSPECT->setCheckState(wp2.ultrasonicPara.B21_EnableSuspectBadByte.bits.b0?Qt::Checked:Qt::Unchecked);
   this->ui->CV_weldPointEnable->setValue(wp2.enable?1:0);
   this->ui->CV_thrusterNO_WPC->setValue(wp2.ThrusterNO);
   this->ui->CV_stepNO_WPC->setValue(wp2.stepNO);
}
void MainWindow::updateDisplay_All_EdittingValue()
{
    //update generator page
    this->ui->checkBox_GenEnable_1->setChecked(this->tempTooling_editting->plcToolingInfo.generator_enable[1]);
    this->ui->checkBox_GenEnable_2->setChecked(this->tempTooling_editting->plcToolingInfo.generator_enable[2]);
    this->ui->checkBox_GenEnable_3->setChecked(this->tempTooling_editting->plcToolingInfo.generator_enable[3]);
    this->ui->checkBox_GenEnable_4->setChecked(this->tempTooling_editting->plcToolingInfo.generator_enable[4]);
    //update thruster page
    if(this->ui->spinBox_thrusterNO->value()==0)
    {
     this->ui->spinBox_thrusterNO->setValue(1);
    }

    this->ui->spinBox_thrusterEnable->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[this->ui->spinBox_thrusterNO->value()].enable);
    this->ui->spinBox_GenNO->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[this->ui->spinBox_thrusterNO->value()].GenNO);
    this->ui->spinBox_channel->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[this->ui->spinBox_thrusterNO->value()].ChannelNO);
    //update weld point page
    if(this->ui->spinBox_PointNO_pointPara->value()==0)
    {
       this->ui->spinBox_PointNO_pointPara->setValue(1);
    }

    this->ui->PointName_display->setText(this->tempTooling_editting->pointNameMapping[this->ui->spinBox_PointNO_pointPara->value()]);
    //this->ui->comboBox_barcode_assignment->setCurrentIndex(this->tempTooling_editting->pointBarcodeMapping[this->ui->spinBox_PointNO_pointPara->value()]);
    this->ui->comboBox_barcode_assignment->setCurrentText(this->tempTooling_editting->pointBarcodeMapping[this->ui->spinBox_PointNO_pointPara->value()]);
    //dispaly ultrasonic pararmeters
    this->updateWeldPoitDisplay_Editting(this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()]);
    //update station setting page
    if(this->ui->stationPara_stationNO->value()==0)
    {
      this->ui->stationPara_stationNO->setValue(1);
    }
    this->ui->stationPara_upperLimit->setValue(this->tempTooling_editting->plcToolingInfo.station_List
                                               [this->ui->stationPara_stationNO->value()].pos_upperLimit);
    this->ui->stationPara_lowerLimit->setValue(this->tempTooling_editting->plcToolingInfo.station_List
                                               [this->ui->stationPara_stationNO->value()].pos_lowerLimit);
    this->ui->stationPara_setPoint->setValue(this->tempTooling_editting->plcToolingInfo.station_List
                                             [this->ui->stationPara_stationNO->value()].pos_setPoint);
    this->ui->stationPara_speedType->setCurrentIndex(this->tempTooling_editting->plcToolingInfo.station_List
                                                     [this->ui->stationPara_stationNO->value()].speedType);
    this->ui->stationPara_HighSpeed->setValue(this->tempTooling_editting->plcToolingInfo.servoSpeed_high);
    this->ui->stationPara_MediumSpeed->setValue(this->tempTooling_editting->plcToolingInfo.servoSpeed_medium);
    this->ui->stationPara_LowSpeed->setValue(this->tempTooling_editting->plcToolingInfo.servoSpeed_low);
    this->ui->servoPara_CWCCW->setCurrentIndex(this->tempTooling_editting->plcToolingInfo.servoHomingDir?1:0);
    //update stepStation Connection page
    if(this->ui->spinBox_stepNO->value()==0)
    {
      this->ui->spinBox_stepNO->setValue(1);
    }

    this->ui->spinBox_stationNO->setValue(this->tempTooling_editting->plcToolingInfo.stepStationConnection
                                          [this->ui->spinBox_stepNO->value()]);
    //update valve page
    if(this->ui->spinBox_valveNO->value()==0)
    {
        this->ui->spinBox_valveNO->setValue(1);
    }

    this->ui->valveName_lineEdit->setText(this->tempTooling_editting->valveNameMapping
                                          [this->ui->spinBox_valveNO->value()]);
    this->ui->spinBox_valveEnable->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist
                                            [this->ui->spinBox_valveNO->value()].enable?1:0);
    this->ui->comboBox_valveType->setCurrentIndex(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist
                                                  [this->ui->spinBox_valveNO->value()].valveType);
    this->ui->spinBox_valveActionConfirm->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist
                                                   [this->ui->spinBox_valveNO->value()].actionConfirm?1:0);
    this->ui->spinBox_valveStartStep->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist
                                               [this->ui->spinBox_valveNO->value()].startStep);
    this->ui->spinBox_valveEndStep->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist
                                             [this->ui->spinBox_valveNO->value()].stopStep);
    //update barcode page
    this->ui->lineEdit_barcode_prefix->setText(this->tempTooling_editting->leftBarcodeSettings.prefix);
    this->ui->lineEdit_barcode_suffix->setText(this->tempTooling_editting->leftBarcodeSettings.suffix);
    this->ui->checkBox_barcode_left_enable->setChecked(this->tempTooling_editting->leftBarcodeSettings.enable);
    this->ui->checkBox_barcode_right_enable->setChecked(this->tempTooling_editting->rightBarcodeSettings.enable);
    this->ui->comboBox_BarcodePort_Left->setCurrentText(this->tempTooling_editting->leftBarcodeSettings.portName);
    this->ui->comboBox__BarcodePort_Right->setCurrentText(this->tempTooling_editting->rightBarcodeSettings.portName);
    this->ui->comboBox_Baud->setCurrentText(this->tempTooling_editting->leftBarcodeSettings.baud);
    this->ui->spinBox_barcodeDataBits->setValue(this->tempTooling_editting->leftBarcodeSettings.dataBits);
    this->ui->spinBox_barcodeStopBits->setValue(this->tempTooling_editting->leftBarcodeSettings.stopBits);
    this->ui->comboBox_parity->setCurrentText(this->tempTooling_editting->leftBarcodeSettings.parity);
    this->ui->spinBox_barcodeMaxLength->setValue(this->tempTooling_editting->leftBarcodeSettings.maxLength);
    this->ui->spinBox_BarcodeMinLength->setValue(this->tempTooling_editting->leftBarcodeSettings.minLength);
    //update filmFeeder page
    this->ui->spinBox_filmFeeder_enable->setValue(this->tempTooling_editting->plcToolingInfo.feeder.enable);
    this->ui->spinBox_filmFeeder_speed->setValue(this->tempTooling_editting->plcToolingInfo.feeder.speed);
    this->ui->spinBox_filmFeeder_distance->setValue(this->tempTooling_editting->plcToolingInfo.feeder.distance_1);
    this->ui->spinBox_filmFeeder_distance_2->setValue(this->tempTooling_editting->plcToolingInfo.feeder.distance_2);
    this->ui->spinBox_filmFeeder_Interval->setValue(this->tempTooling_editting->plcToolingInfo.feeder.interval);
    this->ui->comboBox_filmFeeder_Dir->setCurrentIndex(this->tempTooling_editting->plcToolingInfo.feeder.direction);
    //update part sensor page
    this->ui->checkBox_PartSensor_1->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b1);
    this->ui->checkBox_PartSensor_2->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b2);
    this->ui->checkBox_PartSensor_3->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b3);
    this->ui->checkBox_PartSensor_4->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b4);
    this->ui->checkBox_PartSensor_5->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b5);
    this->ui->checkBox_PartSensor_6->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b6);
    this->ui->checkBox_PartSensor_7->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b1);
    this->ui->checkBox_PartSensor_8->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b2);
    this->ui->checkBox_PartSensor_9->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b3);
    this->ui->checkBox_PartSensor_10->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b4);
    this->ui->checkBox_PartSensor_11->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b5);
    this->ui->checkBox_PartSensor_12->setChecked(this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b6);

}
void MainWindow::updateDisplay_ALL_CurrentValue()
{
    //generator
    this->ui->CV_GenEnable_1->setChecked(this->tooling_current->plcToolingInfo.generator_enable[1]);
    this->ui->CV_GenEnable_2->setChecked(this->tooling_current->plcToolingInfo.generator_enable[2]);
    this->ui->CV_GenEnable_3->setChecked(this->tooling_current->plcToolingInfo.generator_enable[3]);
    this->ui->CV_GenEnable_4->setChecked(this->tooling_current->plcToolingInfo.generator_enable[4]);
    //thruster
    this->ui->CV_thrusterEnable->setValue(this->tooling_current->plcToolingInfo.thruster_List
                                          [this->ui->spinBox_thrusterNO->value()].enable?1:0);
    this->ui->CV_GenNO->setValue(this->tooling_current->plcToolingInfo.thruster_List
                                 [this->ui->spinBox_thrusterNO->value()].GenNO);
    this->ui->CV_Channel->setValue(this->tooling_current->plcToolingInfo.thruster_List
                                   [this->ui->spinBox_thrusterNO->value()].ChannelNO);

    //display weld point para
    this->updateWeldPoitDisplay_Current(this->tooling_current->plcToolingInfo.weldPoint_List
                                              [this->ui->spinBox_PointNO_pointPara->value()]);
    //stepstation and stations
    this->ui->CV_stationNO->setValue(this->tooling_current->plcToolingInfo.stepStationConnection
                                     [this->ui->spinBox_stepNO->value()]);
    this->ui->CV_stationPara_upperLimit->setValue(this->tooling_current->plcToolingInfo.station_List
                                                  [this->ui->stationPara_stationNO->value()].pos_upperLimit);
    this->ui->CV_stationPara_lowerLimit->setValue(this->tooling_current->plcToolingInfo.station_List
                                                  [this->ui->stationPara_stationNO->value()].pos_lowerLimit);
    this->ui->CV_stationPara_setPoint->setValue(this->tooling_current->plcToolingInfo.station_List
                                                  [this->ui->stationPara_stationNO->value()].pos_setPoint);
    this->ui->CV_stationPara_speedType->setCurrentIndex(this->tooling_current->plcToolingInfo.station_List
                                                  [this->ui->stationPara_stationNO->value()].speedType);
    //servo speed
    this->ui->CV_stationPara_HighSpeed->setValue(this->tooling_current->plcToolingInfo.servoSpeed_high);
    this->ui->CV_stationPara_MediumSpeed->setValue(this->tooling_current->plcToolingInfo.servoSpeed_medium);
    this->ui->CV_stationPara_lowSpeed->setValue(this->tooling_current->plcToolingInfo.servoSpeed_low);
    //this->ui->CV_stationPara_defaultSpeed->setValue(this->tooling_current->plcToolingInfo.defaultSpeed);
    this->ui->CV_servoPara_CWCCW->setCurrentIndex(this->tooling_current->plcToolingInfo.servoHomingDir?1:0);
    //valves
    this->ui->CV_valveEnableBypass->setValue(this->tooling_current->plcToolingInfo.pneumaticValvelist
                                             [this->ui->spinBox_valveNO->value()].enable?1:0);
    this->ui->comboBox_ValveType->setCurrentIndex(this->tooling_current->plcToolingInfo.pneumaticValvelist
                                             [this->ui->spinBox_valveNO->value()].valveType);

    this->ui->CV_valveActionConfirm->setValue(this->tooling_current->plcToolingInfo.pneumaticValvelist
                                             [this->ui->spinBox_valveNO->value()].actionConfirm?1:0);
    this->ui->CV_valveStartStep->setValue(this->tooling_current->plcToolingInfo.pneumaticValvelist
                                             [this->ui->spinBox_valveNO->value()].startStep);
    this->ui->CV_valveStartStep->setValue(this->tooling_current->plcToolingInfo.pneumaticValvelist
                                             [this->ui->spinBox_valveNO->value()].stopStep);
    //filmFeeder page
    this->ui->CV_filmFeeder_Enable->setValue(this->tooling_current->plcToolingInfo.feeder.enable);
    this->ui->CV_filmFeeder_speed->setValue(this->tooling_current->plcToolingInfo.feeder.speed);
    this->ui->CV_filmFeeder_distance->setValue(this->tooling_current->plcToolingInfo.feeder.distance_1);
    this->ui->CV_filmFeeder_distance_2->setValue(this->tooling_current->plcToolingInfo.feeder.distance_2);
    this->ui->CV_filmFeeder_Interval->setValue(this->tooling_current->plcToolingInfo.feeder.interval);
    this->ui->CV_filmFeeder_Dir->setCurrentIndex(this->tooling_current->plcToolingInfo.feeder.direction);
    //part sensors
    this->ui->CV_PartSensor_1->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b1);
    this->ui->CV_PartSensor_2->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b2);
    this->ui->CV_PartSensor_3->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b3);
    this->ui->CV_PartSensor_4->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b4);
    this->ui->CV_PartSensor_5->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b5);
    this->ui->CV_PartSensor_6->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.extendBypass.bits.b6);

    this->ui->CV_PartSensor_7->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b1);
    this->ui->CV_PartSensor_8->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b2);
    this->ui->CV_PartSensor_9->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b3);
    this->ui->CV_PartSensor_10->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b4);
    this->ui->CV_PartSensor_11->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b5);
    this->ui->CV_PartSensor_12->setChecked(this->tooling_current->plcToolingInfo.partSensorBypass.retractBypass.bits.b6);
    //valve sensor
    this->ui->CV_ValveExtendSensorBypass_1->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                       [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b1);
    this->ui->CV_ValveExtendSensorBypass_2->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                       [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b2);
    this->ui->CV_ValveExtendSensorBypass_3->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                       [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b3);
    this->ui->CV_ValveExtendSensorBypass_4->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                       [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b4);
    this->ui->CV_ValveExtendSensorBypass_5->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                       [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b5);
    this->ui->CV_ValveExtendSensorBypass_6->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                       [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b6);
    this->ui->CV_ValveRetractSensorBypass_1->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                        [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b1);
    this->ui->CV_ValveRetractSensorBypass_2->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                        [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b2);
    this->ui->CV_ValveRetractSensorBypass_3->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                        [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b3);
    this->ui->CV_ValveRetractSensorBypass_4->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                        [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b4);
    this->ui->CV_ValveRetractSensorBypass_5->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                        [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b5);
    this->ui->CV_ValveRetractSensorBypass_6->setChecked(this->tooling_current->plcToolingInfo.valveSensorBypass
                                                        [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b6);
}

void MainWindow::on_btn_setThrusterConfig_clicked()
{
    //set thruster config
    qDebug()<<"set thruster config btn clicked";
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x72;//commandNO low byte,114
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->spinBox_thrusterNO->value();
    dataToTcpCommObj[7]=this->ui->spinBox_GenNO->value();
    dataToTcpCommObj[8]=(this->ui->spinBox_channel->value()>0)?(this->ui->spinBox_channel->value()-1):0;
    dataToTcpCommObj[9]=this->ui->spinBox_thrusterEnable->value();
    dataToTcpCommObj[10]=0x00;
    dataToTcpCommObj[11]=0x00;
    //emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    //query thruster  config
    dataToTcpCommObj[12]=0x00;//length high byte
    dataToTcpCommObj[13]=0x06;//length low byte
    dataToTcpCommObj[14]=0x00;//commandNO high byte
    dataToTcpCommObj[15]=0x68;//commandNO low byte,104
    dataToTcpCommObj[16]=0x00;//reserve byte
    dataToTcpCommObj[17]=0x00;//reserve byte
    if(this->tcpConnectionStatus_send)
    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_QueryThrusterConfig_2_clicked()
{
    //query thruster config
    qDebug()<<"query thruster config btn_2 clicked";
    if(this->tcpConnectionStatus_send)
    emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getThrusterConfigFromPLC());
}

void MainWindow::on_btn_undoThrusterConfig_clicked()
{
   //qDebug()<<"query undo thruster config btn_2 clicked";
   this->ui->spinBox_GenNO->setValue(this->ui->CV_GenNO->value());
   this->ui->spinBox_channel->setValue(this->ui->CV_Channel->value());
   this->ui->spinBox_thrusterEnable->setValue(this->ui->CV_thrusterEnable->value());
}

void MainWindow::on_btn_setStationToStep_clicked()
{
    //set step station config
    qDebug()<<"set step station btn clicked";
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x74;//commandNO low byte,116
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->spinBox_stepNO->value();
    dataToTcpCommObj[7]=this->ui->spinBox_stationNO->value();
    dataToTcpCommObj[8]=0x00;
    dataToTcpCommObj[9]=0x00;

    //emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    //query step station  config
    dataToTcpCommObj[10]=0x00;//length high byte
    dataToTcpCommObj[11]=0x06;//length low byte
    dataToTcpCommObj[12]=0x00;//commandNO high byte
    dataToTcpCommObj[13]=0x6A;//commandNO low byte,106
    dataToTcpCommObj[14]=0x00;//reserve byte
    dataToTcpCommObj[15]=0x00;//reserve byte
    if(this->tcpConnectionStatus_send)
    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_QueryStepStation_clicked()
{
    if(this->tcpConnectionStatus_send)
    emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getStepStationConnectionFromPLC());
}

void MainWindow::on_btn_undoStepsStationEdit_clicked()
{
    this->ui->spinBox_stationNO->setValue(this->ui->CV_stationNO->value());
}


void MainWindow::on_btn_setValveConfig_clicked()
{
    //set valve config
    qDebug()<<"set valve btn clicked";
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0E;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x76;//commandNO low byte,118
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->spinBox_valveNO->value();
    dataToTcpCommObj[7]=this->ui->spinBox_valveStartStep->value();
    dataToTcpCommObj[8]=this->ui->spinBox_valveEndStep->value();
    dataToTcpCommObj[9]=this->ui->spinBox_valveActionConfirm->value();
    dataToTcpCommObj[10]=this->ui->spinBox_valveEnable->value();//enable/disable
    dataToTcpCommObj[11]=this->ui->comboBox_valveType->currentIndex();//valve type
    dataToTcpCommObj[12]=0x00;//reserve
    dataToTcpCommObj[13]=0x00;//reserve
    //emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    //query valve  config
    dataToTcpCommObj[14]=0x00;//length high byte
    dataToTcpCommObj[15]=0x06;//length low byte
    dataToTcpCommObj[16]=0x00;//commandNO high byte
    dataToTcpCommObj[17]=0x6C;//commandNO low byte,108
    dataToTcpCommObj[18]=0x00;//reserve byte
    dataToTcpCommObj[19]=0x00;//reserve byte
    if(this->tcpConnectionStatus_send)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_QueryValveConfig_clicked()
{
    if(this->tcpConnectionStatus_send)
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getValveConfigFromPLC());
}

void MainWindow::on_btn_undoValveConfig_clicked()
{

    this->ui->spinBox_valveActionConfirm->setValue(this->ui->CV_valveActionConfirm->value());
    this->ui->spinBox_valveStartStep->setValue(this->ui->CV_valveStartStep->value());
    this->ui->spinBox_valveEndStep->setValue(this->ui->CV_valveEndStep->value());
    this->ui->spinBox_valveEnable->setValue(this->ui->CV_valveEnableBypass->value());
}
#pragma pack(2)
void MainWindow::on_btn_setPointPara_clicked()//include point config and us/para
{
   qDebug()<<"set point us para btn pressed";
    if(true)
    {
        wp1=new weldPoint;
        wp1->pointNO=this->ui->spinBox_PointNO_pointPara->value();
        wp1->thrusterPressure_down=this->ui->spinBox_pointPara_downPressure->value()*(3868-228)/500+228;
        //wp1->thrusterPressure_up=this->ui->spinBox_pointPara_upPressure->value();
        wp1->enable=this->ui->spinBox_weldPointEnable->value();
        wp1->GenNO=this->tempTooling_editting->plcToolingInfo.weldPoint_List
                [this->ui->spinBox_PointNO_pointPara->value()].GenNO;
        wp1->stepNO=this->ui->spinBox_stepNO_WPC->value();
        wp1->ThrusterNO=this->ui->spinBox_thrusterNOWPC->value();
        wp1->ultrasonicPara.AfterBurstdelay0to999=this->ui->spinBox_pointPara_afterburstDelay->value();
        wp1->ultrasonicPara.Afterburst_Duration0t999=this->ui->spinBox_pointPara_afterburst_duration->value();
        wp1->ultrasonicPara.Amplitude20to100=this->ui->spinBox_pointPara_amp->value();
        wp1->ultrasonicPara.BadPart_MaxEnergy_Limits=this->ui->spinBox_energyUpperLimit_BAD->value();
        wp1->ultrasonicPara.BadPart_MaxPower_Limits=this->ui->spinBox_powerUpperLimit_BAD->value();
        wp1->ultrasonicPara.BadPart_MaxTime_limits=this->ui->spinBox_timeUpperLimit_BAD->value();
        wp1->ultrasonicPara.BadPart_MinEnergy_Limits=this->ui->spinBox_energyLowerLimit_BAD->value();
        wp1->ultrasonicPara.BadPart_MinPower_Limits=this->ui->spinBox_powerLowerLimit_BAD->value();
        wp1->ultrasonicPara.Badpart_MinTime_Limits=this->ui->spinBox_timeLowerLimit_BAD->value();
        wp1->ultrasonicPara.B0_CommandByte.byteChar=0;
        wp1->ultrasonicPara.B1_iQLinQCommStatus.byteChar=0;
        wp1->ultrasonicPara.B21_EnableSuspectBadByte.bits.b1=this->ui->checkBox_LimitsEnable_BAD->isChecked();
        wp1->ultrasonicPara.B21_EnableSuspectBadByte.bits.b0=this->ui->checkBox_LimitsEnable_SUSPECT->isChecked();
        wp1->ultrasonicPara.B64_enableTrigByPower.byteChar=this->ui->spinBox_pointPara_trigByPower->value()?1:0;

        wp1->ultrasonicPara.Energy0to65535=this->ui->spinBox_pointPara_weldEnergy->value();
        wp1->ultrasonicPara.Free_Run_Frequency=this->ui->spinbox_pointPara_freeRunFreq->value();
        wp1->ultrasonicPara.B47_freqLockHold.byteChar=0;
        wp1->ultrasonicPara.Freq_Lower_Limit=this->ui->spinbox_pointPara_freq_l_limit->value();
        wp1->ultrasonicPara.Freq_Upper_Limit=this->ui->spinbox_pointPara_freq_u_limit->value();
        wp1->ultrasonicPara.Hold_TIme0to30000=this->ui->spinBox_pointPara_holdTime->value();

        wp1->ultrasonicPara.Neg20_to_Pos20=0;

        wp1->ultrasonicPara.Peak_Power=this->ui->spinBox_pointPara_peakPower->value();


        wp1->ultrasonicPara.Ramp_Down_TIme0to250=this->ui->spinBox_pointPara_rampdownTime->value();
        wp1->ultrasonicPara.Ramp_up_Time20to1250=this->ui->spinBox_pointPara_rampupTime->value();
        wp1->ultrasonicPara.suspect_MinTime_Limits=this->ui->spinBox_timeLowerLimit_Suspect->value();
        wp1->ultrasonicPara.Suspect_MinEnergy_Limits=this->ui->spinBox_energyLowerLimit_Suspect->value();
        wp1->ultrasonicPara.Suspect_MinPower_Limits=this->ui->spinBox_powerLowerLimit_Suspect->value();
        wp1->ultrasonicPara.Suspect_MaxEnergy_Limits=this->ui->spinBox_energyUpperLimit_Suspect_2->value();
        wp1->ultrasonicPara.Suspect_MaxPower_Limits=this->ui->spinBox_powerUpperLimit_Suspect_2->value();
        wp1->ultrasonicPara.Suspect_MaxTime_Limits=this->ui->spinBox_timeUpperLimit_Suspect->value();
        wp1->ultrasonicPara.B2_trigByte_ProcessControl.byteChar=0;
        wp1->ultrasonicPara.B20_trigByte_ProcLimits.byteChar=0;
        wp1->ultrasonicPara.B63_trigByteTrigBypower.byteChar=0;
        wp1->ultrasonicPara.B46_trigByteHW_Setup.byteChar=0;

        wp1->ultrasonicPara.Trigger_Amplitude=this->ui->spinBox_pointPara_trigAmp->value();
        wp1->ultrasonicPara.Trigger_Power=this->ui->spinBox_pointPara_trigPower->value();
        wp1->ultrasonicPara.Trigger_Timeout=this->ui->spinBox_pointPara_trigTimeout->value();
        wp1->ultrasonicPara.Weld_Time0to30000=this->ui->spinBox_pointPara_weldTime->value();
        //swap high and low bytes to adapt Siemens PLC's store method
        wp1->ultrasonicPara.Ramp_up_Time20to1250=BigLittleSwap16(wp1->ultrasonicPara.Ramp_up_Time20to1250);//B4-B5
        wp1->ultrasonicPara.Ramp_Down_TIme0to250=BigLittleSwap16(wp1->ultrasonicPara.Ramp_Down_TIme0to250);//B6-B7
        wp1->ultrasonicPara.Weld_Time0to30000=BigLittleSwap16(wp1->ultrasonicPara.Weld_Time0to30000);//B8-B9
        wp1->ultrasonicPara.Peak_Power=BigLittleSwap16(wp1->ultrasonicPara.Peak_Power);//B10-B11
        wp1->ultrasonicPara.Energy0to65535=BigLittleSwap16(wp1->ultrasonicPara.Energy0to65535);//B12-B13
        wp1->ultrasonicPara.Hold_TIme0to30000=BigLittleSwap16(wp1->ultrasonicPara.Hold_TIme0to30000);//B14-B15
        wp1->ultrasonicPara.AfterBurstdelay0to999=BigLittleSwap16(wp1->ultrasonicPara.AfterBurstdelay0to999);//B16-B17
        wp1->ultrasonicPara.Afterburst_Duration0t999=BigLittleSwap16(wp1->ultrasonicPara.Afterburst_Duration0t999);//B18-B19
        wp1->ultrasonicPara.Suspect_MaxTime_Limits=BigLittleSwap16(wp1->ultrasonicPara.Suspect_MaxTime_Limits);//B22-B23
        wp1->ultrasonicPara.suspect_MinTime_Limits=BigLittleSwap16(wp1->ultrasonicPara.suspect_MinTime_Limits);//B24-B25
        wp1->ultrasonicPara.Suspect_MaxPower_Limits=BigLittleSwap16(wp1->ultrasonicPara.Suspect_MaxPower_Limits);//B26-B27
        wp1->ultrasonicPara.Suspect_MinPower_Limits=BigLittleSwap16(wp1->ultrasonicPara.Suspect_MinPower_Limits);//B28-B29
        wp1->ultrasonicPara.Suspect_MaxEnergy_Limits=BigLittleSwap16(wp1->ultrasonicPara.Suspect_MaxEnergy_Limits);//B30-B31
        wp1->ultrasonicPara.Suspect_MinEnergy_Limits=BigLittleSwap16(wp1->ultrasonicPara.Suspect_MinEnergy_Limits);//B32-B33
        wp1->ultrasonicPara.BadPart_MaxTime_limits=BigLittleSwap16(wp1->ultrasonicPara.BadPart_MaxTime_limits);//B34-B35
        wp1->ultrasonicPara.Badpart_MinTime_Limits=BigLittleSwap16(wp1->ultrasonicPara.Badpart_MinTime_Limits);//B36-B37
        wp1->ultrasonicPara.BadPart_MaxPower_Limits=BigLittleSwap16(wp1->ultrasonicPara.BadPart_MaxPower_Limits);//B38-B39
        wp1->ultrasonicPara.BadPart_MinPower_Limits=BigLittleSwap16(wp1->ultrasonicPara.BadPart_MinPower_Limits);//B40-B41
        wp1->ultrasonicPara.BadPart_MaxEnergy_Limits=BigLittleSwap16(wp1->ultrasonicPara.BadPart_MaxEnergy_Limits);//B42-B43
        wp1->ultrasonicPara.BadPart_MinEnergy_Limits=BigLittleSwap16(wp1->ultrasonicPara.BadPart_MinEnergy_Limits);//B44-B45
        wp1->ultrasonicPara.Free_Run_Frequency=BigLittleSwap32(wp1->ultrasonicPara.Free_Run_Frequency);//B50~B53
        wp1->ultrasonicPara.Freq_Upper_Limit=BigLittleSwap32(wp1->ultrasonicPara.Freq_Upper_Limit);//B54~B57
        wp1->ultrasonicPara.Freq_Lower_Limit=BigLittleSwap32(wp1->ultrasonicPara.Freq_Lower_Limit);//B58~B61
        wp1->ultrasonicPara.Trigger_Power=BigLittleSwap16(wp1->ultrasonicPara.Trigger_Power);//B66-B67
        wp1->ultrasonicPara.Trigger_Timeout=BigLittleSwap16(wp1->ultrasonicPara.Trigger_Timeout);//B68-B69
    }

    char *ptrCharData=(char*)wp1;
    //ptrCharData= (char*)wp1;
    QByteArray ar1(ptrCharData,sizeof(weldPoint));
    //ar1.append(2,(char)0x00);
    //QByteArray ar1;
    //memcpy(wp1,ar1,sizeof(weldPoint));
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x56;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x75;//commandNO low byte,117
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj.append(ar1);
    //dataToTcpCommObj.append(80,(char)0x00);
    //if(this->tcpConnectionStatus_send)
        //emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    qDebug()<<"data from window to tcpComm signal emitted";
    //query weld point  config
    QByteArray ar2;
    ar2[0]=0x00;//length high byte
    ar2[1]=0x0A;//length low byte
    ar2[2]=0x00;//commandNO high byte
    ar2[3]=0x6B;//commandNO low byte,107
    ar2[4]=0x00;//reserve byte
    ar2[5]=0x00;//reserve byte
    ar2[6]=wp1->pointNO;
    ar2[7]=0x01;//point count,can query more than 1 point's data
    ar2[8]=0x00;//reserve byte
    ar2[9]=0x00;//reserve byte
    dataToTcpCommObj.append(ar2);
    if(this->tcpConnectionStatus_send)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    //Reset the pointer
    delete wp1;
    wp1=nullptr;
    ptrCharData=nullptr;
}
#pragma pack()

void MainWindow::on_btn_getPointPara_clicked()
{

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(
                this->tempTooling_editting->prepareCommand_getPointUSParaFromPLC(
                    this->ui->spinBox_PointNO_pointPara->value()));
    qDebug()<<"call tooling_prepareCommand_getPointUSParaFromPLC() returned";
}


void MainWindow::on_btn_setServoPara_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x18;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x77;//commandNO low byte,119
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte

    dWordBytes dw1;
    dw1.DWordVar=this->ui->stationPara_HighSpeed->value()*5;
    dataToTcpCommObj[6]=dw1.bytesVar.B0;
    dataToTcpCommObj[7]=dw1.bytesVar.B1;
    dataToTcpCommObj[8]=dw1.bytesVar.B2;
    dataToTcpCommObj[9]=dw1.bytesVar.B3;
    dw1.DWordVar=this->ui->stationPara_MediumSpeed->value()*5;
    dataToTcpCommObj[10]=dw1.bytesVar.B0;
    dataToTcpCommObj[11]=dw1.bytesVar.B1;
    dataToTcpCommObj[12]=dw1.bytesVar.B2;
    dataToTcpCommObj[13]=dw1.bytesVar.B3;
    dw1.DWordVar=this->ui->stationPara_LowSpeed->value()*5;
    dataToTcpCommObj[14]=dw1.bytesVar.B0;
    dataToTcpCommObj[15]=dw1.bytesVar.B1;
    dataToTcpCommObj[16]=dw1.bytesVar.B2;
    dataToTcpCommObj[17]=dw1.bytesVar.B3;
    //dw1.DWordVar=this->ui->stationPara_DefaultSpeed->value();
    //dataToTcpCommObj[18]=dw1.bytesVar.B0;
    //dataToTcpCommObj[19]=dw1.bytesVar.B1;
    //dataToTcpCommObj[20]=dw1.bytesVar.B2;
    //dataToTcpCommObj[21]=dw1.bytesVar.B3;
    dataToTcpCommObj[22]=this->ui->servoPara_CWCCW->currentIndex();
    //qDebug()<<"homming dir, current index"<<dataToTcpCommObj[22];
    dataToTcpCommObj[23]=0x00;
    //set servo para
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    //query station&&servo para
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getstationParaFromPLC());
}

void MainWindow::on_btn_setStationPara_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x18;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x71;//commandNO low byte,113
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dWordBytes dw1;
    dataToTcpCommObj[6]=this->ui->stationPara_stationNO->value();
    dataToTcpCommObj[7]=this->ui->stationPara_speedType->currentIndex();
    dw1.DWordVar=this->ui->stationPara_setPoint->value();
    dataToTcpCommObj[8]=dw1.bytesVar.B0;
    dataToTcpCommObj[9]=dw1.bytesVar.B1;
    dataToTcpCommObj[10]=dw1.bytesVar.B2;
    dataToTcpCommObj[11]=dw1.bytesVar.B3;
    dw1.DWordVar=this->ui->stationPara_upperLimit->value();
    dataToTcpCommObj[12]=dw1.bytesVar.B0;
    dataToTcpCommObj[13]=dw1.bytesVar.B1;
    dataToTcpCommObj[14]=dw1.bytesVar.B2;
    dataToTcpCommObj[15]=dw1.bytesVar.B3;
    dw1.DWordVar=this->ui->stationPara_lowerLimit->value();
    dataToTcpCommObj[16]=dw1.bytesVar.B0;
    dataToTcpCommObj[17]=dw1.bytesVar.B1;
    dataToTcpCommObj[18]=dw1.bytesVar.B2;
    dataToTcpCommObj[19]=dw1.bytesVar.B3;

    dataToTcpCommObj[20]=0x00;
    dataToTcpCommObj[21]=0x00;
    dataToTcpCommObj[22]=0x00;
    dataToTcpCommObj[23]=0x00;

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
    {
        //set
       emit this->sendDataToTCPCommObj(dataToTcpCommObj);

       //get to check
       emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getstationParaFromPLC());

    }
}

void MainWindow::on_btn_QuerystationPara_clicked()
{
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getstationParaFromPLC());
}

void MainWindow::on_actionTooling_Config_triggered()
{
    this->changePage(0);
    //this->ui->stackedWidget_mainProgram->setCurrentIndex(0);
}
void MainWindow::on_actionautoRun_triggered()
{
    this->changePage(4);
    //this->ui->stackedWidget_mainProgram->setCurrentIndex(4);
}

void MainWindow::on_pushButton_weldByManual_checkAlarm_clicked()
{
    this->changePage(3);
    //this->ui->stackedWidget_mainProgram->setCurrentIndex(3);
}
void MainWindow::on_btn_alarmPage_clicked()
{
    this->changePage(3);
}
void MainWindow::on_pushButton_leaveAlarmPage_clicked()
{
    this->changePage(255);
    //this->ui->stackedWidget_mainProgram->setCurrentIndex(1);
}
void MainWindow::on_pushButton_Advance_clicked()
{
    //changePage(6);
    this->ui->stackedWidget_mainProgram->setCurrentIndex(6);
}
void MainWindow::on_btn_IOTable_clicked()
{
    this->changePage(2);
}

void MainWindow::on_btn_IO_Table_Quit_clicked()
{
    changePage(255);
}
void MainWindow::on_btn_undoStationPara_clicked()
{
    this->ui->stationPara_setPoint->setValue(this->ui->CV_stationPara_setPoint->value());
    this->ui->stationPara_lowerLimit->setValue(this->ui->CV_stationPara_lowerLimit->value());
    this->ui->stationPara_upperLimit->setValue(this->ui->CV_stationPara_upperLimit->value());
    this->ui->stationPara_speedType->setCurrentIndex(this->ui->CV_stationPara_speedType->currentIndex());

}

void MainWindow::on_btn_undoServoSetting_clicked()
{
    this->ui->stationPara_HighSpeed->setValue(this->ui->CV_stationPara_HighSpeed->value());
    this->ui->stationPara_MediumSpeed->setValue(this->ui->CV_stationPara_MediumSpeed->value());
    this->ui->stationPara_LowSpeed->setValue(this->ui->CV_stationPara_lowSpeed->value());
    this->ui->stationPara_DefaultSpeed->setValue(this->ui->CV_stationPara_defaultSpeed->value());
    this->ui->servoPara_CWCCW->setCurrentIndex(this->ui->CV_servoPara_CWCCW->currentIndex());
}

void MainWindow::on_WeldByManual_PointNO_valueChanged(int arg1)
{
    //this->ui->weldByManual_Down->setEnabled(false);
    //this->ui->weldByManual_runWeld->setEnabled(false);
    //CMD109, prepare info for weld by manual
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x08;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x6D;//commandNO low byte,109
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=arg1;//PointNO
    dataToTcpCommObj[7]=0x00;//reserved
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_weldByManual_GOTO_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x79;//commandNO low byte,121
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x0F;//15 MDI
    dataToTcpCommObj[7]=this->ui->weldByManual_targetStationNO->value();//MDI station NO
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_weldByManual_Down_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0E;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x78;//commandNO low byte,120
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->WeldByManual_PointNO->value();//PointNO
    dataToTcpCommObj[7]=0x01;//coilA
    dataToTcpCommObj[8]=0x00;//coilB
    dataToTcpCommObj[9]=0x00;//TEST
    dataToTcpCommObj[10]=0x00;//FIRE
    dataToTcpCommObj[11]=this->ui->weldByManual_Batch->isChecked()?1:0;//batch flag
    dataToTcpCommObj[12]=this->ui->weldByManual_stepNO->value();//reserve
    dataToTcpCommObj[13]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}


void MainWindow::on_weldByManual_runWeld_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0E;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x78;//commandNO low byte,120
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->WeldByManual_PointNO->value();//PointNO
    dataToTcpCommObj[7]=0x01;//coilA
    dataToTcpCommObj[8]=0x01;//coilB
    dataToTcpCommObj[9]=0x00;//TEST
    dataToTcpCommObj[10]=0x01;//FIRE
    dataToTcpCommObj[11]=this->ui->weldByManual_Batch->isChecked()?1:0;//batch flag
    dataToTcpCommObj[12]=this->ui->weldByManual_stepNO->value();//reserve
    dataToTcpCommObj[13]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);

}
void MainWindow::on_Clamper_Extend_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7E;//commandNO low byte,126
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->WeldByManual_valveNO->value();//valveNO
    dataToTcpCommObj[7]=0x01;//coilA
    dataToTcpCommObj[8]=0x00;//coilB
    dataToTcpCommObj[9]=0x00;//reserve
    dataToTcpCommObj[10]=0x00;//reserve
    dataToTcpCommObj[11]=0x00;//reserve

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_Clamper_Free_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7E;//commandNO low byte,126
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->WeldByManual_valveNO->value();//valveNO
    dataToTcpCommObj[7]=0x00;//coilA
    dataToTcpCommObj[8]=0x00;//coilB
    dataToTcpCommObj[9]=0x00;//reserve
    dataToTcpCommObj[10]=0x00;//reserve
    dataToTcpCommObj[11]=0x00;//reserve

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_Clamper_Retract_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7E;//commandNO low byte,126
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->WeldByManual_valveNO->value();//valveNO
    dataToTcpCommObj[7]=0x00;//coilA
    dataToTcpCommObj[8]=0x01;//coilB
    dataToTcpCommObj[9]=0x00;//reserve
    dataToTcpCommObj[10]=0x00;//reserve
    dataToTcpCommObj[11]=0x00;//reserve

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_pointPara_TEST_toggled(bool checked)
{
    if(checked)
    {
        qDebug()<<"TEST toggled to true";
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0E;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x78;//commandNO low byte,120
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=this->ui->spinBox_PointNO_pointPara->value();//PointNO
        dataToTcpCommObj[7]=0x01;//DownUp CoilA
        dataToTcpCommObj[8]=0x01;//DownUp CoilB, A==B==1，then PLC ignore them
        dataToTcpCommObj[9]=0x01;//TEST
        dataToTcpCommObj[10]=0x00;//Fire
        dataToTcpCommObj[11]=0x00;//Batch
        dataToTcpCommObj[12]=0x00;//reserve
        dataToTcpCommObj[13]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
    else
    {
        qDebug()<<"TEST toggled to false";
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0E;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x78;//commandNO low byte,120
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=this->ui->spinBox_PointNO_pointPara->value();//PointNO
        dataToTcpCommObj[7]=0x01;//DownUp CoilA
        dataToTcpCommObj[8]=0x01;//DownUp CoilB, A==B==1，then PLC ignore them
        dataToTcpCommObj[9]=0x00;//TEST
        dataToTcpCommObj[10]=0x00;//Fire
        dataToTcpCommObj[11]=0x00;//Batch
        dataToTcpCommObj[12]=0x00;//reserve
        dataToTcpCommObj[13]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
}


void MainWindow::on_servo_enable_btn_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x79;//commandNO low byte,121
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x0B;//11 enable servo
    dataToTcpCommObj[7]=0x00;//MDI station NO
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_servo_disable_btn_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x79;//commandNO low byte,121
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x0C;//12 disable servo
    dataToTcpCommObj[7]=0x00;//MDI station NO
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_servo_Jog2_btn_toggled(bool checked)
{
    if(checked)
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x79;//commandNO low byte,121
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x0E;//14 JOG2
        dataToTcpCommObj[7]=0x00;//MDI station NO
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
    else
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x79;//commandNO low byte,121
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//stop servo while keep enable
        dataToTcpCommObj[7]=0x00;//MDI station NO
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
         emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }

}

void MainWindow::on_servoACKbtn_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x79;//commandNO low byte,121
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x10;//servo fault ACK
    dataToTcpCommObj[7]=0x00;//MDI station NO
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_servo_Jog1_btn_toggled(bool checked)
{
    if(checked)
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x79;//commandNO low byte,121
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x0D;//13 JOG1
        dataToTcpCommObj[7]=0x00;//MDI station NO
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
    else
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x79;//commandNO low byte,121
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//stop servo while keep enable
        dataToTcpCommObj[7]=0x00;//MDI station NO
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
         emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
}

void MainWindow::on_servo_home_btn_toggled(bool checked)
{
    if(checked)
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x79;//commandNO low byte,121
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x11;//17 home servo
        dataToTcpCommObj[7]=0x00;//MDI station NO
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
    else
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x79;//commandNO low byte,121
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//stop servo while keep enable
        dataToTcpCommObj[7]=0x00;//MDI station NO
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
}


void MainWindow::on_stationConfig_servo_GOTO_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x79;//commandNO low byte,121
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x0F;//15 MDI
    dataToTcpCommObj[7]=this->ui->stationPara_stationNO->value();//MDI station NO
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_setGenEnable_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x08;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x6F;//commandNO low byte,111
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    bytebits bb1;
    bb1.byteChar=0;
    bb1.bits.b1=this->ui->checkBox_GenEnable_1->isChecked()?true:false;
    bb1.bits.b2=this->ui->checkBox_GenEnable_2->isChecked()?true:false;
    bb1.bits.b3=this->ui->checkBox_GenEnable_3->isChecked()?true:false;
    bb1.bits.b4=this->ui->checkBox_GenEnable_3->isChecked()?true:false;
    dataToTcpCommObj[6]=bb1.byteChar;//gen enable/disable status
    dataToTcpCommObj[7]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getGenEnableStatusFromPLC());
}

void MainWindow::on_btn_getGenEnableStatus_clicked()
{
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getGenEnableStatusFromPLC());
}

void MainWindow::on_btn_undoGenEnable_clicked()
{
    this->ui->checkBox_GenEnable_1->setChecked(this->ui->CV_GenEnable_1->isChecked());
    this->ui->checkBox_GenEnable_2->setChecked(this->ui->CV_GenEnable_2->isChecked());
    this->ui->checkBox_GenEnable_3->setChecked(this->ui->CV_GenEnable_3->isChecked());
    this->ui->checkBox_GenEnable_4->setChecked(this->ui->CV_GenEnable_4->isChecked());
}


void MainWindow::on_toolID_Editting_valueChanged(int arg1)
{
    this->toolID_editing=arg1;
    this->tempTooling_editting->plcToolingInfo.toolingNO=arg1;
    //load config file if exist
    QString fileName="toolingConfig/toolingConfig_"+QString::number(this->ui->toolID_Editting->value())+".dc";
    if(QFile::exists(fileName))
    {
        qDebug()<<"found config file,will load it ";
        //load config file
        if(this->tempTooling_editting->loadFromDisk(fileName))
        {
          qDebug()<<"load done,file name:"<<fileName;
          this->updateDisplay_All_EdittingValue();
          this->ui->toolName_lineEdit->setText(this->tempTooling_editting->toolingName);
          if(!this->tempTooling_editting->toolingImageSource.isEmpty())
          {
              this->setPixmapForLabel(this->ui->toolingPicLabel,this->tempTooling_editting->toolingImageSource);
          }
          else
          {
              this->setPixmapForLabel(this->ui->toolingPicLabel,":/img/dukaneLogo.PNG");

          }

        }

    }
    else
    {
       qDebug()<<"can not find config file,need create it first";
    }

    //set PLC components and value visible or not
    if((!this->tcpConnectionStatus_receive)||(!this->tcpConnectionStatus_send)||this->toolID_editing!=this->toolID_PLC)
    {

        this->setPLCValueVisible(false);

    }
    else
    {

        this->setPLCValueVisible(true);
        qDebug()<<"uploading whole setting para from PLC";
        //here upload setting will only update CV values&&tooling_Current,while, if
        // press "upload from PLC" button, temp_tooling_Editting&&Editting value will also be overwritten
        this->startDownloadUploadWholeSettings(2);

    }

}



void MainWindow::on_toolName_lineEdit_textEdited(const QString &arg1)
{
    this->tempTooling_editting->toolingName=arg1;
}

void MainWindow::on_btn_selectToolingImage_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "./",
                                                    tr("Images (*.png *.xpm *.jpg)"));
    if(!fileName.isEmpty())
    {
        //this->ui->toolImageUrl->setText(fileName);
        this->tempTooling_editting->toolingImageSource=fileName;
        this->setPixmapForLabel(this->ui->toolingPicLabel,fileName);

    }


}
void MainWindow::on_pushButton_removeImage_clicked()
{
    this->tempTooling_editting->toolingImageSource=":/img/dukaneLogo.PNG";
    this->setPixmapForLabel(this->ui->toolingPicLabel,":/img/dukaneLogo.PNG");

}

void MainWindow::on_btn_saveToDisk_clicked()
{

    QString fileName="toolingConfig/toolingConfig_"+QString::number(this->ui->toolID_Editting->value())+".dc";
    this->tempTooling_editting->saveToDisk(fileName);
    this->tempTooling_editting->saveToDisk("toolingConfig/currentTooling.dc");
}

void MainWindow::on_toolID_fromPLC_valueChanged(int arg1)
{
    if(this->toolID_PLC!=arg1)
        this->toolID_PLC=arg1;
    if(this->toolID_PLC==this->toolID_editing&&(this->tcpConnectionStatus_receive)&&(this->tcpConnectionStatus_send))
        this->setPLCValueVisible(true);
}

void MainWindow::on_checkBox_GenEnable_1_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.generator_enable[1]=arg1?true:false;

}

void MainWindow::on_checkBox_GenEnable_2_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.generator_enable[2]=arg1?true:false;
}

void MainWindow::on_checkBox_GenEnable_3_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.generator_enable[3]=arg1?true:false;
}

void MainWindow::on_checkBox_GenEnable_4_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.generator_enable[4]=arg1?true:false;
}

void MainWindow::on_spinBox_thrusterEnable_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.thruster_List[this->ui->spinBox_thrusterNO->value()].enable=arg1;

}

void MainWindow::on_spinBox_GenNO_valueChanged(int arg1)
{
   this->tempTooling_editting->plcToolingInfo.thruster_List[this->ui->spinBox_thrusterNO->value()].GenNO=arg1;

}

void MainWindow::on_spinBox_channel_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.thruster_List[this->ui->spinBox_thrusterNO->value()].ChannelNO=arg1;

}

void MainWindow::on_spinBox_weldPointEnable_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].enable=arg1;
}

void MainWindow::on_spinBox_thrusterNOWPC_valueChanged(int arg1)
{
   this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ThrusterNO=arg1;
}

void MainWindow::on_spinBox_stepNO_WPC_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].stepNO=arg1;
}

void MainWindow::on_spinBox_pointPara_amp_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Amplitude20to100=arg1;
}

void MainWindow::on_spinBox_pointPara_trigAmp_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Trigger_Amplitude=arg1;
}

void MainWindow::on_spinBox_pointPara_trigPower_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Trigger_Power=arg1;
}

void MainWindow::on_spinBox_pointPara_trigTimeout_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Trigger_Timeout=arg1;
}

void MainWindow::on_spinBox_pointPara_weldTime_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Weld_Time0to30000=arg1;
}

void MainWindow::on_spinBox_pointPara_weldEnergy_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Energy0to65535=arg1;
}

void MainWindow::on_spinBox_pointPara_peakPower_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Peak_Power=arg1;
}

void MainWindow::on_spinBox_pointPara_holdTime_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Hold_TIme0to30000=arg1;
}

void MainWindow::on_spinBox_pointPara_trigByPower_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.B64_enableTrigByPower.byteChar=arg1;
}

void MainWindow::on_spinbox_pointPara_freeRunFreq_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Free_Run_Frequency=arg1;
}

void MainWindow::on_spinbox_pointPara_freq_u_limit_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Freq_Upper_Limit=arg1;
}

void MainWindow::on_spinbox_pointPara_freq_l_limit_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Freq_Lower_Limit=arg1;
}

void MainWindow::on_spinBox_pointPara_rampupTime_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Ramp_up_Time20to1250=arg1;
}

void MainWindow::on_spinBox_pointPara_rampdownTime_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Ramp_Down_TIme0to250=arg1;
}

void MainWindow::on_spinBox_pointPara_afterburst_duration_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Afterburst_Duration0t999=arg1;
}

void MainWindow::on_spinBox_pointPara_afterburstDelay_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.AfterBurstdelay0to999=arg1;
}

void MainWindow::on_spinBox_pointPara_downPressure_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].thrusterPressure_down=arg1;
}

void MainWindow::on_spinBox_timeLowerLimit_BAD_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Badpart_MinTime_Limits=arg1;
}

void MainWindow::on_spinBox_powerLowerLimit_BAD_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.BadPart_MinPower_Limits=arg1;
}

void MainWindow::on_spinBox_energyLowerLimit_BAD_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.BadPart_MinEnergy_Limits=arg1;
}

void MainWindow::on_spinBox_timeLowerLimit_Suspect_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.suspect_MinTime_Limits=arg1;
}

void MainWindow::on_spinBox_powerLowerLimit_Suspect_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Suspect_MinPower_Limits=arg1;
}

void MainWindow::on_spinBox_energyLowerLimit_Suspect_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Suspect_MinEnergy_Limits=arg1;
}

void MainWindow::on_spinBox_timeUpperLimit_Suspect_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Suspect_MaxTime_Limits=arg1;
}

void MainWindow::on_spinBox_powerUpperLimit_Suspect_2_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Suspect_MaxPower_Limits=arg1;
}

void MainWindow::on_spinBox_energyUpperLimit_Suspect_2_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.Suspect_MaxEnergy_Limits=arg1;
}

void MainWindow::on_spinBox_timeUpperLimit_BAD_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.BadPart_MaxTime_limits=arg1;
}

void MainWindow::on_spinBox_powerUpperLimit_BAD_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.BadPart_MaxPower_Limits=arg1;
}

void MainWindow::on_spinBox_energyUpperLimit_BAD_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.BadPart_MaxEnergy_Limits=arg1;
}

void MainWindow::on_checkBox_LimitsEnable_BAD_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.B21_EnableSuspectBadByte.bits.b1=arg1?true:false;
}

void MainWindow::on_checkBox_LimitsEnable_SUSPECT_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.B21_EnableSuspectBadByte.bits.b0=arg1?true:false;
}

void MainWindow::on_stationPara_upperLimit_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.station_List[this->ui->stationPara_stationNO->value()].pos_upperLimit=arg1;
}

void MainWindow::on_stationPara_lowerLimit_valueChanged(int arg1)
{
   this->tempTooling_editting->plcToolingInfo.station_List[this->ui->stationPara_stationNO->value()].pos_lowerLimit=arg1;
}

void MainWindow::on_stationPara_setPoint_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.station_List[this->ui->stationPara_stationNO->value()].pos_setPoint=arg1;
}

void MainWindow::on_stationPara_speedType_currentIndexChanged(int index)
{
    this->tempTooling_editting->plcToolingInfo.station_List[this->ui->stationPara_stationNO->value()].speedType=index;
}

void MainWindow::on_stationPara_HighSpeed_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.servoSpeed_high=arg1;
}

void MainWindow::on_stationPara_MediumSpeed_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.servoSpeed_medium=arg1;
}

void MainWindow::on_stationPara_LowSpeed_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.servoSpeed_low=arg1;
}

void MainWindow::on_servoPara_CWCCW_currentIndexChanged(int index)
{
    this->tempTooling_editting->plcToolingInfo.servoHomingDir=index;
}

void MainWindow::on_spinBox_stationNO_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.stepStationConnection[this->ui->spinBox_stepNO->value()]=arg1;
}

void MainWindow::on_PointName_display_textEdited(const QString &arg1)
{
    this->tempTooling_editting->pointNameMapping[this->ui->spinBox_PointNO_pointPara->value()]=arg1;
}

void MainWindow::on_comboBox_barcode_assignment_currentIndexChanged(const QString &arg1)
{
    this->tempTooling_editting->pointBarcodeMapping[this->ui->spinBox_PointNO_pointPara->value()]=arg1;
}
void MainWindow::on_spinBox_PointNO_pointPara_valueChanged(int arg1)
{

    this->ui->PointName_display->setText(this->tempTooling_editting->pointNameMapping[arg1]);
    //comboBox_barcode_assignment
    this->ui->comboBox_barcode_assignment->setCurrentText(this->tempTooling_editting->pointBarcodeMapping[arg1]);
    //this->tempTooling_editting->plcToolingInfo.weldPoint_List[arg1].pointNO=arg1;
    this->updateWeldPoitDisplay_Editting(this->tempTooling_editting->plcToolingInfo.weldPoint_List[arg1]);
    qDebug()<<tr("pointNO:%1,tooID_Editting:%2,toolID_PLC:%3").arg(arg1).arg(this->toolID_editing).arg(this->toolID_PLC);
    if(toolID_PLC==toolID_editing && toolID_PLC>0)
    {
        if(this->tcpConnectionStatus_receive&&this->tcpConnectionStatus_send)
        emit this->sendDataToTCPCommObj(
                    this->tempTooling_editting->prepareCommand_getPointUSParaFromPLC(
                        this->ui->spinBox_PointNO_pointPara->value()));


    }
}

void MainWindow::on_spinBox_thrusterNO_valueChanged(int arg1)
{
    //this->tempTooling_editting->plcToolingInfo.thruster_List[arg1].thrusterNO=arg1;
    this->ui->spinBox_thrusterEnable->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[arg1].enable);
    this->ui->spinBox_GenNO->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[arg1].GenNO);
    this->ui->spinBox_channel->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[arg1].ChannelNO);
    if(this->toolID_PLC==this->tempTooling_editting->plcToolingInfo.toolingNO && this->toolID_PLC>0)
    {
        //this->ui->CV_thrusterEnable->setValue(this->tooling_current->plcToolingInfo.thruster_List[arg1].enable?1:0);
        //this->ui->CV_GenNO->setValue(this->tooling_current->plcToolingInfo.thruster_List[arg1].GenNO);
        //this->ui->CV_Channel->setValue(this->tooling_current->plcToolingInfo.thruster_List[arg1].ChannelNO);
        if(this->tcpConnectionStatus_receive&&this->tcpConnectionStatus_send)
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getThrusterConfigFromPLC());
    }
}
void MainWindow::on_stationPara_stationNO_valueChanged(int arg1)
{
  this->ui->stationPara_upperLimit->setValue(this->tempTooling_editting->plcToolingInfo.station_List[arg1].pos_upperLimit);
  this->ui->stationPara_lowerLimit->setValue(this->tempTooling_editting->plcToolingInfo.station_List[arg1].pos_lowerLimit);
  this->ui->stationPara_setPoint->setValue(this->tempTooling_editting->plcToolingInfo.station_List[arg1].pos_setPoint);
  this->ui->stationPara_speedType->setCurrentIndex(this->tempTooling_editting->plcToolingInfo.station_List[arg1].speedType);
  this->ui->stationPara_HighSpeed->setValue(this->tempTooling_editting->plcToolingInfo.servoSpeed_high);
  this->ui->stationPara_MediumSpeed->setValue(this->tempTooling_editting->plcToolingInfo.servoSpeed_medium);
  this->ui->stationPara_LowSpeed->setValue(this->tempTooling_editting->plcToolingInfo.servoSpeed_low);
  this->ui->servoPara_CWCCW->setCurrentIndex(this->tempTooling_editting->plcToolingInfo.servoHomingDir?1:0);
    if(this->toolID_editing==this->toolID_PLC&&toolID_PLC>0)
    {
        if(this->tcpConnectionStatus_receive&&this->tcpConnectionStatus_send)
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getstationParaFromPLC());

    }
}
void MainWindow::on_spinBox_stepNO_valueChanged(int arg1)
{
    this->ui->spinBox_stationNO->setValue(this->tempTooling_editting->plcToolingInfo.stepStationConnection[arg1]);
    if(toolID_editing==toolID_PLC)
    {
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getStepStationConnectionFromPLC());
    }
}

void MainWindow::on_valveName_lineEdit_textEdited(const QString &arg1)
{
    this->tempTooling_editting->valveNameMapping[this->ui->spinBox_valveNO->value()]=arg1;
}

void MainWindow::on_spinBox_valveStartStep_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[this->ui->spinBox_valveNO->value()].startStep=arg1;
}



void MainWindow::on_spinBox_valveNO_valueChanged(int arg1)
{
    this->ui->valveName_lineEdit->setText(this->tempTooling_editting->valveNameMapping[arg1]);
    //this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].valveNO=arg1;
    if(arg1==1)
    {
       this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].valveType=0;
       this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].actionConfirm=0;
       this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].enable=1;
       this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].startStep=0;
       this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].stopStep=99;
       this->ui->spinBox_valveEnable->setEnabled(false);
        this->ui->comboBox_valveType->setEnabled(false);
        this->ui->spinBox_valveActionConfirm->setEnabled(false);
        this->ui->spinBox_valveStartStep->setEnabled(false);
        this->ui->spinBox_valveEndStep->setEnabled(false);
    }
    else if(arg1==2)
    {
        this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].valveType=1;
        this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].actionConfirm=0;
        this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].enable=1;
        this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].startStep=0;
        this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].stopStep=99;
        this->ui->spinBox_valveEnable->setEnabled(false);
         this->ui->comboBox_valveType->setEnabled(false);
         this->ui->spinBox_valveActionConfirm->setEnabled(false);
         this->ui->spinBox_valveStartStep->setEnabled(false);
         this->ui->spinBox_valveEndStep->setEnabled(false);
    }
    else if (arg1==3)
    {
        this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].valveType=2;
        if(this->parameterEditable)
        {
            this->ui->comboBox_valveType->setEnabled(false);
            this->ui->spinBox_valveActionConfirm->setEnabled(true);
            this->ui->spinBox_valveStartStep->setEnabled(true);
            this->ui->spinBox_valveEndStep->setEnabled(true);
            this->ui->spinBox_valveEnable->setEnabled(true);
        }

    }
    else
    {
        if(this->parameterEditable)
        {
            this->ui->comboBox_valveType->setEnabled(true);
            this->ui->spinBox_valveActionConfirm->setEnabled(true);
            this->ui->spinBox_valveStartStep->setEnabled(true);
            this->ui->spinBox_valveEndStep->setEnabled(true);
            this->ui->spinBox_valveEnable->setEnabled(true);
        }

    }
    this->ui->spinBox_valveEnable->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].enable);
    this->ui->comboBox_valveType->setCurrentIndex(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].valveType);
    this->ui->spinBox_valveActionConfirm->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].actionConfirm);
    this->ui->spinBox_valveStartStep->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].startStep);
    this->ui->spinBox_valveEndStep->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].stopStep);
    if(toolID_PLC==toolID_editing && toolID_PLC>0)
    {
        if(this->tcpConnectionStatus_receive&&this->tcpConnectionStatus_send)
        emit this->sendDataToTCPCommObj(
                    this->tempTooling_editting->prepareCommand_getValveConfigFromPLC());


    }
}

void MainWindow::on_spinBox_valveEnable_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[this->ui->spinBox_valveNO->value()].enable=arg1;
}

void MainWindow::on_comboBox_valveType_currentIndexChanged(int index)
{

    if((index<=2)&&this->ui->spinBox_valveNO->value()>3)//wrong setting, will force to correct it
    {
        this->ui->comboBox_valveType->setCurrentIndex(3);
        this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[this->ui->spinBox_valveNO->value()].valveType=3;
    }
    else
    this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[this->ui->spinBox_valveNO->value()].valveType=index;
}

void MainWindow::on_spinBox_valveActionConfirm_valueChanged(int arg1)
{
   this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[this->ui->spinBox_valveNO->value()].actionConfirm=arg1;
}

void MainWindow::on_spinBox_valveEndStep_valueChanged(int arg1)
{
  this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[this->ui->spinBox_valveNO->value()].stopStep=arg1;
}

void MainWindow::on_btn_setToAllPoints_clicked()
{
    this->wp1=&(this->tempTooling_editting->plcToolingInfo
            .weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()]);

    for(int i=0;i<17;i++)
    {
        if(i==this->ui->spinBox_PointNO_pointPara->value())
            continue;
        this->tempTooling_editting->plcToolingInfo.weldPoint_List[i].ultrasonicPara=wp1->ultrasonicPara;
        this->tempTooling_editting->plcToolingInfo.weldPoint_List[i].thrusterPressure_down=wp1->thrusterPressure_down;
        //this->tempTooling_editting->plcToolingInfo.weldPoint_List[i].thrusterPressure_up=wp1->thrusterPressure_up;
    }
}

void MainWindow::on_btn_uploadFromPLC_clicked()
{
    if(this->uploadingWholeSettingFromPLCInProcess)
        return;
    if(this->tcpConnectionStatus_receive&&this->tcpConnectionStatus_send)
    {
        this->uploadingWholeSettingFromPLCInProcess=true;
        this->startDownloadUploadWholeSettings(2);
        QTimer::singleShot(2000, this, &OnUploadWholeSettingsTimeout);
        //this->ui->btn_uploadFromPLC->setText("Uploading...");
    }


}
 void MainWindow::OnLogInTimeout()
 {
     this->logInStatus=false;
     this->parameterEditable=false;
     emit parameterEditableStausChanged(false);
 }
void MainWindow::OnUploadWholeSettingsTimeout()
{
  this->uploadingWholeSettingFromPLCInProcess=false;
  //this->ui->btn_uploadFromPLC->setText("UploadFromPLC");
}
void MainWindow::on_btn_downLoadToPLC_clicked()
{

    this->startDownloadUploadWholeSettings(1);
}
void MainWindow::startDownloadUploadWholeSettings(quint8 direction)
{
    //direction==1,download direction==2,upload
    this->ptrprogressDialog=new QProgressDialog;
    this->ptrprogressDialog->setModal(Qt::WindowModal);
    qDebug()<<"dialog initialized";
    if(direction==1)
    {
        qDebug()<<"start downloading";
        this->ptrprogressDialog->setWindowTitle(tr("downloading"));
        this->downloadWholeSettingArrayList=this->tempTooling_editting->prepareCommand_downloadWholeSettingToPLC();
        this->ptrprogressDialog->setRange(0,this->downloadWholeSettingArrayList.size());
        qDebug()<<tr("before downloading the first item of %1").arg(this->downloadWholeSettingArrayList.size());

        this->execDownloadUploadWholeSettings(0,1);
        qDebug()<<tr("after downloading the first item");

        if(this->ptrprogressDialog->wasCanceled())
        {
              QMessageBox::warning(NULL,tr("Caution"),tr("download failed"));
              return;
        }

    }
    else if(direction==2)
    {
        this->ptrprogressDialog->setWindowTitle(tr("uploading"));
        this->uploadWholeSettingArrayList=this->tempTooling_editting->prepareCommand_uploadWholeSettingFromPLC();

        this->ptrprogressDialog->setRange(0,this->uploadWholeSettingArrayList.size());
        this->execDownloadUploadWholeSettings(0,2);
        if(this->ptrprogressDialog->wasCanceled())
        {
              QMessageBox::warning(NULL,tr("Caution"),tr("upload failed"));
             return;
        }


    }
}
void MainWindow::execDownloadUploadWholeSettings(quint8 cmdIndex,quint8 direction)
{
    this->ptrprogressDialog->setValue(cmdIndex);
    if(direction==1)//download
    {
        if(cmdIndex<this->downloadWholeSettingArrayList.size())
        {
            if(this->tcpConnectionStatus_send )
            emit this->sendDataToTCPCommObj(this->downloadWholeSettingArrayList.at(cmdIndex));
        }
        else
        {
            this->downloadWholeSettingArrayList.clear();
            QTimer::singleShot(100, this, &OnDownloadWholeSettingsTimeout);
            this->ptrprogressDialog->close();
            this->ptrprogressDialog->deleteLater();
            this->ptrprogressDialog=nullptr;
        }
    }
    else if(direction==2)
    {
        if(cmdIndex<this->uploadWholeSettingArrayList.size())
        {
            if(this->tcpConnectionStatus_send )
            emit this->sendDataToTCPCommObj(this->uploadWholeSettingArrayList.at(cmdIndex));
        }
        else
        {
            this->uploadWholeSettingArrayList.clear();
            this->ptrprogressDialog->close();
            this->ptrprogressDialog->deleteLater();
            this->ptrprogressDialog=nullptr;
        }
    }

}
void MainWindow::OnDownloadWholeSettingsTimeout()
{
    //upload all parameters after downloading done
    this->startDownloadUploadWholeSettings(2);
}

void MainWindow::on_btn_PLC2Editting_WeldPoint_clicked()
{


    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()]
            =this->tooling_current->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()];
    this->updateWeldPoitDisplay_Editting
            (this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()]);
}

void MainWindow::on_pushButton_Calc_ItemID_clicked()
{
    quint32 calc_result=(this->ui->comboBox_PLC_Area->currentIndex()+(quint32)(0x81))*500*8192
            +this->ui->spinBox_ByteNO->value()*8+this->ui->spinBox_BitPosInByte->value();
    //this->bitPosInByte()+this->byteAddress()*8+DB_NO*8192+itemGroup_area*500*8192
    this->ui->spinBox_ItemID_result->setValue(calc_result);
}

void MainWindow::on_pushButton_registText_clicked()
{
    this->systemRegisteredTextList[this->ui->spinBox_textID_reg->value()]=this->ui->textEdit_text_reg->toPlainText();
    QList<quint32> keys=this->systemRegisteredTextList.keys();
    QString itemText;
    QString allRegistedText;
    for(int i=0;i<keys.size();i++)
    {
       itemText=tr("ID:%1,Text:%2 \n").arg(keys.at(i)).arg(this->systemRegisteredTextList[keys.at(i)]);
       allRegistedText.append(itemText);
    }
    this->ui->textEdit_sysRegistedText->setText(allRegistedText);
    QFile loadFile("sysRegText.txt");

     if (!loadFile.open(QIODevice::WriteOnly)) {
         qWarning("Couldn't open  file when try to load system Registed text");
         return;
     }
     else
     {
         qDebug()<<"file opened,file.size:"<<loadFile.size();
         QDataStream Out1(&loadFile);
         Out1<<QString("predefined text list")<<(this->systemRegisteredTextList);
         qDebug()<<"save to file execed, file.size()"<<loadFile.size();
         loadFile.close();
     }
    QString fileName;
     if(this->LanguageFlag==0)
    {
        //overwrite "sysRegText_CN.txt"
         fileName="sysRegText_CN.txt";

    }
    else if(this->LanguageFlag==1)
    {
        //overwrite "sysRegText_EN.txt"
        fileName="sysRegText_EN.txt";
    }
     QFile loadFile2(fileName);
     if(loadFile2.open(QIODevice::WriteOnly))
     {
         QDataStream Out1(&loadFile2);
         Out1<<QString("predefined text list")<<(this->systemRegisteredTextList);
         loadFile2.close();
     }
}

void MainWindow::on_spinBox_textID_reg_editingFinished()
{
    this->ui->textEdit_text_reg->setText(this->systemRegisteredTextList.value(this->ui->spinBox_textID_reg->value()));
}

void MainWindow::on_pushButton_registText_remove_clicked()
{
    this->systemRegisteredTextList.remove(this->ui->spinBox_textID_reg->value());
    QList<quint32> keys=this->systemRegisteredTextList.keys();
    QString itemText;
    QString allRegistedText;
    for(int i=0;i<keys.size();i++)
    {
       itemText=tr("ID:%1,Text:%2 \n").arg(keys.at(i)).arg(this->systemRegisteredTextList[keys.at(i)]);
       allRegistedText.append(itemText);
    }
    this->ui->textEdit_sysRegistedText->setText(allRegistedText);
    QFile loadFile("sysRegText.txt");

         if (!loadFile.open(QIODevice::WriteOnly)) {
             qWarning("Couldn't open  file when try to load system Registed text");
             return;
         }
         else
         {
             qDebug()<<"file opened,file.size:"<<loadFile.size();
             QDataStream Out1(&loadFile);
             Out1<<QString("predefined text list")<<(this->systemRegisteredTextList);
             qDebug()<<"save to file execed, file.size()"<<loadFile.size();
             loadFile.close();
         }

}

void MainWindow::on_checkBox_barcode_left_enable_stateChanged(int arg1)
{
    this->tempTooling_editting->leftBarcodeSettings.enable=arg1?true:false;
}

void MainWindow::on_checkBox_barcode_right_enable_stateChanged(int arg1)
{
    this->tempTooling_editting->rightBarcodeSettings.enable=arg1?true:false;
}

void MainWindow::on_comboBox_BarcodePort_Left_currentIndexChanged(const QString &arg1)
{
    qDebug()<<"on_comboBox_BarcodePort_Left_currentIndexChanged,arg1:"<<arg1;
    this->tempTooling_editting->leftBarcodeSettings.portName=arg1;

}

void MainWindow::on_comboBox__BarcodePort_Right_currentIndexChanged(const QString &arg1)
{
    qDebug()<<"on_comboBox_BarcodePort_right_currentIndexChanged,arg1:"<<arg1;
    this->tempTooling_editting->rightBarcodeSettings.portName=arg1;
}

void MainWindow::on_comboBox_Baud_currentIndexChanged(const QString &arg1)
{
    this->tempTooling_editting->leftBarcodeSettings.baud=arg1;
    this->tempTooling_editting->rightBarcodeSettings.baud=arg1;
}

void MainWindow::on_spinBox_barcodeDataBits_valueChanged(int arg1)
{
    this->tempTooling_editting->leftBarcodeSettings.dataBits=arg1;
    this->tempTooling_editting->rightBarcodeSettings.dataBits=arg1;
}

void MainWindow::on_spinBox_barcodeStopBits_valueChanged(int arg1)
{
    this->tempTooling_editting->leftBarcodeSettings.stopBits=arg1;
    this->tempTooling_editting->rightBarcodeSettings.stopBits=arg1;
}

void MainWindow::on_comboBox_parity_currentIndexChanged(const QString &arg1)
{
    this->tempTooling_editting->leftBarcodeSettings.parity=arg1;
    this->tempTooling_editting->rightBarcodeSettings.parity=arg1;
}

void MainWindow::on_spinBox_barcodeMaxLength_valueChanged(int arg1)
{
    this->tempTooling_editting->leftBarcodeSettings.maxLength=arg1;
    this->tempTooling_editting->rightBarcodeSettings.maxLength=arg1;
}

void MainWindow::on_spinBox_BarcodeMinLength_valueChanged(int arg1)
{
    this->tempTooling_editting->leftBarcodeSettings.minLength=arg1;
    this->tempTooling_editting->rightBarcodeSettings.minLength=arg1;
}

void MainWindow::on_lineEdit_barcode_prefix_textEdited(const QString &arg1)
{
    this->tempTooling_editting->leftBarcodeSettings.prefix=arg1;
    this->tempTooling_editting->rightBarcodeSettings.prefix=arg1;
}

void MainWindow::on_lineEdit_barcode_suffix_textEdited(const QString &arg1)
{
    this->tempTooling_editting->leftBarcodeSettings.suffix=arg1;
    this->tempTooling_editting->rightBarcodeSettings.suffix=arg1;
}

void MainWindow::on_pushButton_toolingSettingRefresh_clicked()
{

    QJsonObject json;
    this->tempTooling_editting->writeJason(json);

    QJsonDocument jsonDoc(json);

    this->ui->textEdit_toolingSetting_Overview->setText(QString(jsonDoc.toJson()));

}

void MainWindow::on_spinBox_filmFeeder_enable_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.feeder.enable=arg1;
}

void MainWindow::on_spinBox_filmFeeder_speed_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.feeder.speed=arg1;
}

void MainWindow::on_spinBox_filmFeeder_distance_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.feeder.distance_1=arg1;
}

void MainWindow::on_spinBox_filmFeeder_distance_2_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.feeder.distance_2=arg1;
}

void MainWindow::on_spinBox_filmFeeder_Interval_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.feeder.interval=arg1;
}

void MainWindow::on_comboBox_filmFeeder_Dir_currentIndexChanged(int index)
{
    this->tempTooling_editting->plcToolingInfo.feeder.direction=index;
}

void MainWindow::on_pushButton_toolingSettingSearch_clicked()
{
    QString search_text = this->ui->lineEdit_SearchWordsFromSettingOverview->text();
        if (search_text.trimmed().isEmpty()) {
            QMessageBox::information(this, tr("Empty search field"), tr("The search field is empty."));
        } else {
            QTextDocument *document =this->ui->textEdit_toolingSetting_Overview->document();
            bool found = false;
            QTextCursor highlight_cursor(document);
            QTextCursor cursor(document);
            //开始
            cursor.beginEditBlock();
            QTextCharFormat color_format(highlight_cursor.charFormat());
            color_format.setForeground(Qt::red);
            color_format.setBackground(Qt::yellow);
            while (!highlight_cursor.isNull() && !highlight_cursor.atEnd()) {
                //查找指定的文本，匹配整个单词
                highlight_cursor = document->find(search_text, highlight_cursor);
                if (!highlight_cursor.isNull()) {
                    if(!found)
                        found = true;
                    highlight_cursor.mergeCharFormat(color_format);
                }
            }
            cursor.endEditBlock();
            //结束
            if (found == false) {
                QMessageBox::information(this, tr("Word not found"), tr("Sorry,the word cannot be found."));
            }
        }
}

void MainWindow::on_checkBox_PartSensor_1_stateChanged(int arg1)
{
    qDebug()<<"on_checkBox_PartSensor_1_stateChanged(int arg1),arg1:"<<arg1;
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b1=arg1?true:false;
    qDebug()<<"tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b1,b1:"<<this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b1;
}

void MainWindow::on_checkBox_PartSensor_2_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b2=arg1?true:false;
}

void MainWindow::on_checkBox_PartSensor_3_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b3=arg1?true:false;
}

void MainWindow::on_checkBox_PartSensor_4_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b4=arg1?true:false;
}

void MainWindow::on_checkBox_PartSensor_5_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b5=arg1?true:false;
}

void MainWindow::on_checkBox_PartSensor_6_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.extendBypass.bits.b6=arg1?true:false;
}
void MainWindow::on_checkBox_PartSensor_7_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b1=arg1?true:false;
}

void MainWindow::on_checkBox_PartSensor_8_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b2=arg1?true:false;
}

void MainWindow::on_checkBox_PartSensor_9_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b3=arg1?true:false;
}

void MainWindow::on_checkBox_PartSensor_10_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b4=arg1?true:false;
}

void MainWindow::on_checkBox_PartSensor_11_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b5=arg1?true:false;
}

void MainWindow::on_checkBox_PartSensor_12_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.partSensorBypass.retractBypass.bits.b6=arg1?true:false;
}

void MainWindow::on_checkBox_ValveExtendSensorBypass_1_stateChanged(int arg1)
{
    qDebug()<<"on_checkBox_ValveExtendSensorBypass_1_stateChanged(int arg1),arg1:"<<arg1;

    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b1=arg1?true:false;
    qDebug()<<tr("valveSensorBypass[%1].extendBypass.bits.b1:%2").arg(this->ui->sensorBypass_valveNO->value())
              .arg(this->tempTooling_editting->plcToolingInfo.valveSensorBypass
                   [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b1);

}

void MainWindow::on_checkBox_ValveExtendSensorBypass_2_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b2=arg1?true:false;
}

void MainWindow::on_checkBox_ValveExtendSensorBypass_3_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b3=arg1?true:false;
}

void MainWindow::on_checkBox_ValveExtendSensorBypass_4_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b4=arg1?true:false;
}

void MainWindow::on_checkBox_ValveExtendSensorBypass_5_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b5=arg1?true:false;
}

void MainWindow::on_checkBox_ValveExtendSensorBypass_6_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].extendBypass.bits.b6=arg1?true:false;
}

void MainWindow::on_checkBox_ValveRetractSensorBypass_1_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b1=arg1?true:false;
}

void MainWindow::on_checkBox_ValveRetractSensorBypass_2_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b2=arg1?true:false;
}

void MainWindow::on_checkBox_ValveRetractSensorBypass_3_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b3=arg1?true:false;
}

void MainWindow::on_checkBox_ValveRetractSensorBypass_4_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b4=arg1?true:false;
}

void MainWindow::on_checkBox_ValveRetractSensorBypass_5_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b5=arg1?true:false;
}

void MainWindow::on_checkBox_ValveRetractSensorBypass_6_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.valveSensorBypass
            [this->ui->sensorBypass_valveNO->value()].retractBypass.bits.b6=arg1?true:false;
}

void MainWindow::on_btn_Edit2PLC_FilmFeeder_clicked()
{
    //set film feeder related parameters
    qDebug()<<"set film feeder para btn clicked";
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x18;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7B;//commandNO low byte,123
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->spinBox_filmFeeder_enable->value();
    dataToTcpCommObj[7]=this->ui->comboBox_filmFeeder_Dir->currentIndex();
    dWordBytes dw1;
    dw1.DWordVar=this->ui->spinBox_filmFeeder_speed->value();
    dataToTcpCommObj[8]=dw1.bytesVar.B0;
    dataToTcpCommObj[9]=dw1.bytesVar.B1;
    dataToTcpCommObj[10]=dw1.bytesVar.B2;
    dataToTcpCommObj[11]=dw1.bytesVar.B3;
    dw1.DWordVar=this->ui->spinBox_filmFeeder_distance->value();
    dataToTcpCommObj[12]=dw1.bytesVar.B0;
    dataToTcpCommObj[13]=dw1.bytesVar.B1;
    dataToTcpCommObj[14]=dw1.bytesVar.B2;
    dataToTcpCommObj[15]=dw1.bytesVar.B3;
    dw1.DWordVar=this->ui->spinBox_filmFeeder_distance_2->value();
    dataToTcpCommObj[16]=dw1.bytesVar.B0;
    dataToTcpCommObj[17]=dw1.bytesVar.B1;
    dataToTcpCommObj[18]=dw1.bytesVar.B2;
    dataToTcpCommObj[19]=dw1.bytesVar.B3;
    dataToTcpCommObj[20]=this->ui->spinBox_filmFeeder_Interval->value();
    dataToTcpCommObj[21]=0;//reserve
    dataToTcpCommObj[22]=0;//reserve
    dataToTcpCommObj[23]=0;//reserve
    //emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    //query film feeder  config,doing
    dataToTcpCommObj[24]=0x00;//length high byte
    dataToTcpCommObj[25]=0x06;//length low byte
    dataToTcpCommObj[26]=0x00;//commandNO high byte
    dataToTcpCommObj[27]=0x7C;//commandNO low byte,124
    dataToTcpCommObj[28]=0x00;//reserve byte
    dataToTcpCommObj[29]=0x00;//reserve byte
    if(this->tcpConnectionStatus_send)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_refresh_FilmFeeder_clicked()
{
    //query film feeder  config
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7C;//commandNO low byte,124
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    if(this->tcpConnectionStatus_send)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_PLC2Edit_FilmFeeder_clicked()
{
    this->ui->spinBox_filmFeeder_enable->setValue(this->ui->CV_filmFeeder_Enable->value());
    this->ui->spinBox_filmFeeder_speed->setValue(this->ui->CV_filmFeeder_speed->value());
    this->ui->spinBox_filmFeeder_distance->setValue(this->ui->CV_filmFeeder_distance->value());
    this->ui->spinBox_filmFeeder_distance_2->setValue(this->ui->CV_filmFeeder_distance_2->value());
    this->ui->spinBox_filmFeeder_Interval->setValue(this->ui->CV_filmFeeder_Interval->value());
    this->ui->comboBox_filmFeeder_Dir->setCurrentIndex(this->ui->CV_filmFeeder_Dir->currentIndex());
}



void MainWindow::on_ToolingChange_valveNO_valueChanged(int arg1)
{
    //this->ui->comboBox_ValveType_toolingChange->setCurrentIndex
            //(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].valveType);
    this->ui->ToolingChange_ValveName->setText(this->tempTooling_editting->valveNameMapping.at(arg1));
}

void MainWindow::on_Clamper_Extend_toolingChange_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7E;//commandNO low byte,126
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->ToolingChange_valveNO->value();//valveNO
    dataToTcpCommObj[7]=0x01;//coilA
    dataToTcpCommObj[8]=0x00;//coilB
    dataToTcpCommObj[9]=0x00;//reserve
    dataToTcpCommObj[10]=0x00;//reserve
    dataToTcpCommObj[11]=0x00;//reserve

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_Clamper_Free_ToolingChange_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7E;//commandNO low byte,126
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->ToolingChange_valveNO->value();//valveNO
    dataToTcpCommObj[7]=0x00;//coilA
    dataToTcpCommObj[8]=0x00;//coilB
    dataToTcpCommObj[9]=0x00;//reserve
    dataToTcpCommObj[10]=0x00;//reserve
    dataToTcpCommObj[11]=0x00;//reserve

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_Clamper_Retract_ToolingChange_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7E;//commandNO low byte,126
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->ToolingChange_valveNO->value();//valveNO
    dataToTcpCommObj[7]=0x00;//coilA
    dataToTcpCommObj[8]=0x01;//coilB
    dataToTcpCommObj[9]=0x00;//reserve
    dataToTcpCommObj[10]=0x00;//reserve
    dataToTcpCommObj[11]=0x00;//reserve

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}



void MainWindow::on_btn_Edit2PLC_PartSensor_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x70;//commandNO low byte,112
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x01;//0x01=set,0x02=get
    dataToTcpCommObj[7]=0x01;//0x01=part sensor,0x02=valve sensor
    dataToTcpCommObj[8]=0x00;//valve NO
    bytebits bb1;
    bb1.byteChar=0;
    bb1.bits.b1=this->ui->checkBox_PartSensor_1->isChecked();
    bb1.bits.b2=this->ui->checkBox_PartSensor_2->isChecked();
    bb1.bits.b3=this->ui->checkBox_PartSensor_3->isChecked();
    bb1.bits.b4=this->ui->checkBox_PartSensor_4->isChecked();
    bb1.bits.b5=this->ui->checkBox_PartSensor_5->isChecked();
    bb1.bits.b6=this->ui->checkBox_PartSensor_6->isChecked();
    dataToTcpCommObj[9]=bb1.byteChar;//sensors enable status in Byte
    bb1.byteChar=0;
    bb1.bits.b1=this->ui->checkBox_PartSensor_7->isChecked();
    bb1.bits.b2=this->ui->checkBox_PartSensor_8->isChecked();
    bb1.bits.b3=this->ui->checkBox_PartSensor_9->isChecked();
    bb1.bits.b4=this->ui->checkBox_PartSensor_10->isChecked();
    bb1.bits.b5=this->ui->checkBox_PartSensor_11->isChecked();
    bb1.bits.b6=this->ui->checkBox_PartSensor_12->isChecked();
    dataToTcpCommObj[10]=bb1.byteChar;//sensors enable status in Byte
    dataToTcpCommObj[11]=0x00;//reserve

    //query part sensor status
    dataToTcpCommObj[12]=0x00;//length high byte
    dataToTcpCommObj[13]=0x0C;//length low byte
    dataToTcpCommObj[14]=0x00;//commandNO high byte
    dataToTcpCommObj[15]=0x70;//commandNO low byte,112
    dataToTcpCommObj[16]=0x00;//reserve byte
    dataToTcpCommObj[17]=0x00;//reserve byte
    dataToTcpCommObj[18]=0x02;//0x01=set,0x02=get
    dataToTcpCommObj[19]=0x01;//0x01=part sensor,0x02=valve sensor
    dataToTcpCommObj[20]=0x00;//valve NO

    dataToTcpCommObj[21]=0x00;//sensors enable status in Byte

    dataToTcpCommObj[22]=0x00;//sensors enable status in Byte
    dataToTcpCommObj[23]=0x00;//reserve


    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);

}

void MainWindow::on_btn_refresh_PartSensor_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x70;//commandNO low byte,112
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x02;//0x01=set,0x02=get
    dataToTcpCommObj[7]=0x01;//0x01=part sensor,0x02=valve sensor
    dataToTcpCommObj[8]=0x00;//valve NO

    dataToTcpCommObj[9]=0x00;//sensors enable status in Byte

    dataToTcpCommObj[10]=0x00;//sensors enable status in Byte
    dataToTcpCommObj[11]=0x00;//reserve

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_PLC2Edit_PartSensor_clicked()
{
    this->ui->checkBox_PartSensor_1->setChecked(this->ui->CV_PartSensor_1->isChecked());
    this->ui->checkBox_PartSensor_2->setChecked(this->ui->CV_PartSensor_2->isChecked());
    this->ui->checkBox_PartSensor_3->setChecked(this->ui->CV_PartSensor_3->isChecked());
    this->ui->checkBox_PartSensor_4->setChecked(this->ui->CV_PartSensor_4->isChecked());
    this->ui->checkBox_PartSensor_5->setChecked(this->ui->CV_PartSensor_5->isChecked());
    this->ui->checkBox_PartSensor_6->setChecked(this->ui->CV_PartSensor_6->isChecked());
    this->ui->checkBox_PartSensor_7->setChecked(this->ui->CV_PartSensor_7->isChecked());
    this->ui->checkBox_PartSensor_8->setChecked(this->ui->CV_PartSensor_8->isChecked());
    this->ui->checkBox_PartSensor_9->setChecked(this->ui->CV_PartSensor_9->isChecked());
    this->ui->checkBox_PartSensor_10->setChecked(this->ui->CV_PartSensor_10->isChecked());
    this->ui->checkBox_PartSensor_11->setChecked(this->ui->CV_PartSensor_11->isChecked());
    this->ui->checkBox_PartSensor_12->setChecked(this->ui->CV_PartSensor_12->isChecked());
}

void MainWindow::on_sensorBypass_valveNO_valueChanged(int arg1)
{
    //this->ui->comboBox_ValveType_sensorBypass->setCurrentIndex
            //(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].valveType);
    this->ui->valveBypass_valveName->setText(this->tempTooling_editting->valveNameMapping.at(arg1));
    this->ui->checkBox_ValveExtendSensorBypass_1->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b1);
    this->ui->checkBox_ValveExtendSensorBypass_2->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b2);
    this->ui->checkBox_ValveExtendSensorBypass_3->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b3);
    this->ui->checkBox_ValveExtendSensorBypass_4->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b4);
    this->ui->checkBox_ValveExtendSensorBypass_5->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b5);
    this->ui->checkBox_ValveExtendSensorBypass_6->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b6);
    this->ui->checkBox_ValveRetractSensorBypass_1->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b1);
    this->ui->checkBox_ValveRetractSensorBypass_2->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b2);
    this->ui->checkBox_ValveRetractSensorBypass_3->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b3);
    this->ui->checkBox_ValveRetractSensorBypass_4->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b4);
    this->ui->checkBox_ValveRetractSensorBypass_5->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b5);
    this->ui->checkBox_ValveRetractSensorBypass_6->setChecked
            (this->tempTooling_editting->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b6);
    this->ui->CV_ValveExtendSensorBypass_1->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b1);
    this->ui->CV_ValveExtendSensorBypass_2->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b2);
    this->ui->CV_ValveExtendSensorBypass_3->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b3);
    this->ui->CV_ValveExtendSensorBypass_4->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b4);
    this->ui->CV_ValveExtendSensorBypass_5->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b5);
    this->ui->CV_ValveExtendSensorBypass_6->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].extendBypass.bits.b6);
    this->ui->CV_ValveRetractSensorBypass_1->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b1);
    this->ui->CV_ValveRetractSensorBypass_2->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b2);
    this->ui->CV_ValveRetractSensorBypass_3->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b3);
    this->ui->CV_ValveRetractSensorBypass_4->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b4);
    this->ui->CV_ValveRetractSensorBypass_5->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b5);
    this->ui->CV_ValveRetractSensorBypass_6->setChecked
            (this->tooling_current->plcToolingInfo.valveSensorBypass[arg1].retractBypass.bits.b6);


}

void MainWindow::on_btn_refresh_valveSensor_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x70;//commandNO low byte,112
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x02;//0x01=set,0x02=get
    dataToTcpCommObj[7]=0x02;//0x01=part sensor,0x02=valve sensor
    dataToTcpCommObj[8]=this->ui->sensorBypass_valveNO->value();//valve NO

    dataToTcpCommObj[9]=0x00;//sensors enable status in Byte

    dataToTcpCommObj[10]=0x00;//sensors enable status in Byte
    dataToTcpCommObj[11]=0x00;//reserve

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_Edit2PLC_valveSensor_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x70;//commandNO low byte,112
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x01;//0x01=set,0x02=get
    dataToTcpCommObj[7]=0x02;//0x01=part sensor,0x02=valve sensor
    dataToTcpCommObj[8]=this->ui->sensorBypass_valveNO->value();//valve NO
    bytebits bb1;
    bb1.byteChar=0;
    bb1.bits.b1=this->ui->checkBox_ValveExtendSensorBypass_1->isChecked();
    bb1.bits.b2=this->ui->checkBox_ValveExtendSensorBypass_2->isChecked();
    bb1.bits.b3=this->ui->checkBox_ValveExtendSensorBypass_3->isChecked();
    bb1.bits.b4=this->ui->checkBox_ValveExtendSensorBypass_4->isChecked();
    bb1.bits.b5=this->ui->checkBox_ValveExtendSensorBypass_5->isChecked();
    bb1.bits.b6=this->ui->checkBox_ValveExtendSensorBypass_6->isChecked();
    dataToTcpCommObj[9]=bb1.byteChar;//sensors enable status in Byte
    bb1.byteChar=0;
    bb1.bits.b1=this->ui->checkBox_ValveRetractSensorBypass_1->isChecked();
    bb1.bits.b2=this->ui->checkBox_ValveRetractSensorBypass_2->isChecked();
    bb1.bits.b3=this->ui->checkBox_ValveRetractSensorBypass_3->isChecked();
    bb1.bits.b4=this->ui->checkBox_ValveRetractSensorBypass_4->isChecked();
    bb1.bits.b5=this->ui->checkBox_ValveRetractSensorBypass_5->isChecked();
    bb1.bits.b6=this->ui->checkBox_ValveRetractSensorBypass_6->isChecked();
    dataToTcpCommObj[10]=bb1.byteChar;//sensors enable status in Byte
    dataToTcpCommObj[11]=0x00;//reserve
    //query valve sensor bypass status
    dataToTcpCommObj[12]=0x00;//length high byte
    dataToTcpCommObj[13]=0x0C;//length low byte
    dataToTcpCommObj[14]=0x00;//commandNO high byte
    dataToTcpCommObj[15]=0x70;//commandNO low byte,112
    dataToTcpCommObj[16]=0x00;//reserve byte
    dataToTcpCommObj[17]=0x00;//reserve byte
    dataToTcpCommObj[18]=0x02;//0x01=set,0x02=get
    dataToTcpCommObj[19]=0x02;//0x01=part sensor,0x02=valve sensor
    dataToTcpCommObj[20]=this->ui->sensorBypass_valveNO->value();//valve NO
    dataToTcpCommObj[21]=0x00;//sensors enable status in Byte
    dataToTcpCommObj[22]=0x00;//sensors enable status in Byte
    dataToTcpCommObj[23]=0x00;//reserve

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_PLC2Edit_valveSensor_clicked()
{
    this->ui->checkBox_ValveRetractSensorBypass_1->setChecked
            (this->ui->CV_ValveRetractSensorBypass_1->isChecked());
    this->ui->checkBox_ValveRetractSensorBypass_2->setChecked
            (this->ui->CV_ValveRetractSensorBypass_2->isChecked());
    this->ui->checkBox_ValveRetractSensorBypass_3->setChecked
            (this->ui->CV_ValveRetractSensorBypass_3->isChecked());
    this->ui->checkBox_ValveRetractSensorBypass_4->setChecked
            (this->ui->CV_ValveRetractSensorBypass_4->isChecked());
    this->ui->checkBox_ValveRetractSensorBypass_5->setChecked
            (this->ui->CV_ValveRetractSensorBypass_5->isChecked());
    this->ui->checkBox_ValveRetractSensorBypass_6->setChecked
            (this->ui->CV_ValveRetractSensorBypass_6->isChecked());
    this->ui->checkBox_ValveExtendSensorBypass_1->setChecked
            (this->ui->CV_ValveExtendSensorBypass_1->isChecked());
    this->ui->checkBox_ValveExtendSensorBypass_2->setChecked
            (this->ui->CV_ValveExtendSensorBypass_2->isChecked());
    this->ui->checkBox_ValveExtendSensorBypass_3->setChecked
            (this->ui->CV_ValveExtendSensorBypass_3->isChecked());
    this->ui->checkBox_ValveExtendSensorBypass_4->setChecked
            (this->ui->CV_ValveExtendSensorBypass_4->isChecked());
    this->ui->checkBox_ValveExtendSensorBypass_5->setChecked
            (this->ui->CV_ValveExtendSensorBypass_5->isChecked());
    this->ui->checkBox_ValveExtendSensorBypass_6->setChecked
            (this->ui->CV_ValveExtendSensorBypass_6->isChecked());

}

void MainWindow::on_pushButton_logIN_leave_clicked()
{
    this->changePage(255);
}

void MainWindow::on_pushButton_logIN_GO_clicked()
{
    quint8 userID=this->ui->UserID_spinbox->value();
    if(this->ui->lineEdit_password->text()!=this->systemRegisteredTextList.value(userID))
    {
       this->ui->label_password_wrong->setVisible(true);
    }
    else
    {
        this->ui->lineEdit_password->clear();
        this->ui->label_password_wrong->setVisible(false);
        if(userID>=1&&userID<=5)
        {
            this->parameterEditable=true;
        }
        else if (userID>=6&&userID<=11)
        {
           this->parameterEditable=false;
        }
        //userID 11 is used for parameterChecking
        if(userID==11)
        {
            //prevent operator enter "weld by manual" page by clicking the "leave" btn
            this->ui->btn_leave_toolingConfig->setEnabled(false);
            //display the point basic para by defult when para checking started
            this->ui->stackedWidget_mainProgram->setCurrentIndex(0);
            this->ui->tab_toolingConfig->setCurrentIndex(3);
            this->ui->tabWidget_weldPoint->setCurrentIndex(1);
            //prevent operator to move servo by clicking the "goto" btn
            this->ui->stationConfig_servo_GOTO->setEnabled(false);
            //prevent operator to change editting toolID
            this->ui->toolID_Editting->setEnabled(false);
        }
        else
        {
            //other user logged in, retore the changes did by parachecking
            this->ui->btn_leave_toolingConfig->setEnabled(true);
            this->ui->stationConfig_servo_GOTO->setEnabled(true);
            this->ui->toolID_Editting->setEnabled(true);
        }
        emit parameterEditableStausChanged(this->parameterEditable);
        this->ui->stackedWidget_mainProgram->setCurrentIndex(this->pageInfo1.targetPage_Index_mainStackWidget);
        quint8 loginDuration=0;
        bool needLogOff=true;
        switch (this->ui->comboBox_LogOffTime->currentIndex()) {
        case 0:
            this->logInStatus=false;
            needLogOff=true;
            break;
        case 1:
            this->logInStatus=true;
            needLogOff=true;
            loginDuration=1;
            break;
        case 2:
            this->logInStatus=true;
            needLogOff=true;
            loginDuration=5;
            break;
        case 3:
            this->logInStatus=true;
            needLogOff=true;
            loginDuration=10;
            break;
        case 4:
            this->logInStatus=true;
            needLogOff=false;
            loginDuration=10;
            break;
        default:
            break;
        }


        if(this->logInStatus&&needLogOff)
        {
          QTimer::singleShot(60000*loginDuration, this, &OnLogInTimeout);
        }

    }

}
void MainWindow::on_pushButton_paraChecking_clicked()
{
    this->ui->comboBox_LogOffTime->setCurrentIndex(3);
    this->ui->UserID_spinbox->setValue(11);
    this->ui->lineEdit_password->setText(this->systemRegisteredTextList.value(11));
    this->ui->comboBox_LogOffTime->setCurrentIndex(0);
    //simulate operator to press the logIn btn
    this->on_pushButton_logIN_GO_clicked();

}

void MainWindow::on_stackedWidget_mainProgram_currentChanged(int arg1)
{
    //arg1==0,toolConfig
    //arg1==1,weldBymanual
    //arg1==2,IO Table
    //arg1==3,Alarm
    //arg1==4,run screen
    //arg1==5,log on
    //arg1==6,discard,text reg
    //arg1==7,tool change
    //arg1==8,machine info
    //arg1==9,welcome page
    this->pageInfo1.previousPage_Index_mainStackWidget=this->pageInfo1.currentPage_Index_mainStackWidget;
    this->pageInfo1.currentPage_Index_mainStackWidget=arg1;
    qDebug()<<"page changed,previous page:"<<this->pageInfo1.previousPage_Index_mainStackWidget;
    qDebug()<<"page changed,current page:"<<this->pageInfo1.currentPage_Index_mainStackWidget;
    //hide "check"
    if(arg1==9)
    {
        this->ui->btn_CheckAlarm->setVisible(false);
        this->ui->pushButton_language->setVisible(false);
    }
    else
    {
        if(!this->ui->btn_CheckAlarm->isVisible())
          this->ui->btn_CheckAlarm->setVisible(true);
        if(!this->ui->pushButton_language->isVisible())
            this->ui->pushButton_language->setVisible(true);

    }
    //when pageNO changed, hide text_reg groupBox,disable machineInfo editting
    if(this->ui->groupBox_textReg->isVisible())
       this->ui->groupBox_textReg->setVisible(false);
    if(this->ui->groupBox_machineInfo->isEnabled())
       this->ui->groupBox_machineInfo->setEnabled(false);

    switch (arg1)
    {
    case 0://tooling config page
    {

        //toolImage presetting
        if(!this->tempTooling_editting->toolingImageSource.isEmpty())
        {
            this->setPixmapForLabel(this->ui->toolingPicLabel,this->tempTooling_editting->toolingImageSource);

        }
        else
        {
            this->setPixmapForLabel(this->ui->toolingPicLabel,":/img/dukaneLogo.PNG");

        }
        //valve NO&&Name preset to 1
        this->ui->spinBox_valveNO->setValue(1);
        this->ui->valveName_lineEdit->setText(this->tempTooling_editting->valveNameMapping.at(1));
        this->ui->sensorBypass_valveNO->setValue(1);
        this->ui->valveBypass_valveName->setText(this->tempTooling_editting->valveNameMapping.at(1));
        break;

    }
    case 1://weld by manual page
    {
        //CMD109, prepare info for weld by manual
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x08;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x6D;//commandNO low byte,109
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        //set pointNO to 1
        this->ui->WeldByManual_PointNO->setValue(1);
        dataToTcpCommObj[6]=0x01;//PointNO
        dataToTcpCommObj[7]=0x00;//reserved
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
        //valve NO&&Name preset to 4
        this->ui->WeldByManual_valveNO->setValue(4);//left clamper selected
        this->ui->WeldByManual_valveName->setText(this->tempTooling_editting->valveNameMapping.at(4));
        break;
    }
    case 4://run monitor page
    {
        if(!this->tempTooling_editting->toolingImageSource.isEmpty())
        {

            this->setPixmapForLabel(this->ui->label_ToolingPicture_run,this->tempTooling_editting->toolingImageSource);
        }
        else
        {

            this->setPixmapForLabel(this->ui->label_ToolingPicture_run,":/img/dukaneLogo.PNG");
        }

        break;
    }
    case 7://tool change
    {
        //valve NO&&Name preset to 1
        this->ui->ToolingChange_valveNO->setValue(1);
        this->ui->ToolingChange_ValveName->setText(this->tempTooling_editting->valveNameMapping.at(1));
        break;
    }
    case 8:
    {
        if(this->machineInfoReady)
        {
            this->ui->lineEdit_machineName->setText(this->machineInfo1.machineName);
            this->ui->lineEdit_PLC_IPAddress->setText(this->machineInfo1.PLC_IPAddress);
            this->ui->spinBox_machineInfo_frq_upper->setValue(this->machineInfo1.frequencyLimit_upper);
            this->ui->spinBox_machineInfo_frq_lower->setValue(this->machineInfo1.frequencyLimit_lower);
        }

        break;
    }

    default:
        break;
    }


}

void MainWindow::on_btn_filmfeeder_JOG1_weldByManual_toggled(bool checked)
{
    if(checked)
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x0D;//13 Jog1
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
    else
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//stop
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
}
void MainWindow::on_btn_filmfeeder_JOG2_weldByManual_toggled(bool checked)
{
    if(checked)
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x0E;//14 Jog2
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
    else
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//stop
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
}
void MainWindow::on_btn_filmfeeder_RunSpeed__weldByManual_toggled(bool checked)
{
    if(checked)
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x0B;//11 run speed
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
    else
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//stop
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
}
void MainWindow::on_btn_filmfeeder_RunRelative__weldByManual_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x0C;//12 run relative_1
    dataToTcpCommObj[7]=0x00;//reserve
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}
void MainWindow::on_btn_filmfeeder_RunRelative__weldByManual_2_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x0F;//15 run relative_2
    dataToTcpCommObj[7]=0x00;//reserve
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}
void MainWindow::on_btn_filmfeeder_JOG1_toggled(bool checked)
{
    if(checked)
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x0D;//13 Jog1
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
    else
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//stop
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
}
void MainWindow::on_btn_filmfeeder_JOG2_toggled(bool checked)
{
    if(checked)
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x0E;//14 Jog2
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
    else
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//stop
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
}
void MainWindow::on_btn_filmfeeder_RunSpeed_toggled(bool checked)
{
    if(checked)
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x0B;//11 run speed
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
    else
    {
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//stop
        dataToTcpCommObj[7]=0x00;//reserve
        dataToTcpCommObj[8]=0x00;//reserve
        dataToTcpCommObj[9]=0x00;//reserve
        if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
            emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    }
}
void MainWindow::on_btn_filmfeeder_RunRelative_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x0C;//12 run relative_1
    dataToTcpCommObj[7]=0x00;//reserve
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}
void MainWindow::on_btn_filmfeeder_RunRelative_2_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7D;//commandNO low byte,125
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x0F;//15 run relative_2
    dataToTcpCommObj[7]=0x00;//reserve
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_actionWeldByManual_triggered()
{
    this->changePage(1);
}
void MainWindow::on_actionWeld_By_Manual_triggered()
{


}
void MainWindow::on_actionToolingChange_triggered()
{
    this->changePage(7);
}

void MainWindow::on_btn_QueryServoPara_clicked()
{
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_getstationParaFromPLC());
}

void MainWindow::on_btn_toLoadPos_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x79;//commandNO low byte,121
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x0F;//15 MDI
    dataToTcpCommObj[7]=0x01;//MDI station NO
    dataToTcpCommObj[8]=0x00;//reserve
    dataToTcpCommObj[9]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_CheckAlarm_clicked()
{
    this->changePage(3);

}

void MainWindow::on_btn_leave_toolingConfig_clicked()
{
    this->changePage(1);
}

void MainWindow::on_btn_toToolingChange_clicked()
{
    this->changePage(7);
}

void MainWindow::on_btn_toToolingGonfig_clicked()
{
    this->changePage(0);
}

void MainWindow::on_btn_leave_toolingChange_clicked()
{
    this->changePage(1);
}

void MainWindow::on_btn_pointBypass_toPLC_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7F;//commandNO low byte,127
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x01;//01==set,02==get
    dataToTcpCommObj[7]=this->ui->checkBox_PointBypass_retain->isChecked()?1:0;//retain flag
    bytebits bb1;
    bb1.bits.b0=this->ui->checkBox_PointBypass_point_1->isChecked()?true:false;
    bb1.bits.b1=this->ui->checkBox_PointBypass_point_2->isChecked()?true:false;
    bb1.bits.b2=this->ui->checkBox_PointBypass_point_3->isChecked()?true:false;
    bb1.bits.b3=this->ui->checkBox_PointBypass_point_4->isChecked()?true:false;
    bb1.bits.b4=this->ui->checkBox_PointBypass_point_5->isChecked()?true:false;
    bb1.bits.b5=this->ui->checkBox_PointBypass_point_6->isChecked()?true:false;
    bb1.bits.b6=this->ui->checkBox_PointBypass_point_7->isChecked()?true:false;
    bb1.bits.b7=this->ui->checkBox_PointBypass_point_8->isChecked()?true:false;
    dataToTcpCommObj[8]=bb1.byteChar;//point 1~8
    bb1.bits.b0=this->ui->checkBox_PointBypass_point_9->isChecked()?true:false;
    bb1.bits.b1=this->ui->checkBox_PointBypass_point_10->isChecked()?true:false;
    bb1.bits.b2=this->ui->checkBox_PointBypass_point_11->isChecked()?true:false;
    bb1.bits.b3=this->ui->checkBox_PointBypass_point_12->isChecked()?true:false;
    bb1.bits.b4=this->ui->checkBox_PointBypass_point_13->isChecked()?true:false;
    bb1.bits.b5=this->ui->checkBox_PointBypass_point_14->isChecked()?true:false;
    bb1.bits.b6=this->ui->checkBox_PointBypass_point_15->isChecked()?true:false;
    bb1.bits.b7=this->ui->checkBox_PointBypass_point_16->isChecked()?true:false;
    dataToTcpCommObj[9]=bb1.byteChar;//point 9~16
    dataToTcpCommObj[10]=0x00;//reserve
    dataToTcpCommObj[11]=0x00;//reserve
    //query bypass status after sent set command
    dataToTcpCommObj[12]=0x00;//length high byte
    dataToTcpCommObj[13]=0x0C;//length low byte
    dataToTcpCommObj[14]=0x00;//commandNO high byte
    dataToTcpCommObj[15]=0x7F;//commandNO low byte,127
    dataToTcpCommObj[16]=0x00;//reserve byte
    dataToTcpCommObj[17]=0x00;//reserve byte
    dataToTcpCommObj[18]=0x02;//01==set,02==get
    dataToTcpCommObj[19]=0x00;//retain flag
    dataToTcpCommObj[20]=0x00;//point 1~8
    dataToTcpCommObj[21]=0x00;//point 9~16
    dataToTcpCommObj[22]=0x00;//reserve
    dataToTcpCommObj[23]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_pointBypass_fromPLC_clicked()
{
    this->ui->checkBox_PointBypass_retain->setChecked(this->ui->CV_PointBypass_retain->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_1->setChecked(this->ui->CV_PointBypass_point_1->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_2->setChecked(this->ui->CV_PointBypass_point_2->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_3->setChecked(this->ui->CV_PointBypass_point_3->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_4->setChecked(this->ui->CV_PointBypass_point_4->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_5->setChecked(this->ui->CV_PointBypass_point_5->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_6->setChecked(this->ui->CV_PointBypass_point_6->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_7->setChecked(this->ui->CV_PointBypass_point_7->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_8->setChecked(this->ui->CV_PointBypass_point_8->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_9->setChecked(this->ui->CV_PointBypass_point_9->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_10->setChecked(this->ui->CV_PointBypass_point_10->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_11->setChecked(this->ui->CV_PointBypass_point_11->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_12->setChecked(this->ui->CV_PointBypass_point_12->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_13->setChecked(this->ui->CV_PointBypass_point_13->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_14->setChecked(this->ui->CV_PointBypass_point_14->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_15->setChecked(this->ui->CV_PointBypass_point_15->isChecked()?true:false);
    this->ui->checkBox_PointBypass_point_16->setChecked(this->ui->CV_PointBypass_point_16->isChecked()?true:false);

}

void MainWindow::on_btn_pointBypass_refresh_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7F;//commandNO low byte,127
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x02;//01==set,02==get
    dataToTcpCommObj[7]=0x00;//retain flag
    dataToTcpCommObj[8]=0x00;//point 1~8
    dataToTcpCommObj[9]=0x00;//point 9~16
    dataToTcpCommObj[10]=0x00;//reserve
    dataToTcpCommObj[11]=0x00;//reserve
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_pointBypass_clicked()
{
    this->ui->stackedWidget_run->setCurrentIndex(1);
}

void MainWindow::on_btn_run_monitor_clicked()
{
    this->ui->stackedWidget_run->setCurrentIndex(0);
}

void MainWindow::on_pushButton_leaveMachineInfo_clicked()
{
    this->changePage(255);

}

void MainWindow::on_pushButton_saveMachineInfo_clicked()
{
    this->machineInfo1.machineName=this->ui->lineEdit_machineName->text();
    this->machineInfo1.PLC_IPAddress=this->ui->lineEdit_PLC_IPAddress->text();
    this->machineInfo1.frequencyLimit_upper=this->ui->spinBox_machineInfo_frq_upper->value();
    this->machineInfo1.frequencyLimit_lower=this->ui->spinBox_machineInfo_frq_lower->value();
    QFile loadFile("machineInfo.txt");

         if (!loadFile.open(QIODevice::WriteOnly)) {
             qWarning("Couldn't open  file when try to save machine info");
             return;
         }
         else
         {
             qDebug()<<"file opened,file.size:"<<loadFile.size();
             QDataStream Out1(&loadFile);
             Out1<<this->machineInfo1.machineName<<this->machineInfo1.PLC_IPAddress
                <<this->machineInfo1.frequencyLimit_upper<<this->machineInfo1.frequencyLimit_lower;
             this->machineInfoReady=true;
             qDebug()<<"machine info save to file executed, file.size()"<<loadFile.size();
             loadFile.close();
         }

}
void MainWindow::on_actionMachine_Infomation_triggered()
{
    this->changePage(8);
}


void MainWindow::on_pushButton_registText_import_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "./",
                                                    tr("CSV file (*.csv *.CSV)"));
    if(!fileName.isEmpty())
    {
        QFile loadFile(fileName);

             if (!loadFile.open(QIODevice::ReadOnly)) {
                 qWarning("Couldn't open  file when try to import reg_text");
                 return;
             }
             else
             {
                 qDebug()<<"import reg_text, file opened,file.size:"<<loadFile.size();

                 QTextStream In1(&loadFile);
                 if(!In1.readLine().contains("predefined"))
                 {
                    qWarning()<<"import reg_text ,bad file format";
                    QMessageBox::information(this, tr("bad file format"), tr("bad file format."));
                    return;
                 }

                 QString line,key,content;

                 quint16 lineCount=0;
                 QStringList lineToList;

                 //backup user info before clear the text list for later restore
                 QMap<quint32,QString>userInfo;
                 for(int i=1;i<=10;i++)
                 {
                    userInfo[i]=this->systemRegisteredTextList.value(i);
                 }
                 this->systemRegisteredTextList.clear();
                 //restore user info
                 for(int i=1;i<=10;i++)
                 {
                    this->systemRegisteredTextList[i]=userInfo.value(i);
                 }
                 while (!In1.atEnd())//逐行读取文本，并去除每行的回车
                 {
                     qDebug()<<"reading file,line NO"<<lineCount;
                     line = In1.readLine();
                     qDebug()<<"line:"<<line;
                     line.remove('\n');
                     lineToList=line.split(';');
                     if(lineToList.size()<2)
                     {
                         continue;
                     }
                     key=lineToList.at(0);
                     content=lineToList.at(1);
                     qDebug()<<tr("reading file, key:%1,content:%2,key.toInt():%3")
                               .arg(key).arg(content).arg(key.toInt());
                     if(!content.isEmpty())
                     {

                         this->systemRegisteredTextList[key.toInt()]=content;
                     }

                     lineCount++;

                 }
                 loadFile.close();
                 //save new reg_text to file
                 QFile file1("sysRegText.txt");
                  if (!file1.open(QIODevice::WriteOnly)) {
                      qWarning("Couldn't open  file when try to save new imported Registed text");

                  }
                  else
                  {
                      qDebug()<<"file opened,file.size:"<<file1.size();
                      QDataStream Out1(&file1);
                      Out1<<QString("predefined text list")<<(this->systemRegisteredTextList);
                      qDebug()<<"save to file execed, file.size()"<<file1.size();
                      file1.close();
                  }
                 qDebug()<<QTime::currentTime()<<"file reading done,line count"<<lineCount;
                 //display the new imported reg_text
                 QString itemText;
                 QString allRegistedText;
                 QList<quint32> keys=this->systemRegisteredTextList.keys();
                 for(int i=0;i<keys.size();i++)
                 {
                    itemText=tr("ID:%1,Text:%2 \n").arg(keys.at(i)).arg(this->systemRegisteredTextList[keys.at(i)]);
                    allRegistedText.append(itemText);
                 }
                 this->ui->textEdit_sysRegistedText->setText(allRegistedText);

             }
    }
}
void MainWindow::on_pushButton_registText_export_clicked()
{
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save File"),"./",tr("CSV file(*.csv *.CSV)"));
    QFile loadFile(fileName);

         if (!loadFile.open(QIODevice::WriteOnly)) {
             qWarning("Couldn't open  file when try to export reg_text");
             return;
         }
         else
         {
             qDebug()<<"export reg_text, file opened,file.size:"<<loadFile.size();

             QTextStream Out1(&loadFile);
             Out1.seek(0);
             Out1<<"predefined text list \n";
             QString itemText;
             QList<quint32> keys=this->systemRegisteredTextList.keys();
             for(int i=0;i<keys.size();i++)
             {
                itemText=tr("%1;%2\n").arg(keys.at(i)).arg(this->systemRegisteredTextList[keys.at(i)]);
                Out1<<itemText;
             }
             loadFile.close();
         }

}
void MainWindow::onParameterEditableStausChanged(bool editable)
{

    this->ui->toolName_lineEdit->setEnabled(editable);
    this->ui->btn_saveToDisk->setEnabled(editable);
    this->ui->btn_selectToolingImage->setEnabled(editable);
    this->ui->pushButton_removeImage->setEnabled(editable);
    this->ui->btn_uploadFromPLC->setEnabled(editable);
    this->ui->btn_downLoadToPLC->setEnabled(editable);

    this->ui->checkBox_GenEnable_1->setEnabled(editable);
    this->ui->checkBox_GenEnable_2->setEnabled(editable);
    this->ui->checkBox_GenEnable_3->setEnabled(editable);
    this->ui->checkBox_GenEnable_4->setEnabled(editable);
    this->ui->btn_setGenEnable->setEnabled(editable);
    this->ui->btn_undoGenEnable->setEnabled(editable);

    this->ui->spinBox_thrusterEnable->setEnabled(editable);
    this->ui->spinBox_GenNO->setEnabled(editable);
    this->ui->spinBox_channel->setEnabled(editable);
    this->ui->btn_setThrusterConfig->setEnabled(editable);
    this->ui->btn_undoThrusterConfig->setEnabled(editable);

    this->ui->PointName_display->setEnabled(editable);
    this->ui->btn_setToAllPoints->setEnabled(editable);
    this->ui->btn_setPointPara->setEnabled(editable);
    this->ui->btn_PLC2Editting_WeldPoint->setEnabled(editable);
    this->ui->spinBox_weldPointEnable->setEnabled(editable);
    this->ui->spinBox_thrusterNOWPC->setEnabled(editable);
    this->ui->spinBox_stepNO_WPC->setEnabled(editable);
    this->ui->comboBox_barcode_assignment->setEnabled(editable);
    this->ui->spinBox_pointPara_amp->setEnabled(editable);
    this->ui->spinBox_pointPara_weldTime->setEnabled(editable);
    this->ui->spinBox_pointPara_weldEnergy->setEnabled(editable);
    this->ui->spinBox_pointPara_peakPower->setEnabled(editable);
    this->ui->spinBox_pointPara_holdTime->setEnabled(editable);
    this->ui->spinBox_pointPara_trigAmp->setEnabled(editable);
    this->ui->spinBox_pointPara_trigPower->setEnabled(editable);
    this->ui->spinBox_pointPara_trigTimeout->setEnabled(editable);
    this->ui->spinBox_pointPara_trigByPower->setEnabled(editable);
    this->ui->spinbox_pointPara_freeRunFreq->setEnabled(editable);
    this->ui->spinbox_pointPara_freq_u_limit->setEnabled(editable);
    this->ui->spinbox_pointPara_freq_l_limit->setEnabled(editable);
    this->ui->spinBox_pointPara_rampupTime->setEnabled(editable);
    this->ui->spinBox_pointPara_rampdownTime->setEnabled(editable);
    this->ui->spinBox_pointPara_afterburst_duration->setEnabled(editable);
    this->ui->spinBox_pointPara_afterburstDelay->setEnabled(editable);
    this->ui->spinBox_pointPara_downPressure->setEnabled(editable);

    this->ui->checkBox_LimitsEnable_BAD->setEnabled(editable);
    this->ui->checkBox_LimitsEnable_SUSPECT->setEnabled(editable);
    this->ui->spinBox_timeLowerLimit_BAD->setEnabled(editable);
    this->ui->spinBox_timeLowerLimit_Suspect->setEnabled(editable);
    this->ui->spinBox_timeUpperLimit_Suspect->setEnabled(editable);
    this->ui->spinBox_timeUpperLimit_BAD->setEnabled(editable);
    this->ui->spinBox_powerLowerLimit_BAD->setEnabled(editable);
    this->ui->spinBox_powerLowerLimit_Suspect->setEnabled(editable);
    this->ui->spinBox_powerUpperLimit_Suspect_2->setEnabled(editable);
    this->ui->spinBox_powerUpperLimit_BAD->setEnabled(editable);
    this->ui->spinBox_energyLowerLimit_BAD->setEnabled(editable);
    this->ui->spinBox_energyLowerLimit_Suspect->setEnabled(editable);
    this->ui->spinBox_energyUpperLimit_Suspect_2->setEnabled(editable);
    this->ui->spinBox_energyUpperLimit_BAD->setEnabled(editable);

    this->ui->stationPara_HighSpeed->setEnabled(editable);
    this->ui->stationPara_MediumSpeed->setEnabled(editable);
    this->ui->stationPara_LowSpeed->setEnabled(editable);
    this->ui->servoPara_CWCCW->setEnabled(editable);
    this->ui->btn_setServoPara->setEnabled(editable);
    this->ui->btn_undoServoSetting->setEnabled(editable);


    this->ui->stationPara_upperLimit->setEnabled(editable);
    this->ui->stationPara_lowerLimit->setEnabled(editable);
    this->ui->stationPara_setPoint->setEnabled(editable);
    this->ui->stationPara_speedType->setEnabled(editable);
    this->ui->btn_setStationPara->setEnabled(editable);
    this->ui->btn_undoStationPara->setEnabled(editable);


    this->ui->spinBox_stationNO->setEnabled(editable);
    this->ui->btn_undoStepsStationEdit->setEnabled(editable);
    this->ui->btn_setStationToStep->setEnabled(editable);

    this->ui->valveName_lineEdit->setEnabled(editable);
    this->ui->spinBox_valveEnable->setEnabled(editable);
    this->ui->comboBox_valveType->setEnabled(editable);
    this->ui->spinBox_valveActionConfirm->setEnabled(editable);
    this->ui->spinBox_valveStartStep->setEnabled(editable);
    this->ui->spinBox_valveEndStep->setEnabled(editable);
    this->ui->btn_setValveConfig->setEnabled(editable);
    this->ui->btn_undoValveConfig->setEnabled(editable);

    this->ui->groupBox_barcodeSetting->setEnabled(editable);

    this->ui->spinBox_filmFeeder_enable->setEnabled(editable);
    this->ui->spinBox_filmFeeder_speed->setEnabled(editable);
    this->ui->spinBox_filmFeeder_distance->setEnabled(editable);
    this->ui->spinBox_filmFeeder_distance_2->setEnabled(editable);
    this->ui->spinBox_filmFeeder_Interval->setEnabled(editable);
    this->ui->comboBox_filmFeeder_Dir->setEnabled(editable);
    this->ui->btn_Edit2PLC_FilmFeeder->setEnabled(editable);
    this->ui->btn_PLC2Edit_FilmFeeder->setEnabled(editable);


    this->ui->groupBox_partSensorBypass->setEnabled(editable);
    this->ui->btn_Edit2PLC_PartSensor->setEnabled(editable);
    this->ui->btn_PLC2Edit_PartSensor->setEnabled(editable);

    this->ui->groupBox_valveSensorBypass->setEnabled(editable);
    this->ui->btn_Edit2PLC_valveSensor->setEnabled(editable);
    this->ui->btn_PLC2Edit_valveSensor->setEnabled(editable);
}

void MainWindow::on_pushButton_language_clicked()
{

   this->LanguageFlag+=1;
   this->LanguageFlag%=2;
   qDebug()<<"language flag:"<<this->LanguageFlag;
   QTranslator translator;
   bool loadLangFileResult;
   if(LanguageFlag ==0)
          loadLangFileResult=translator.load("CR519_CN.qm");
   else if(LanguageFlag ==1)
          loadLangFileResult=translator.load("CR519_EN.qm");
   qDebug()<<tr("load file result:%1,fileName:%2").arg(loadLangFileResult?"OK":"failed").arg(LanguageFlag==0?"CR519_CN.qm":"CR519_EN.qm");
   bool installTranslatorResult;
   installTranslatorResult=qApp->installTranslator(&translator);
   qDebug()<<tr("install translator result result:%1").arg(installTranslatorResult?"OK":"failed");
   if(installTranslatorResult)
   {

       //if succeed to install new translater, then update "sysRegText.txt" with "sysRegText_CN.txt"
       // or "sysRegText_EN.txt",then next startup software will use the correct language
       ui->retranslateUi(this);
       if(LanguageFlag ==0)
       {
           this->ui->pushButton_language->setText("语言");
           if(QFile::exists("sysRegText_CN.txt"))
           {
               QFile loadFile("sysRegText_CN.txt");
               QString textLoaded;
               if(loadFile.open(QIODevice::ReadOnly))
               {
                   //load chinese text to systemRegisteredTextList then update text list on HMI
                   QDataStream in1(&loadFile);

                   if(loadFile.size()>0)
                   {
                       this->systemRegisteredTextList.clear();
                       in1>>textLoaded>>this->systemRegisteredTextList;
                       QList<quint32> keys=this->systemRegisteredTextList.keys();
                       QString itemText;
                       QString allRegistedText;
                       for(int i=0;i<keys.size();i++)
                       {
                          itemText=tr("ID:%1,Text:%2 \n").arg(keys.at(i)).arg(this->systemRegisteredTextList[keys.at(i)]);
                          allRegistedText.append(itemText);
                       }
                       this->ui->textEdit_sysRegistedText->setText(allRegistedText);
                   }
                   //reflush "sysRegText.txt" with "sysRegText_CN.txt"'s contents
                   if(QFile::exists("sysRegText.txt"))
                   {
                       QFile loadFile2("sysRegText.txt");
                       if(loadFile2.open(QIODevice::WriteOnly))
                       {
                           QDataStream out1(&loadFile2);
                           out1<<textLoaded<<this->systemRegisteredTextList;
                           loadFile2.close();
                       }
                   }
               loadFile.close();
               }

           }
       }
       else if (LanguageFlag ==1)
       {
           this->ui->pushButton_language->setText("Language");
           if(QFile::exists("sysRegText_EN.txt"))
           {
               QFile loadFile("sysRegText_EN.txt");
               QString textLoaded;
               if(loadFile.open(QIODevice::ReadOnly))
               {
                   //load English text to systemRegisteredTextList, then update display on HMI
                   QDataStream in1(&loadFile);

                   if(loadFile.size()>0)
                   {
                       this->systemRegisteredTextList.clear();
                       in1>>textLoaded>>this->systemRegisteredTextList;
                       QList<quint32> keys=this->systemRegisteredTextList.keys();
                       QString itemText;
                       QString allRegistedText;
                       for(int i=0;i<keys.size();i++)
                       {
                          itemText=tr("ID:%1,Text:%2 \n").arg(keys.at(i)).arg(this->systemRegisteredTextList[keys.at(i)]);
                          allRegistedText.append(itemText);
                       }
                       this->ui->textEdit_sysRegistedText->setText(allRegistedText);
                   }
                   //reflush "sysRegText.txt" with "sysRegText_EN.txt"'s contents
                   if(QFile::exists("sysRegText.txt"))
                   {
                       QFile loadFile2("sysRegText.txt");
                       if(loadFile2.open(QIODevice::WriteOnly))
                       {
                           QDataStream out1(&loadFile2);
                           out1<<textLoaded<<this->systemRegisteredTextList;
                           loadFile2.close();
                       }
                   }
               loadFile.close();
               }

           }

       }

   }


}

void MainWindow::on_pushButton_3_clicked()
{
    this->ui->textEdit_weldByManual->clear();
}

void MainWindow::on_pushButton_clearHistoryAlarm_clicked()
{
    this->ui->textEdit_historyAlarm->clear();
}

void MainWindow::on_btn_partCounterReset_clicked()
{
    //clear part counters
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x80;//commandNO low byte,128
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte

    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}


void MainWindow::on_tab_stepStation_currentChanged(int index)
{
    if(index!=0)
    {
        if(this->ui->servo_Jog1_btn->isChecked()||this->ui->servo_Jog2_btn->isChecked()||this->ui->servo_home_btn->isChecked())
        {
            this->ui->tab_stepStation->setCurrentIndex(0);
        }
    }
}

void MainWindow::on_tab_toolingConfig_currentChanged(int index)
{
    switch (index) {
    case 6:
    {
        //this->clsBarcode_left->isOpen()
        this->switchItemOnOff(this->ui->LED_serialPort_left,this->clsBarcode_left->isOpen());
        this->switchItemOnOff(this->ui->LED_serialPort_right,this->clsBarcode_right->isOpen());
        break;
    }

    default:
        break;
    }
}


void MainWindow::on_btn_historyData_clicked()
{
    this->ui->stackedWidget_run->setCurrentIndex(2);    
    this->ui->dateTimeEdit_startTime->setDateTime(QDateTime::currentDateTime().addDays(-1));
    this->ui->dateTimeEdit_endTime->setDateTime(QDateTime::currentDateTime().addDays(1));
}

void MainWindow::on_pushButton_chooseDatabase_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "./",
                                                    tr("Sqlite3 (*.sqlite3)"));
    if(!fileName.isEmpty())
    {
        //this->ui->toolImageUrl->setText(fileName);
        this->ui->lineEdit_selectedDataBase->setText(fileName);
    }

}

void MainWindow::on_pushButton_historyCycleData_search_clicked()
{
    if(this->ui->lineEdit_selectedDataBase->text().isEmpty())
        return;
    DB_Handler dbh_2;
    dbh_2.onInit("QSQLITE","queryPartHistoryData",this->ui->lineEdit_selectedDataBase->text());

    connect(&dbh_2,&DB_Handler::logRequest,this,&MainWindow::execLogging);

    QString sql;
    if(this->ui->tabWidget_getHistoryCycleData->currentIndex()==0)
    {
        //query by barcode
        sql=tr("select partHistoryCycleData.CreatedTime, partHistoryCycleData.partBarcode,partHistoryCycleData.partWeldResult,partHistoryCycleData.toolID,partHistoryCycleData.toolName,partHistoryCycleData.location from partHistoryCycleData where partHistoryCycleData.partBarcode=\"%1\"")
                .arg(this->ui->lineEdit_getHistoryCycleData_Barcode->text());
        this->ui->textEdit_sql->setText(sql);
    }
    else if(this->ui->tabWidget_getHistoryCycleData->currentIndex()==1)
    {
        //query by time,"2000/1/1 0:00" to "2000-01-01 hh:mm:ss"
        QDateTime dt1=QDateTime::fromString(this->ui->dateTimeEdit_startTime->text(),"yyyy/M/d h:mm");
        QString startTime=dt1.toString("yyyy-MM-dd hh:mm:ss");
        QDateTime dt2=QDateTime::fromString(this->ui->dateTimeEdit_endTime->text(),"yyyy/M/d h:mm");
        QString endTime=dt2.toString("yyyy-MM-dd hh:mm:ss");
        qDebug()<<"date format,startTime:"<<startTime;
        qDebug()<<"date format,startTime:"<<endTime;
        sql=tr("select partHistoryCycleData.CreatedTime, partHistoryCycleData.partBarcode,partHistoryCycleData.partWeldResult,partHistoryCycleData.toolID,partHistoryCycleData.toolName,partHistoryCycleData.location from partHistoryCycleData where CreatedTime between datetime(\'%1\') and datetime(\'%2\')")
                .arg(startTime).arg(endTime);
        this->ui->textEdit_sql->setText(sql);
    }
    else if(this->ui->tabWidget_getHistoryCycleData->currentIndex()==2)
    {
        //query by SQL statement directly
        sql=this->ui->textEdit_sql->toPlainText();
    }
    dbh_2.q1.exec(sql);
    qDebug()<<tr("sql statement:%1,lastError:%2").arg(sql).arg(dbh_2.q1.lastError().text());
    if(dbh_2.q1.isActive())
    {
             qDebug()<<"sql executed successfuly,q1 is active";
             //
             this->model_partData->setQuery(dbh_2.q1);
             this->model_partData->setHeaderData(0, Qt::Horizontal, tr("dateTime"));
             this->model_partData->setHeaderData(1, Qt::Horizontal, tr("barcode"));
             this->model_partData->setHeaderData(2, Qt::Horizontal, tr("weldResult"));
             this->model_partData->setHeaderData(3, Qt::Horizontal, tr("toolID"));
             this->model_partData->setHeaderData(4, Qt::Horizontal, tr("toolName"));
             this->model_partData->setHeaderData(5, Qt::Horizontal, tr("location"));
             //QTableView *view = new QTableView;
             //view->setModel(model);
             this->ui->tableView_partCycleData->setModel(this->model_partData);
             this->ui->tableView_partCycleData->show();

//             while(dbh_2.q1.next())
//             {
//                 qDebug()<<tr("dateTime:%1,barcode:%2,weldResult:%3,toolID:%4,toolName:%5,location:%6")
//                           .arg(dbh_2.q1.value(0).toString()).arg(dbh_2.q1.value(1).toString())
//                           .arg(dbh_2.q1.value(2).toString()).arg(dbh_2.q1.value(3).toInt())
//                           .arg(dbh_2.q1.value(4).toString()).arg(dbh_2.q1.value(5).toString());
//             }
    }
    else
    {
        if(loggingEnable&&loggingLevel>0)
        {
            QString logcontents=tr("sql statement:%1,lastError:%2").arg(sql).arg(dbh_2.q1.lastError().text());
            emit this->logRequest(logcontents,200,0);
        }
    }
    //close database
    dbh_2.database1.close();
    disconnect(&dbh_2,&DB_Handler::logRequest,this,&MainWindow::execLogging);
}
void MainWindow::on_pushButton_historyCycleData_export_clicked()
{
    //if database not selected, inform to select data base first
    if(this->ui->lineEdit_selectedDataBase->text().isEmpty())
    {
        QMessageBox::information(this, tr("database not specified"), tr("pls. select database first"));
        return;
    }
    //specify CSV file name
    QString fileName = QFileDialog::getSaveFileName(this,tr("Export Cycle Data"),tr("./"),tr("CSV (*.csv *.CSV)"));
    if(fileName.isEmpty())
        return;
    //contruct CSV headers then write it into file
    QStringList cycleDataStrList_header;
    cycleDataStrList_header<<"dateTime"<<"barcode"<<"partWeldResult"<<"toolID"<<"toolName"<<"location";
    QString pointNOAndName;
    for(int i=1;i<=16;i++)
    {
        pointNOAndName=tr("-Point#%1-%2")
                .arg(i)
                .arg(this->tempTooling_editting->pointNameMapping.at(i));
        cycleDataStrList_header<<tr("pointWeldResult%1").arg(pointNOAndName)
                               <<tr("amplitude(\%)%1").arg(pointNOAndName)
                               <<tr("pressure(Kpa)%1").arg(pointNOAndName)
                               <<tr("weldTime(ms)%1").arg(pointNOAndName)
                               <<tr("peakPower(W)%1").arg(pointNOAndName)
                               <<tr("weldEnergy(J)%1").arg(pointNOAndName)
                               <<tr("holdTime(ms)%1").arg(pointNOAndName);
    }
    qDebug()<<"csv header ready,going to write them into CSV";
    QFile CSVFile(fileName);
    QTextStream out1(&CSVFile);
    //write header to CSV
    if(CSVFile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
       for(int i=0;i<cycleDataStrList_header.size();i++)
       {
           out1<<cycleDataStrList_header.at(i)<<',';
       }
       //insert line terminator
       out1<<'\n';
    }
    //if failed to open CSV,return
    else
    {
        QMessageBox::information(this, tr("open file failed"), tr("can not open file,failed to set header for CSV"));
        return;
    }
    qDebug()<<"write csv header done";
    //constrct CSV cycle data record
    //It's important to init stringList before access it by "[]"

    QStringList emptyStrList;
    for(int i=0;i<cycleDataStrList_header.size();i++)
    {
        emptyStrList<<" ";
    }
    QStringList lastCycleDataStrList;
    QString selectedBarcode,sql;
    DB_Handler dbh_3;
    quint8 currentPointNO;
    dbh_3.onInit("QSQLITE","queryPointsHistoryData",this->ui->lineEdit_selectedDataBase->text());
    qDebug()<<"database init done";
    connect(&dbh_3,&DB_Handler::logRequest,this,&MainWindow::execLogging);
    for(int rowNO=0;rowNO<this->model_partData->rowCount();rowNO++)
    {
        //init strLIst with blank charactar everytime
        lastCycleDataStrList=emptyStrList;
        qDebug()<<tr("this->model_partData,rowCount:%1,columnCount:%2,currentRowNO:%3")
                  .arg(this->model_partData->rowCount()).arg(this->model_partData->columnCount()).arg(rowNO);
        //construct data record with part data fields:dateTime,barcode,result,toolID,toolName,location
        lastCycleDataStrList[0]=this->model_partData->data(this->model_partData->index(rowNO,0)).toString();
        lastCycleDataStrList[1]=this->model_partData->data(this->model_partData->index(rowNO,1)).toString();
        lastCycleDataStrList[2]=this->model_partData->data(this->model_partData->index(rowNO,2)).toString();
        lastCycleDataStrList[3]=this->model_partData->data(this->model_partData->index(rowNO,3)).toString();
        lastCycleDataStrList[4]=this->model_partData->data(this->model_partData->index(rowNO,4)).toString();
        lastCycleDataStrList[5]=this->model_partData->data(this->model_partData->index(rowNO,5)).toString();
        //try to get point data from table "pointHistoryCycleData" in database
        selectedBarcode=this->model_partData->data(this->model_partData->index(rowNO,1)).toString();
        qDebug()<<tr("will query by barcode,barcode:%1,rowNO:%2").arg(selectedBarcode).arg(rowNO);
        if(selectedBarcode.isEmpty())
            continue;
        //query by barcode
        sql=tr("select pointHistoryCycleData.pointNO,pointHistoryCycleData.pointName,pointHistoryCycleData.pointWeldResult,pointHistoryCycleData.amplitude, pointHistoryCycleData.pressure,pointHistoryCycleData.weldTime,pointHistoryCycleData.peakPower,pointHistoryCycleData.weldEnergy, pointHistoryCycleData.holdTime from pointHistoryCycleData where pointHistoryCycleData.Barcode=\"%1\"")
                .arg(selectedBarcode);

        dbh_3.q1.exec(sql);
        //if suceed to get data from database , then construct cycle data with the point data
        if(dbh_3.q1.isActive())
        {
            while(dbh_3.q1.next())
             {
                 qDebug()<<tr("pointNO:%1,pointName:%2,weldResult:%3,amplitude:%4,pressure:%5,weldTime:%6,peakPower:%7,energy:%8,holdTime:%9")
                           .arg(dbh_3.q1.value(0).toInt()).arg(dbh_3.q1.value(1).toString())
                           .arg(dbh_3.q1.value(2).toString()).arg(dbh_3.q1.value(3).toInt())
                           .arg(dbh_3.q1.value(4).toInt()).arg(dbh_3.q1.value(5).toInt())
                           .arg(dbh_3.q1.value(6).toInt()).arg(dbh_3.q1.value(7).toInt())
                           .arg(dbh_3.q1.value(8).toInt());
                 currentPointNO=dbh_3.q1.value(0).toInt();
                 if(currentPointNO>0)
                 {
                     lastCycleDataStrList[(currentPointNO-1)*7+6]=dbh_3.q1.value(2).toString();
                     lastCycleDataStrList[(currentPointNO-1)*7+7]=dbh_3.q1.value(3).toString();
                     lastCycleDataStrList[(currentPointNO-1)*7+8]=dbh_3.q1.value(4).toString();
                     lastCycleDataStrList[(currentPointNO-1)*7+9]=dbh_3.q1.value(5).toString();
                     lastCycleDataStrList[(currentPointNO-1)*7+10]=dbh_3.q1.value(6).toString();
                     lastCycleDataStrList[(currentPointNO-1)*7+11]=dbh_3.q1.value(7).toString();
                     lastCycleDataStrList[(currentPointNO-1)*7+12]=dbh_3.q1.value(8).toString();

                 }
             }
        }
        else
        {
            if(loggingEnable&&loggingLevel>0)
            {
                QString logcontents=tr("sql statement:%1,lastError:%2").arg(sql).arg(dbh_3.q1.lastError().text());
                emit this->logRequest(logcontents,200,0);
            }
        }
        qDebug()<<"data ready,going to write them into CSV file";
        //write data into CSV
        for(int j=0;j<lastCycleDataStrList.size();j++)
        {
            out1<<lastCycleDataStrList.at(j)<<',';
        }
        out1<<'\n';
    }
    //close file
    CSVFile.close();
    //disconnect signal slot
    disconnect(&dbh_3,&DB_Handler::logRequest,this,&MainWindow::execLogging);
    //keep the last sql statement for later check in case  needed
    this->ui->textEdit_sql->setText(sql);
    //notify the operator the exportting is done
    QMessageBox::information(this, tr("data exportting"), tr("data exportting done"));
    return;
}

void MainWindow::on_tableView_partCycleData_clicked(const QModelIndex &index)
{
    if(this->ui->lineEdit_selectedDataBase->text().isEmpty())
        return;
    QString selectedBarcode=this->model_partData->data(this->model_partData->index(index.row(),1)).toString();

    DB_Handler dbh_3;
    dbh_3.onInit("QSQLITE","queryPointsHistoryData",this->ui->lineEdit_selectedDataBase->text());
    connect(&dbh_3,&DB_Handler::logRequest,this,&MainWindow::execLogging);
    QString sql;
    //query by barcode
    sql=tr("select pointHistoryCycleData.pointNO,pointHistoryCycleData.pointName,pointHistoryCycleData.pointWeldResult,pointHistoryCycleData.amplitude, pointHistoryCycleData.pressure,pointHistoryCycleData.weldTime,pointHistoryCycleData.peakPower,pointHistoryCycleData.weldEnergy, pointHistoryCycleData.holdTime from pointHistoryCycleData where pointHistoryCycleData.Barcode=\"%1\"")
            .arg(selectedBarcode);
    this->ui->textEdit_sql->setText(sql);

    dbh_3.q1.exec(sql);
    qDebug()<<tr("sql statement:%1,lastError:%2").arg(sql).arg(dbh_3.q1.lastError().text());
    if(dbh_3.q1.isActive())
    {
             qDebug()<<"sql executed successfuly,q1 is active";
             //
             this->model_pointsData->setQuery(dbh_3.q1);
             this->model_pointsData->setHeaderData(0, Qt::Horizontal, tr("pointNO"));
             this->model_pointsData->setHeaderData(1, Qt::Horizontal, tr("pointName"));
             this->model_pointsData->setHeaderData(2, Qt::Horizontal, tr("weldResult"));
             this->model_pointsData->setHeaderData(3, Qt::Horizontal, tr("amplitude(%)"));
             this->model_pointsData->setHeaderData(4, Qt::Horizontal, tr("pressure(Kpa)"));
             this->model_pointsData->setHeaderData(5, Qt::Horizontal, tr("weldTime(ms)"));
             this->model_pointsData->setHeaderData(6, Qt::Horizontal, tr("peakPower(W)"));
             this->model_pointsData->setHeaderData(7, Qt::Horizontal, tr("weldEnergy(J)"));
             this->model_pointsData->setHeaderData(8, Qt::Horizontal, tr("holdTime(ms)"));
             //QTableView *view = new QTableView;
             //view->setModel(model);
             this->ui->tableView_pointCycleData->setModel(this->model_pointsData);
             this->ui->tableView_pointCycleData->show();

//             while(dbh_3.q1.next())
//             {
//                 qDebug()<<tr("pointNO:%1,pointName:%2,weldResult:%3,amplitude:%4,pressure:%5,weldTime:%6,peakPower:%7,energy:%8,holdTime:%9")
//                           .arg(dbh_3.q1.value(0).toInt()).arg(dbh_3.q1.value(1).toString())
//                           .arg(dbh_3.q1.value(2).toString()).arg(dbh_3.q1.value(3).toInt())
//                           .arg(dbh_3.q1.value(4).toInt()).arg(dbh_3.q1.value(5).toInt())
             //              .arg(dbh_3.q1.value(6).toInt()).arg(dbh_3.q1.value(7).toInt())
             //              .arg(dbh_3.q1.value(8).toInt());
//             }
    }
    else
    {
        if(loggingEnable&&loggingLevel>0)
        {
            QString logcontents=tr("sql statement:%1,lastError:%2").arg(sql).arg(dbh_3.q1.lastError().text());
            emit this->logRequest(logcontents,200,0);
        }
    }
    //disconnect signal slot

    disconnect(&dbh_3,&DB_Handler::logRequest,this,&MainWindow::execLogging);

}

void MainWindow::on_pushButton_MachineInfo_login_clicked()
{
    quint16 calculatedPassword=quint16(2359)-(QTime::currentTime().hour()*100+QTime::currentTime().minute())
            +this->ui->spinbox_programmerID->value();
    if(this->ui->spinBox_programmerPassword->value()==calculatedPassword)
    {
        this->ui->groupBox_machineInfo->setEnabled(true);
        //this->ui->groupBox_textReg->setEnabled(true);
        this->ui->groupBox_textReg->setVisible(true);
    }
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit();
}


void MainWindow::on_btn_IO_Table_Previos_clicked()
{
    quint8 index_IO_Table=this->ui->tabWidget_IO_Table->currentIndex();
    if(index_IO_Table==0)
    {
        //DO
        quint8 currentIndex=this->ui->stackedWidget_DO->currentIndex();
        if(currentIndex>0)
        {
            this->ui->stackedWidget_DO->setCurrentIndex(currentIndex-1);
        }
    }
    else if(index_IO_Table==1)
    {
        //DI
        quint8 currentIndex=this->ui->stackedWidget_DI->currentIndex();
        if(currentIndex>0)
        {
            this->ui->stackedWidget_DI->setCurrentIndex(currentIndex-1);
        }
    }
    else if(index_IO_Table==2)
    {
        //other

    }
}

void MainWindow::on_btn_IO_Table_Next_clicked()
{
    quint8 index_IO_Table=this->ui->tabWidget_IO_Table->currentIndex();
    if(index_IO_Table==0)
    {
        //DO
        quint8 currentIndex=this->ui->stackedWidget_DO->currentIndex();
        if(currentIndex<2)
        {
            this->ui->stackedWidget_DO->setCurrentIndex(currentIndex+1);
        }
    }
    else if(index_IO_Table==1)
    {
        //DI
        quint8 currentIndex=this->ui->stackedWidget_DI->currentIndex();
        if(currentIndex<3)
        {
            this->ui->stackedWidget_DI->setCurrentIndex(currentIndex+1);
        }
    }
    else if(index_IO_Table==2)
    {
        //other

    }

}

void MainWindow::on_btn_realTimeData_clicked()
{
    this->ui->stackedWidget_run->setCurrentIndex(3);
}

void MainWindow::on_WeldByManual_valveNO_valueChanged(int arg1)
{
    this->ui->WeldByManual_valveName->setText(this->tempTooling_editting->valveNameMapping.at(arg1));
}
