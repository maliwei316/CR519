#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <bitsoperation.h>
#include <QTime>
#include <QFileDialog>
#include <clstooling.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->wp1=nullptr;
    this->tempTooling_editting=new clsTooling(this);
    this->tooling_current=new clsTooling(this);
    this->toolID_editing=1;
    this->tempTooling_editting->plcToolingInfo.toolingNO=1;
    this->toolID_PLC=0;
    this->setPLCValueVisible(false);
    this->uploadingWholeSettingFromPLCInProcess=false;
    this->timer_mainWindow.setInterval(10000);
    connect(&this->timer_mainWindow,&QTimer::timeout,this,&MainWindow::OnTimer_mainWindow_Timeout);
    this->timer_mainWindow.start();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete wp1;
    //tempTooling_editting->deleteLater();
    //tooling_current->deleteLater();

}
void MainWindow::OnTimer_mainWindow_Timeout()
{
    qDebug()<<tr("checking tcp comm connection status,current status:%1").
              arg(this->checking_tcpConnectionStatus?"OFFLINE":"ONLINE");
    if(this->checking_tcpConnectionStatus)
    {
        this->tcpConnectionStatus_receive=false;
        this->tcpConnectionStatus_send=false;
        this->setPLCValueVisible(false);
        this->ui->OnOffLine_Label->setText("OFFLINE");
    }
    else
    {

      this->checking_tcpConnectionStatus=true;
      emit this->checkTcpConnectionStatus();
    }
}
void MainWindow::OnTcpCommConnectionStateChanged(QAbstractSocket::SocketState state,quint8 ConnectionID)
{

    qDebug()<<tr("received new state, state:%1,ConnectionID:%2").arg(state).arg(ConnectionID);
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
        this->ui->OnOffLine_Label->setText("OFFLINE");
        this->setPLCValueVisible(false);
    }

    else
    {
        //get toolID from PLC
        QByteArray dataToTcpCommObj;
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7A;//commandNO low byte,122
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//reserve byte
        dataToTcpCommObj[7]=0x00;//reserve byte
        dataToTcpCommObj[8]=0x00;//reserve byte
        dataToTcpCommObj[9]=0x00;//reserve byte
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
        //set label text to online
        this->ui->OnOffLine_Label->setText("ONLINE");
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

        this->tooling_current->plcToolingInfo.generator_enable[1]=bb1.bits.b1;
        this->tooling_current->plcToolingInfo.generator_enable[2]=bb1.bits.b2;
        this->tooling_current->plcToolingInfo.generator_enable[3]=bb1.bits.b3;
        this->tooling_current->plcToolingInfo.generator_enable[4]=bb1.bits.b4;

        this->ui->CV_GenEnable_1->setChecked(bb1.bits.b1);
        this->ui->CV_GenEnable_2->setChecked(bb1.bits.b2);
        this->ui->CV_GenEnable_3->setChecked(bb1.bits.b3);
        this->ui->CV_GenEnable_4->setChecked(bb1.bits.b4);
        if(this->uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.generator_enable[1]=bb1.bits.b1;
            this->tempTooling_editting->plcToolingInfo.generator_enable[2]=bb1.bits.b2;
            this->tempTooling_editting->plcToolingInfo.generator_enable[3]=bb1.bits.b3;
            this->tempTooling_editting->plcToolingInfo.generator_enable[4]=bb1.bits.b4;
            this->ui->checkBox_GenEnable_1->setChecked(bb1.bits.b1);
            this->ui->checkBox_GenEnable_2->setChecked(bb1.bits.b2);
            this->ui->checkBox_GenEnable_3->setChecked(bb1.bits.b3);
            this->ui->checkBox_GenEnable_4->setChecked(bb1.bits.b4);
        }

    }

    case 2://received gen realtime data, update display area accorddingly
    {
        if(dataLoad[1]==this->ui->spinBox_PointNO_pointPara->value())
        {
            this->ui->UStest_genNO->setValue(dataLoad[0]);
            dWordBytes dw1;
            dw1.bytesVar.B0=dataLoad[2];
            dw1.bytesVar.B1=dataLoad[3];
            dw1.bytesVar.B2=dataLoad[4];
            dw1.bytesVar.B3=dataLoad[5];
            this->ui->CV_pointPara_realTime_Freq->setValue((quint32)dw1.DWordVar);
            this->ui->CV_pointPara_realTime_Amp->setValue(dataLoad[8]);
            this->ui->CV_pointPara_realTime_Power->setValue((quint8)dataLoad[6]*256+dataLoad[7]);
            this->ui->usTest_Channel->setValue((quint8)dataLoad[9]);
        }

    }
    break;
    case 3://received servo realtime info , update display area
    {
        dWordBytes dw1;
        qDebug()<<"received servo realtime info";
        dw1.bytesVar.B0=dataLoad[2];
        dw1.bytesVar.B1=dataLoad[3];
        dw1.bytesVar.B2=dataLoad[4];
        dw1.bytesVar.B3=dataLoad[5];
        this->ui->weldByManual_tartgetPos->setText(QString::number((qint32)dw1.DWordVar));
        dw1.bytesVar.B0=dataLoad[6];
        dw1.bytesVar.B1=dataLoad[7];
        dw1.bytesVar.B2=dataLoad[8];
        dw1.bytesVar.B3=dataLoad[9];
        this->ui->weldByManual_realtimeSpeed->setText(QString::number((qint16)dw1.DWordVar));
        this->ui->servo_realTimeSpeed->setText(QString::number((qint16)dw1.DWordVar));
        dw1.bytesVar.B0=dataLoad[10];
        dw1.bytesVar.B1=dataLoad[11];
        dw1.bytesVar.B2=dataLoad[12];
        dw1.bytesVar.B3=dataLoad[13];
        this->ui->weldByManual_realtimePos->setText(QString::number((qint32)dw1.DWordVar));
        this->ui->servo_realTimePos->setText(QString::number((qint32)dw1.DWordVar));
        this->ui->weldByManual_currentStationNO->setValue(dataLoad[0]);
        this->ui->weldByManual_targetStationNO->setValue(dataLoad[1]);

    }
        break;

    case 6://get thruster config info from PLC, then update the display value
    {
        qDebug()<<tr("CommandNO:%1,get thruster config info, thruster NO:%2").arg(command).arg((quint8)dataLoad[0]);
        quint8 thrusterNOFromPLC;
        thrusterNOFromPLC=dataLoad[0];
        //update display if thruster NO is desired
        if(thrusterNOFromPLC==this->ui->spinBox_thrusterNO->value())
        {
            this->ui->CV_thrusterEnable->setValue(dataLoad[3]);
            this->ui->CV_GenNO->setValue(dataLoad[1]);
            this->ui->CV_Channel->setValue(dataLoad[2]);
            if(this->uploadingWholeSettingFromPLCInProcess)
            {
                this->ui->spinBox_thrusterEnable->setValue(dataLoad[3]);
                this->ui->spinBox_GenNO->setValue(dataLoad[1]);
                this->ui->spinBox_channel->setValue(dataLoad[2]);
            }
        }
        //store any received thruster info into tooling info on background
        this->tooling_current->plcToolingInfo.thruster_List[thrusterNOFromPLC].enable=dataLoad[3];
        this->tooling_current->plcToolingInfo.thruster_List[thrusterNOFromPLC].GenNO=dataLoad[1];
        this->tooling_current->plcToolingInfo.thruster_List[thrusterNOFromPLC].ChannelNO=dataLoad[2];
        if(this->uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.thruster_List[thrusterNOFromPLC].enable=dataLoad[3];
            this->tempTooling_editting->plcToolingInfo.thruster_List[thrusterNOFromPLC].GenNO=dataLoad[1];
            this->tempTooling_editting->plcToolingInfo.thruster_List[thrusterNOFromPLC].ChannelNO=dataLoad[2];
        }
        break;
    }
    case 7://get weld point config info from PLC, then update the display value
    {
        quint8 weldPointNOFromPLC;
        weldPointNOFromPLC=dataLoad[0];
        //update display if point NO is desired
        if(weldPointNOFromPLC==this->ui->spinBox_PointNO_pointPara->value())
        {
            this->ui->CV_weldPointEnable->setValue(dataLoad[3]);
            this->ui->CV_thrusterNO_WPC->setValue(dataLoad[1]);
            this->ui->CV_stepNO_WPC->setValue(dataLoad[2]);
            if(this->uploadingWholeSettingFromPLCInProcess)
            {
                this->ui->spinBox_weldPointEnable->setValue(dataLoad[3]);
                this->ui->spinBox_thrusterNOWPC->setValue(dataLoad[1]);
                this->ui->spinBox_stepNO_WPC->setValue(dataLoad[2]);
            }
        }
        //store any received point info into tooling info on background
        this->tooling_current->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].enable=dataLoad[3];
        this->tooling_current->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].ThrusterNO=dataLoad[1];
        this->tooling_current->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].stepNO=dataLoad[2];
        if(this->uploadingWholeSettingFromPLCInProcess)
        {
            this->tempTooling_editting->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].enable=dataLoad[3];
            this->tempTooling_editting->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].ThrusterNO=dataLoad[1];
            this->tempTooling_editting->plcToolingInfo.weldPoint_List[weldPointNOFromPLC].stepNO=dataLoad[2];
        }
        break;
    }
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
        this->toolID_PLC=dataLoad[3];
        this->ui->toolID_fromPLC->setValue(this->toolID_PLC);
        this->tooling_current->plcToolingInfo.toolingNO=this->toolID_PLC;
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
        this->ui->CV_pointPara_upPressure->setVisible(true);
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
        this->ui->CV_pointPara_upPressure->setVisible(false);
        this->ui->groupBox_PLC_Value_limits->setVisible(false);
        //enable/disable servo  operarion
        this->ui->tab_stepStation->setTabEnabled(0,false);
        this->ui->btn_QuerystationPara->setVisible(false);
        this->ui->stationConfig_servo_GOTO->setVisible(false);
        this->ui->btn_setStationPara->setVisible(false);
        this->ui->btn_undoStationPara->setVisible(false);

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

        this->ui->PLC_Value_Label_10->setVisible(false);
        this->ui->CV_valveEnableBypass->setVisible(false);
        this->ui->comboBox_ValveType->setVisible(false);
        this->ui->CV_valveActionConfirm->setVisible(false);
        this->ui->CV_valveStartStep->setVisible(false);
        this->ui->CV_valveEndStep->setVisible(false);
        this->ui->btn_setServoPara->setVisible(false);
        this->ui->btn_undoServoSetting->setVisible(false);
        this->ui->btn_PLC2Editting_WeldPoint->setVisible(false);
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
    this->ui->spinBox_pointPara_upPressure->setValue(wp2.thrusterPressure_up);
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
   this->ui->CV_pointPara_upPressure->setValue(wp2.thrusterPressure_up);
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
    //this->ui->spinBox_thrusterNO->setValue(1);
    this->ui->spinBox_thrusterEnable->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[this->ui->spinBox_thrusterNO->value()].enable);
    this->ui->spinBox_GenNO->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[this->ui->spinBox_thrusterNO->value()].GenNO);
    this->ui->spinBox_channel->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[this->ui->spinBox_thrusterNO->value()].ChannelNO);
    //update weld point page
    //this->ui->spinBox_PointNO_pointPara->setValue(1);
    this->ui->PointName_display->setText(this->tempTooling_editting->pointNameMapping[this->ui->spinBox_PointNO_pointPara->value()]);
    //dispaly ultrasonic pararmeters
    this->updateWeldPoitDisplay_Editting(this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()]);
    //update station setting page
    //this->ui->stationPara_stationNO->setValue(1);
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
    //this->ui->spinBox_stepNO->setValue(1);
    this->ui->spinBox_stationNO->setValue(this->tempTooling_editting->plcToolingInfo.stepStationConnection
                                          [this->ui->spinBox_stepNO->value()]);
    //update valve page
    //this->ui->spinBox_valveNO->setValue(1);
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


}
void MainWindow::updateDisplay_ALL_CurrentValue()
{
    this->ui->CV_GenEnable_1->setChecked(this->tooling_current->plcToolingInfo.generator_enable[1]);
    this->ui->CV_GenEnable_2->setChecked(this->tooling_current->plcToolingInfo.generator_enable[2]);
    this->ui->CV_GenEnable_3->setChecked(this->tooling_current->plcToolingInfo.generator_enable[3]);
    this->ui->CV_GenEnable_4->setChecked(this->tooling_current->plcToolingInfo.generator_enable[4]);
    this->ui->CV_thrusterEnable->setValue(this->tooling_current->plcToolingInfo.thruster_List
                                          [this->ui->spinBox_thrusterNO->value()].enable?1:0);
    this->ui->CV_GenNO->setValue(this->tooling_current->plcToolingInfo.thruster_List
                                 [this->ui->spinBox_thrusterNO->value()].GenNO);
    this->ui->CV_Channel->setValue(this->tooling_current->plcToolingInfo.thruster_List
                                   [this->ui->spinBox_thrusterNO->value()].ChannelNO);

    //display weld point para
    this->updateWeldPoitDisplay_Current(this->tooling_current->plcToolingInfo.weldPoint_List
                                              [this->ui->spinBox_PointNO_pointPara->value()]);
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

    this->ui->CV_stationPara_HighSpeed->setValue(this->tooling_current->plcToolingInfo.servoSpeed_high);
    this->ui->CV_stationPara_MediumSpeed->setValue(this->tooling_current->plcToolingInfo.servoSpeed_medium);
    this->ui->CV_stationPara_lowSpeed->setValue(this->tooling_current->plcToolingInfo.servoSpeed_low);
    //this->ui->CV_stationPara_defaultSpeed->setValue(this->tooling_current->plcToolingInfo.defaultSpeed);
    this->ui->CV_servoPara_CWCCW->setCurrentIndex(this->tooling_current->plcToolingInfo.servoHomingDir?1:0);
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
    dataToTcpCommObj[8]=this->ui->spinBox_channel->value();
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

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_QueryThrusterConfig_2_clicked()
{
    //query thruster config
    qDebug()<<"query thruster config btn_2 clicked";

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
        wp1->thrusterPressure_down=this->ui->spinBox_pointPara_downPressure->value();
        wp1->thrusterPressure_up=this->ui->spinBox_pointPara_upPressure->value();
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
    if(this->tcpConnectionStatus_send&&this->tcpConnectionStatus_receive)
        emit this->sendDataToTCPCommObj(dataToTcpCommObj);
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
    this->ui->stackedWidget->setCurrentIndex(0);
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



void MainWindow::on_actionWeld_By_Manual_triggered()
{
    this->ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_WeldByManual_PointNO_valueChanged(int arg1)
{
    //this->ui->weldByManual_Down->setEnabled(false);
    //this->ui->weldByManual_runWeld->setEnabled(false);
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

void MainWindow::on_servo_Jog1_btn_pressed()
{

}

void MainWindow::on_servo_Jog1_btn_released()
{

}

void MainWindow::on_servo_Jog2_btn_pressed()
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

void MainWindow::on_servo_Jog2_btn_released()
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

void MainWindow::on_servo_home_btn_pressed()
{

}

void MainWindow::on_servo_home_btn_released()
{

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
    dataToTcpCommObj[1]=0x0A;//length low byte
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
    QString fileName="toolingConfig_"+QString::number(this->ui->toolID_Editting->value())+".dc";
    if(QFile::exists(fileName))
    {
        qDebug()<<"found config file,will load it ";
        //load config file, to do
        if(this->tempTooling_editting->loadFromDisk(fileName))
        {
          qDebug()<<"load done,file name:"<<fileName;
          this->updateDisplay_All_EdittingValue();
          this->ui->toolName_lineEdit->setText(this->tempTooling_editting->toolingName);
          this->ui->toolImageUrl->setText(this->tempTooling_editting->toolingImageSource);
          if(!this->tempTooling_editting->toolingImageSource.isEmpty())

              this->ui->toolingPicLabel->setPixmap(QPixmap(this->tempTooling_editting->toolingImageSource));

          else
             this->ui->toolingPicLabel->clear();


        }

    }
    else
    {
       qDebug()<<"can not find config file,need create it first";
    }

    //set PLC components and value visible or not
    if((this->toolID_editing!=this->toolID_PLC)||(!this->tcpConnectionStatus_receive)||(!this->tcpConnectionStatus_send))
    {

        this->setPLCValueVisible(false);

    }
    else
    {

        this->setPLCValueVisible(true);
        qDebug()<<"uploading whole setting para from PLC";
        //here upload setting will only update CV values&&tooling_Current,while, if
        // press "upload from PLC" button, temp_tooling_Editting&&Editting value will also be overwritten

        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_uploadWholeSettingFromPLC());
        qDebug()<<"the next 2 sec, any data from PLC will apply to editting value";
    }

}



void MainWindow::on_toolName_lineEdit_textEdited(const QString &arg1)
{
    this->tempTooling_editting->toolingName=arg1;
}

void MainWindow::on_btn_selectToolingImage_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home",
                                                    tr("Images (*.png *.xpm *.jpg)"));
    if(!fileName.isEmpty())
    {
        this->ui->toolImageUrl->setText(fileName);
        this->tempTooling_editting->toolingImageSource=fileName;
        this->ui->toolingPicLabel->setPixmap(QPixmap(fileName));
    }
}

void MainWindow::on_btn_saveToDisk_clicked()
{

   //To DO
    QString fileName="toolingConfig_"+QString::number(this->ui->toolID_Editting->value())+".dc";
    this->tempTooling_editting->saveToDisk(fileName);
}



void MainWindow::on_toolID_fromPLC_valueChanged(int arg1)
{
    if(this->toolID_PLC!=arg1)
        this->toolID_PLC=arg1;
}

void MainWindow::on_checkBox_GenEnable_1_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.generator_enable[1]=arg1;

}

