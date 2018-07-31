#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <bitsoperation.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->wp1=nullptr;

}

MainWindow::~MainWindow()
{
    delete ui;
    delete wp1;

}
void MainWindow::receiveDataFromTCPCommObj(QByteArray dataFromTcpCommObj)
{
    int dataFromTcpCommObj_size=dataFromTcpCommObj.size();
    if(dataFromTcpCommObj_size<6)
    {
       qDebug()<<"subData size <6";
        return;
    }
    if(dataFromTcpCommObj_size!=dataFromTcpCommObj.at(0)*256+dataFromTcpCommObj.at(1))
    {
       qDebug()<<"subData size not equal with length in byte array";
        return;
    }

    quint16 command=dataFromTcpCommObj.at(2)*256+dataFromTcpCommObj.at(3);
    qDebug()<<"command NO:"<<command;
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

    case 2://received gen realtime data, update display area accorddingly
    {
        if(dataLoad[1]==this->ui->spinBox_PointNO_pointPara->value())
        {
            this->ui->UStest_genNO->setValue(dataLoad[0]);
            dWordBytes dw1;
            dw1.bytesVar.B3=dataLoad[2];
            dw1.bytesVar.B2=dataLoad[3];
            dw1.bytesVar.B1=dataLoad[4];
            dw1.bytesVar.B0=dataLoad[5];
            this->ui->CV_pointPara_realTime_Freq->setValue(dw1.DWordVar);
            this->ui->CV_pointPara_realTime_Amp->setValue(dataLoad[8]);
            this->ui->CV_pointPara_realTime_Power->setValue(dataLoad[6]*256+dataLoad[7]);
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
    case 5://get machine Qty config info from PLC , then update the display value

        this->ui->CV_stationQty->setValue(dataLoad[0]);
        this->ui->CV_genQty->setValue(dataLoad[1]);

        this->ui->CV_weldPointQty->setValue(dataLoad[2]);
        this->ui->CV_thrusterQty->setValue(dataLoad[3]);
        this->ui->CV_stepQty->setValue(dataLoad[4]);
        this->ui->CV_partClamperQty->setValue(dataLoad[5]);
        break;
    case 6://get thruster config info from PLC, then update the display value
    {
        quint8 thrusterNOFromPLC;
        thrusterNOFromPLC=dataLoad[0];
        if(thrusterNOFromPLC==this->ui->spinBox_thrusterNO->value())
        {
            this->ui->CV_GenNO->setValue(dataLoad[1]);
            this->ui->CV_Channel->setValue(dataLoad[2]+1);
        }
        break;
    }
    case 7://get weld point config info from PLC, then update the display value
    {
        quint8 weldPointNOFromPLC;
        weldPointNOFromPLC=dataLoad[0];
        if(weldPointNOFromPLC==this->ui->spinBox_weldPointNO->value())
        {
            this->ui->CV_thrusterNO_WPC->setValue(dataLoad[1]);
            this->ui->CV_stepNO_WPC->setValue(dataLoad[2]);
        }
        break;
    }
    case 8://get step/group station info from PLC, then update the display value
    {
       quint8 stepNOFromPLC;
       stepNOFromPLC=dataLoad[0];
       qDebug()<<"stepNOFromPLC"<<stepNOFromPLC<<"dataload[0]"<<dataLoad[0]<<"dataload[1]"<<dataLoad[1];
       if(stepNOFromPLC==this->ui->spinBox_stepNO->value())
            this->ui->CV_stationNO->setValue(dataLoad[1]);
        break;
    }
    case 9://get point u/s para info from PLC, then update the display value
    {
       quint8 pointNOFromPLC;
       pointNOFromPLC=dataLoad[0];

       qDebug()<<"pointNOFromPLC"<<pointNOFromPLC<<"dataload[0]"<<dataLoad[0];
       if(pointNOFromPLC==this->ui->spinBox_PointNO_pointPara->value())
       {

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
            //assign value to CV_x, display the got data
           this->ui->CV_pointPara_afterburstDelay->setValue(wp2.ultrasonicPara.AfterBurstdelay0to999);
           this->ui->spinBox_pointPara_afterburstDelay->setValue(wp2.ultrasonicPara.AfterBurstdelay0to999);
           this->ui->CV_pointPara_afterburst_duration->setValue(wp2.ultrasonicPara.Afterburst_Duration0t999);
           this->ui->spinBox_pointPara_afterburst_duration->setValue(wp2.ultrasonicPara.Afterburst_Duration0t999);
           this->ui->CV_pointPara_amp->setValue(wp2.ultrasonicPara.Amplitude20to100);
           this->ui->spinBox_pointPara_amp->setValue(wp2.ultrasonicPara.Amplitude20to100);
           this->ui->CV_pointPara_downPressure->setValue(wp2.thrusterPressure_down);
           this->ui->spinBox_pointPara_downPressure->setValue(wp2.thrusterPressure_down);
           this->ui->CV_pointPara_freeRunFreq->setValue(wp2.ultrasonicPara.Free_Run_Frequency);
           this->ui->spinbox_pointPara_freeRunFreq->setValue(wp2.ultrasonicPara.Free_Run_Frequency);
           this->ui->CV_pointPara_freq_l_limit->setValue(wp2.ultrasonicPara.Freq_Lower_Limit);
           this->ui->spinbox_pointPara_freq_l_limit->setValue(wp2.ultrasonicPara.Freq_Lower_Limit);
           this->ui->CV_pointPara_freq_u_limit->setValue(wp2.ultrasonicPara.Freq_Upper_Limit);
           this->ui->spinbox_pointPara_freq_u_limit->setValue(wp2.ultrasonicPara.Freq_Upper_Limit);
           this->ui->CV_pointPara_holdTime->setValue(wp2.ultrasonicPara.Hold_TIme0to30000);
           this->ui->spinBox_pointPara_holdTime->setValue(wp2.ultrasonicPara.Hold_TIme0to30000);
           this->ui->CV_pointPara_peakPower->setValue(wp2.ultrasonicPara.Peak_Power);
           this->ui->spinBox_pointPara_peakPower->setValue(wp2.ultrasonicPara.Peak_Power);
           this->ui->CV_pointPara_rampdownTime->setValue(wp2.ultrasonicPara.Ramp_Down_TIme0to250);
           this->ui->spinBox_pointPara_rampdownTime->setValue(wp2.ultrasonicPara.Ramp_Down_TIme0to250);
           this->ui->CV_pointPara_rampupTime->setValue(wp2.ultrasonicPara.Ramp_up_Time20to1250);
           this->ui->spinBox_pointPara_rampupTime->setValue(wp2.ultrasonicPara.Ramp_up_Time20to1250);
           this->ui->CV_pointPara_trigAmp->setValue(wp2.ultrasonicPara.Trigger_Amplitude);
           this->ui->spinBox_pointPara_trigAmp->setValue(wp2.ultrasonicPara.Trigger_Amplitude);
           this->ui->CV_pointPara_trigBypower->setValue(wp2.ultrasonicPara.B64_enableTrigByPower.bits.b0?1:0);
           this->ui->spinBox_pointPara_trigByPower->setValue(wp2.ultrasonicPara.B64_enableTrigByPower.bits.b0?1:0);
           this->ui->CV_pointPara_trigPower->setValue(wp2.ultrasonicPara.Trigger_Power);
           this->ui->spinBox_pointPara_trigPower->setValue(wp2.ultrasonicPara.Trigger_Power);
           this->ui->CV_pointPara_trigTimeout->setValue(wp2.ultrasonicPara.Trigger_Timeout);
           this->ui->spinBox_pointPara_trigTimeout->setValue(wp2.ultrasonicPara.Trigger_Timeout);
           this->ui->CV_pointPara_upPressure->setValue(wp2.thrusterPressure_up);
           this->ui->spinBox_pointPara_upPressure->setValue(wp2.thrusterPressure_up);
           this->ui->CV_pointPara_weldEnergy->setValue(wp2.ultrasonicPara.Energy0to65535);
           this->ui->spinBox_pointPara_weldEnergy->setValue(wp2.ultrasonicPara.Energy0to65535);
           this->ui->CV_pointPara_weldTime->setValue(wp2.ultrasonicPara.Weld_Time0to30000);
           this->ui->spinBox_pointPara_weldTime->setValue(wp2.ultrasonicPara.Weld_Time0to30000);
           this->ui->CV_timeLowerLimit_BAD->setValue(wp2.ultrasonicPara.Badpart_MinTime_Limits);
           this->ui->spinBox_timeLowerLimit_BAD->setValue(wp2.ultrasonicPara.Badpart_MinTime_Limits);
           this->ui->CV_timeLowerLimit_Suspect->setValue(wp2.ultrasonicPara.suspect_MinTime_Limits);
           this->ui->spinBox_timeLowerLimit_Suspect->setValue(wp2.ultrasonicPara.suspect_MinTime_Limits);
           this->ui->CV_timeUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxTime_limits);
           this->ui->spinBox_timeUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxTime_limits);
           this->ui->CV_timeUpperLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MaxTime_Limits);
           this->ui->spinBox_timeUpperLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MaxTime_Limits);
           this->ui->CV_powerLowerLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MinPower_Limits);
           this->ui->spinBox_powerLowerLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MinPower_Limits);
           this->ui->CV_powerLowerLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MinPower_Limits);
           this->ui->spinBox_powerLowerLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MinPower_Limits);
           this->ui->CV_powerUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxPower_Limits);
           this->ui->spinBox_powerUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxPower_Limits);
           this->ui->CV_powerUpperLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MaxPower_Limits);
           this->ui->spinBox_powerUpperLimit_Suspect_2->setValue(wp2.ultrasonicPara.Suspect_MaxPower_Limits);
           this->ui->CV_energyLowerLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MinEnergy_Limits);
           this->ui->spinBox_energyLowerLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MinEnergy_Limits);
           this->ui->CV_energyLowerLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MinEnergy_Limits);
           this->ui->spinBox_energyLowerLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MinEnergy_Limits);
           this->ui->CV_energyUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxEnergy_Limits);
           this->ui->spinBox_energyUpperLimit_BAD->setValue(wp2.ultrasonicPara.BadPart_MaxEnergy_Limits);
           this->ui->CV_energyUpperLimit_Suspect->setValue(wp2.ultrasonicPara.Suspect_MaxEnergy_Limits);
           this->ui->spinBox_energyUpperLimit_Suspect_2->setValue(wp2.ultrasonicPara.Suspect_MaxEnergy_Limits);
           this->ui->CV_pointPara_LimitsEnable_BAD->setCheckState(wp2.ultrasonicPara.B21_EnableSuspectBadByte.bits.b1?Qt::Checked:Qt::Unchecked);
           this->ui->CV_pointPara_LimitsEnable_SUSPECT->setCheckState(wp2.ultrasonicPara.B21_EnableSuspectBadByte.bits.b0?Qt::Checked:Qt::Unchecked);
           this->ui->checkBox_LimitsEnable_BAD->setCheckState(wp2.ultrasonicPara.B21_EnableSuspectBadByte.bits.b1?Qt::Checked:Qt::Unchecked);
           this->ui->checkBox_LimitsEnable_SUSPECT->setCheckState(wp2.ultrasonicPara.B21_EnableSuspectBadByte.bits.b0?Qt::Checked:Qt::Unchecked);
           wp1=nullptr;
       }

        break;
    }
    case 10://get station info from PLC , then update the display value
    {
        quint8 stationNOFromPLC;
        stationNOFromPLC=dataLoad[0];
        if(stationNOFromPLC==this->ui->stationPara_stationNO->value())
        {
            this->ui->CV_stationPara_speedType->setCurrentIndex((int)dataLoad[1]);
            dWordBytes dw1;
            dw1.bytesVar.B0=dataLoad[2];
            dw1.bytesVar.B1=dataLoad[3];
            dw1.bytesVar.B2=dataLoad[4];
            dw1.bytesVar.B3=dataLoad[5];
            this->ui->CV_stationPara_setPoint->setValue((qint32)dw1.DWordVar);
            dw1.bytesVar.B0=dataLoad[6];
            dw1.bytesVar.B1=dataLoad[7];
            dw1.bytesVar.B2=dataLoad[8];
            dw1.bytesVar.B3=dataLoad[9];
            this->ui->CV_stationPara_upperLimit->setValue((qint32)dw1.DWordVar);
            dw1.bytesVar.B0=dataLoad[10];
            dw1.bytesVar.B1=dataLoad[11];
            dw1.bytesVar.B2=dataLoad[12];
            dw1.bytesVar.B3=dataLoad[13];
            this->ui->CV_stationPara_lowerLimit->setValue((qint32)dw1.DWordVar);

        }
    }
        break;
    case 11://get part Clamper info from PLC, then update the display value
    {
       quint8 clamperNOFromPLC;
       clamperNOFromPLC=dataLoad[0];
       //qDebug()<<"stepNOFromPLC"<<stepNOFromPLC<<"dataload[0]"<<dataLoad[0]<<"dataload[1]"<<dataLoad[1];
       if(clamperNOFromPLC==this->ui->spinBox_partClamperNO->value())
       {
           this->ui->CV_clamperStartStep->setValue(dataLoad[1]);
           this->ui->CV_clamperEndStep->setValue(dataLoad[2]);
           this->ui->CV_ActionConfirm->setValue(dataLoad[3]);
           this->ui->CV_clamperEnableBypass->setValue(dataLoad[4]);
       }
        break;
    }
    case 12://get the servo speed setting and home direction setting on PLC,then update the dispaly value
    {

        dWordBytes dw1;
        dw1.bytesVar.B0=dataLoad[0];
        dw1.bytesVar.B1=dataLoad[1];
        dw1.bytesVar.B2=dataLoad[2];
        dw1.bytesVar.B3=dataLoad[3];
        this->ui->CV_stationPara_HighSpeed->setValue((qint32)dw1.DWordVar/5);
        dw1.bytesVar.B0=dataLoad[4];
        dw1.bytesVar.B1=dataLoad[5];
        dw1.bytesVar.B2=dataLoad[6];
        dw1.bytesVar.B3=dataLoad[7];
        this->ui->CV_stationPara_MediumSpeed->setValue((qint32)dw1.DWordVar/5);
        dw1.bytesVar.B0=dataLoad[8];
        dw1.bytesVar.B1=dataLoad[9];
        dw1.bytesVar.B2=dataLoad[10];
        dw1.bytesVar.B3=dataLoad[11];
        this->ui->CV_stationPara_lowSpeed->setValue((qint32)dw1.DWordVar/5);
        dw1.bytesVar.B0=dataLoad[12];
        dw1.bytesVar.B1=dataLoad[13];
        dw1.bytesVar.B2=dataLoad[14];
        dw1.bytesVar.B3=dataLoad[15];
        this->ui->CV_stationPara_defaultSpeed->setValue((qint32)dw1.DWordVar/5);
        this->ui->CV_servoPara_CWCCW->setCurrentIndex((int)dataLoad[16]);
    }
        break;
    case 13:
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
    case 100://change HMI page NO

        break;

    default:
        qDebug()<<"Command not match , no assigned hanlder";
        break;
    }


}