void MainWindow::on_checkBox_GenEnable_2_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.generator_enable[2]=arg1;
}

void MainWindow::on_checkBox_GenEnable_3_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.generator_enable[3]=arg1;
}

void MainWindow::on_checkBox_GenEnable_4_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.generator_enable[4]=arg1;
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

void MainWindow::on_spinBox_pointPara_upPressure_valueChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].thrusterPressure_up=arg1;
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
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.B21_EnableSuspectBadByte.bits.b1=arg1;
}

void MainWindow::on_checkBox_LimitsEnable_SUSPECT_stateChanged(int arg1)
{
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()].ultrasonicPara.B21_EnableSuspectBadByte.bits.b0=arg1;
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

void MainWindow::on_spinBox_PointNO_pointPara_valueChanged(int arg1)
{

    this->ui->PointName_display->setText(this->tempTooling_editting->pointNameMapping[arg1]);
    this->tempTooling_editting->plcToolingInfo.weldPoint_List[arg1].pointNO=arg1;
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
    this->tempTooling_editting->plcToolingInfo.thruster_List[arg1].thrusterNO=arg1;
    this->ui->spinBox_thrusterEnable->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[arg1].enable);
    this->ui->spinBox_GenNO->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[arg1].GenNO);
    this->ui->spinBox_channel->setValue(this->tempTooling_editting->plcToolingInfo.thruster_List[arg1].ChannelNO);
    if(this->toolID_PLC==this->tempTooling_editting->plcToolingInfo.toolingNO && this->toolID_PLC>0)
    {
        this->ui->CV_thrusterEnable->setValue(this->tooling_current->plcToolingInfo.thruster_List[arg1].enable?1:0);
        this->ui->CV_GenNO->setValue(this->tooling_current->plcToolingInfo.thruster_List[arg1].GenNO);
        this->ui->CV_Channel->setValue(this->tooling_current->plcToolingInfo.thruster_List[arg1].ChannelNO);
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
    this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].valveNO=arg1;
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
        this->ui->comboBox_valveType->setEnabled(false);
        this->ui->spinBox_valveActionConfirm->setEnabled(true);
        this->ui->spinBox_valveStartStep->setEnabled(true);
        this->ui->spinBox_valveEndStep->setEnabled(true);
        this->ui->spinBox_valveEnable->setEnabled(true);
    }
    else
    {
        this->ui->comboBox_valveType->setEnabled(true);
        this->ui->spinBox_valveActionConfirm->setEnabled(true);
        this->ui->spinBox_valveStartStep->setEnabled(true);
        this->ui->spinBox_valveEndStep->setEnabled(true);
        this->ui->spinBox_valveEnable->setEnabled(true);
    }
    this->ui->spinBox_valveEnable->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].enable);
    this->ui->comboBox_valveType->setCurrentIndex(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].valveType);
    this->ui->spinBox_valveActionConfirm->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].actionConfirm);
    this->ui->spinBox_valveStartStep->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].startStep);
    this->ui->spinBox_valveEndStep->setValue(this->tempTooling_editting->plcToolingInfo.pneumaticValvelist[arg1].stopStep);

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
        this->tempTooling_editting->plcToolingInfo.weldPoint_List[i].thrusterPressure_up=wp1->thrusterPressure_up;
    }
}