void MainWindow::on_btn_setQtys_clicked()
{
    //set machine qty config
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x10;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x70;//commandNO low byte,112
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->spinBox_stationQty->value();
    dataToTcpCommObj[7]=this->ui->spinBox_GenQty->value();
    dataToTcpCommObj[8]=this->ui->spinBox_weldPointQty->value();
    dataToTcpCommObj[9]=this->ui->spinBox_thrusterQty->value();
    dataToTcpCommObj[10]=this->ui->spinBox_stepQty->value();
    dataToTcpCommObj[11]=this->ui->spinBox_partClamperQty->value();
    dataToTcpCommObj[12]=0x00;//top holder Qty
    dataToTcpCommObj[13]=0x00;//reserve
    dataToTcpCommObj[14]=0x00;//reserve
    dataToTcpCommObj[15]=0x00;//reserve
    //emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    //query machine qty config
    dataToTcpCommObj[16]=0x00;//length high byte
    dataToTcpCommObj[17]=0x06;//length low byte,102
    dataToTcpCommObj[18]=0x00;//commandNO high byte
    dataToTcpCommObj[19]=0x66;//commandNO low byte
    dataToTcpCommObj[20]=0x00;//reserve byte
    dataToTcpCommObj[21]=0x00;//reserve byte

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);

}
void MainWindow::on_btn_getQtys_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x66;//commandNO low byte,102
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_QtyConfigUndo_clicked()
{

    this->ui->spinBox_stationQty->setValue(this->ui->CV_stationQty->value());
    this->ui->spinBox_GenQty->setValue(this->ui->CV_genQty->value());
    this->ui->spinBox_weldPointQty->setValue(this->ui->CV_weldPointQty->value());
    this->ui->spinBox_thrusterQty->setValue(this->ui->CV_thrusterQty->value());
    this->ui->spinBox_stepQty->setValue(this->ui->CV_stepQty->value());
    this->ui->CV_partClamperQty->setValue(this->ui->CV_partClamperQty->value());
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
    dataToTcpCommObj[8]=this->ui->spinBox_channel->value()-1;
    dataToTcpCommObj[9]=0x00;
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
     QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x68;//commandNO low byte,104
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_undoThrusterConfig_clicked()
{
   qDebug()<<"query undo thruster config btn_2 clicked";
   this->ui->spinBox_GenNO->setValue(this->ui->CV_GenNO->value());
   this->ui->spinBox_channel->setValue(this->ui->CV_Channel->value());
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
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x6A;//commandNO low byte,106
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_undoStepsStationEdit_clicked()
{
    this->ui->spinBox_stationNO->setValue(this->ui->CV_stationNO->value());
}

void MainWindow::on_btn_setWeldPointConfig_clicked()
{
    //set weld Point config
    qDebug()<<"set weld point btn clicked";
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0A;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x73;//commandNO low byte,115
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->spinBox_weldPointNO->value();
    dataToTcpCommObj[7]=this->ui->spinBox_thrusterNOWPC->value();
    dataToTcpCommObj[8]=this->ui->spinBox_stepNO_WPC->value();
    dataToTcpCommObj[9]=0x00;

    //emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    //query thruster  config
    dataToTcpCommObj[10]=0x00;//length high byte
    dataToTcpCommObj[11]=0x06;//length low byte
    dataToTcpCommObj[12]=0x00;//commandNO high byte
    dataToTcpCommObj[13]=0x69;//commandNO low byte,105
    dataToTcpCommObj[14]=0x00;//reserve byte
    dataToTcpCommObj[15]=0x00;//reserve byte

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_QueryWeldPointConfig_clicked()
{
    //query thruster  config
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x69;//commandNO low byte,105
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_undoWeldPointConfig_clicked()
{
    this->ui->spinBox_thrusterNOWPC->setValue(this->ui->CV_thrusterNO_WPC->value());
    this->ui->spinBox_stepNO_WPC->setValue(this->ui->CV_stepNO_WPC->value());
}

void MainWindow::on_btn_setPartClamperConfig_clicked()
{
    //set part clamper config
    qDebug()<<"set part clamper btn clicked";
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0C;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x76;//commandNO low byte,118
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=this->ui->spinBox_partClamperNO->value();
    dataToTcpCommObj[7]=this->ui->spinBox_clamperStartStep->value();
    dataToTcpCommObj[8]=this->ui->spinBox_clamperEndStep->value();
    dataToTcpCommObj[9]=this->ui->spinBox_clamperActionConfirm->value();
    dataToTcpCommObj[10]=0x00;//reserve
    dataToTcpCommObj[11]=0x00;//reserve
    //emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    //query part clamper  config
    dataToTcpCommObj[12]=0x00;//length high byte
    dataToTcpCommObj[13]=0x06;//length low byte
    dataToTcpCommObj[14]=0x00;//commandNO high byte
    dataToTcpCommObj[15]=0x6C;//commandNO low byte,108
    dataToTcpCommObj[16]=0x00;//reserve byte
    dataToTcpCommObj[17]=0x00;//reserve byte

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_QueryClamperConfig_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x6C;//commandNO low byte,108
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_undoClamperConfig_clicked()
{

    this->ui->spinBox_clamperActionConfirm->setValue(this->ui->CV_ActionConfirm->value());
    this->ui->spinBox_clamperStartStep->setValue(this->ui->CV_clamperStartStep->value());
    this->ui->spinBox_clamperEndStep->setValue(this->ui->CV_clamperEndStep->value());
    this->ui->spinBox_partClamperEnable->setValue(this->ui->CV_clamperEnableBypass->value());
}
#pragma pack(2)
void MainWindow::on_btn_setPointPara_clicked()
{
   qDebug()<<"set point us para btn pressed";
    if(true)
    {
        wp1=new weldPoint;
        wp1->pointNO=this->ui->spinBox_PointNO_pointPara->value();
        wp1->thrusterPressure_down=this->ui->spinBox_pointPara_downPressure->value();
        wp1->thrusterPressure_up=this->ui->spinBox_pointPara_upPressure->value();
        wp1->GenNO=0;
        wp1->stepNO=0;
        wp1->ThrusterNO=0;
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

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
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
    //emit this->sendDataToTCPCommObj(dataToTcpCommObj);
    //Reset the pointer
    delete wp1;
    wp1=nullptr;
    ptrCharData=nullptr;
}
#pragma pack()

void MainWindow::on_btn_getPointPara_clicked()
{
    QByteArray ar2;
    ar2[0]=0x00;//length high byte
    ar2[1]=0x0A;//length low byte
    ar2[2]=0x00;//commandNO high byte
    ar2[3]=0x6B;//commandNO low byte,107
    ar2[4]=0x00;//reserve byte
    ar2[5]=0x00;//reserve byte
    ar2[6]=this->ui->spinBox_PointNO_pointPara->value();
    ar2[7]=0x01;//point count,can query more than 1 point's data
    ar2[8]=0x00;//reserve byte
    ar2[9]=0x00;//reserve byte
    //dataToTcpCommObj.append(ar2);
    emit this->sendDataToTCPCommObj(ar2);
}

void MainWindow::on_btn_undoPointPara_clicked()
{

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
    qDebug()<<"homming dir, current index"<<dataToTcpCommObj[22];
    dataToTcpCommObj[23]=0x00;
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
    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_btn_QuerystationPara_clicked()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x67;//commandNO low byte,103
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_actionTooling_Config_triggered()
{
    this->ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_actionPoint_Config_triggered()
{
  this->ui->stackedWidget->setCurrentIndex(1);
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


void MainWindow::on_pushButton_servo_home_pressed()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x0E;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x78;//commandNO low byte,120
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    dataToTcpCommObj[6]=0x00;//PointNO
    dataToTcpCommObj[7]=0x01;//DownUp CoilA
    dataToTcpCommObj[8]=0x01;//DownUp CoilB, A==B==1，then PLC ignore them
    dataToTcpCommObj[9]=0x01;//TEST
    dataToTcpCommObj[10]=0x00;//Fire
    dataToTcpCommObj[11]=0x00;//Batch
    dataToTcpCommObj[12]=0x00;//reserve
    dataToTcpCommObj[13]=0x00;//reserve
    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}

void MainWindow::on_actionWeld_By_Manual_triggered()
{
    this->ui->stackedWidget->setCurrentIndex(2);
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

    emit this->sendDataToTCPCommObj(dataToTcpCommObj);
}