void MainWindow::on_btn_uploadFromPLC_clicked()
{
    if(this->uploadingWholeSettingFromPLCInProcess)
        return;
    if(this->tcpConnectionStatus_receive&&this->tcpConnectionStatus_send)
    {
        this->uploadingWholeSettingFromPLCInProcess=true;
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_uploadWholeSettingFromPLC());
        QTimer::singleShot(2000, this, &OnUploadWholeSettingsTimeout);
        this->ui->btn_uploadFromPLC->setText("Uploading...");
    }


}
void MainWindow::OnUploadWholeSettingsTimeout()
{
  this->uploadingWholeSettingFromPLCInProcess=false;
  this->ui->btn_uploadFromPLC->setText("UploadFromPLC");
}
void MainWindow::on_btn_downLoadToPLC_clicked()
{
    QByteArrayList arl1;
    arl1=this->tempTooling_editting->prepareCommand_downloadWholeSettingToPLC();
    qDebug()<<"download whole setting to PLC on going, item/command counts:"<<arl1.size();
    QString btnText_old;
    btnText_old=this->ui->btn_downLoadToPLC->text();
    for(int i=0;i<arl1.size();i++)
    {
       emit this->sendDataToTCPCommObj(arl1.at(i));
       this->ui->btn_downLoadToPLC->setText("downloading,Item:"+QString::number(i));
       QEventLoop eventloop;
       QTimer::singleShot(50, &eventloop, SLOT(quit()));
       eventloop.exec();

    }
    this->ui->btn_downLoadToPLC->setText(btnText_old);

    QTimer::singleShot(100, this, &OnDownloadWholeSettingsTimeout);
}
void MainWindow::OnDownloadWholeSettingsTimeout()
{
    if(this->tcpConnectionStatus_receive&&this->tcpConnectionStatus_send)
        emit this->sendDataToTCPCommObj(this->tempTooling_editting->prepareCommand_uploadWholeSettingFromPLC());
}

void MainWindow::on_btn_PLC2Editting_WeldPoint_clicked()
{


    this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()]
            =this->tooling_current->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()];
    this->updateWeldPoitDisplay_Editting
            (this->tempTooling_editting->plcToolingInfo.weldPoint_List[this->ui->spinBox_PointNO_pointPara->value()]);
}
