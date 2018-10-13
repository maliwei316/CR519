#include "clstooling.h"
#include <QDebug>
#include <QDir>
extern bool loggingEnable;
extern quint8 loggingLevel;

clsTooling::clsTooling(QObject *parent) : QObject(parent)
{

    memset(&this->plcToolingInfo,0,sizeof(toolingInfo_in_PLC));
    for(int i=0;i<17;i++)
    {
        this->pointNameMapping.append("");
        this->pointBarcodeMapping.append("Barcode_Left");
        if(i<7)
        {
            this->valveNameMapping.append("");

            this->plcToolingInfo.pneumaticValvelist[i].valveNO=i;
        }

        this->plcToolingInfo.weldPoint_List[i].pointNO=i;
        this->plcToolingInfo.station_List[i].stationNO=i;
        this->plcToolingInfo.thruster_List[i].thrusterNO=i;

    }


}
void clsTooling::readJason(const QJsonObject &json)
{
    {
         if (json.contains("toolingName") && json["toolingName"].isString())
             this->toolingName = json["toolingName"].toString();
         if (json.contains("toolingImageSource") && json["toolingImageSource"].isString())
             this->toolingImageSource = json["toolingImageSource"].toString();
         if (json.contains("pointNameMapping") && json["pointNameMapping"].isArray()) {
                  QVariantList pointNameMappingVariantArray = json["pointNameMapping"].toArray().toVariantList();

                  for (int i = 0; i < pointNameMappingVariantArray.size(); ++i) {
                      this->pointNameMapping[i]=pointNameMappingVariantArray.at(i).toString();
                  }
              }
         if (json.contains("pointBarcodeMapping") && json["pointBarcodeMapping"].isArray()) {
                  QVariantList pointBarcodeMappingVariantArray = json["pointBarcodeMapping"].toArray().toVariantList();

                  for (int i = 0; i < pointBarcodeMappingVariantArray.size(); ++i) {
                      this->pointBarcodeMapping[i]=pointBarcodeMappingVariantArray.at(i).toString();
                  }
              }
         if(json.contains("barcodeSettings_left"))
         {

             QJsonObject QJO_leftBarcodeSettings=json["barcodeSettings_left"].toObject();
             if(QJO_leftBarcodeSettings.contains("enable"))
             {
                 this->leftBarcodeSettings.enable=
                         (QJO_leftBarcodeSettings["enable"].toString().toUpper()=="TRUE")?true:false;

             }
             if(QJO_leftBarcodeSettings.contains("portName"))
             {
                 this->leftBarcodeSettings.portName=QJO_leftBarcodeSettings["portName"].toString();
             }
             if(QJO_leftBarcodeSettings.contains("baud"))
             {
                 this->leftBarcodeSettings.baud=QJO_leftBarcodeSettings["baud"].toString();
             }
             if(QJO_leftBarcodeSettings.contains("dataBits"))
             {
                 this->leftBarcodeSettings.dataBits=QJO_leftBarcodeSettings["dataBits"].toInt();
             }
             if(QJO_leftBarcodeSettings.contains("stopBits"))
             {
                 this->leftBarcodeSettings.stopBits=QJO_leftBarcodeSettings["stopBits"].toInt();
             }
             if(QJO_leftBarcodeSettings.contains("parity"))
             {
                 this->leftBarcodeSettings.parity=QJO_leftBarcodeSettings["parity"].toString();
             }
             if(QJO_leftBarcodeSettings.contains("maxLength"))
             {
                 this->leftBarcodeSettings.maxLength=QJO_leftBarcodeSettings["maxLength"].toInt();
             }
             if(QJO_leftBarcodeSettings.contains("minLength"))
             {
                 this->leftBarcodeSettings.minLength=QJO_leftBarcodeSettings["minLength"].toInt();
             }
             if(QJO_leftBarcodeSettings.contains("prefix"))
             {
                 this->leftBarcodeSettings.prefix=QJO_leftBarcodeSettings["prefix"].toString();
             }
             if(QJO_leftBarcodeSettings.contains("sufix"))
             {
                 this->leftBarcodeSettings.suffix=QJO_leftBarcodeSettings["sufix"].toString();
             }
         }
         if(json.contains("barcodeSettings_right"))
         {

             QJsonObject QJO_rightBarcodeSettings=json["barcodeSettings_right"].toObject();
             if(QJO_rightBarcodeSettings.contains("enable"))
             {
                 this->rightBarcodeSettings.enable=
                         (QJO_rightBarcodeSettings["enable"].toString().toUpper()=="TRUE")?true:false;

             }
             if(QJO_rightBarcodeSettings.contains("portName"))
             {
                 this->rightBarcodeSettings.portName=QJO_rightBarcodeSettings["portName"].toString();
             }
             if(QJO_rightBarcodeSettings.contains("baud"))
             {
                 this->rightBarcodeSettings.baud=QJO_rightBarcodeSettings["baud"].toString();
             }
             if(QJO_rightBarcodeSettings.contains("dataBits"))
             {
                 this->rightBarcodeSettings.dataBits=QJO_rightBarcodeSettings["dataBits"].toInt();
             }
             if(QJO_rightBarcodeSettings.contains("stopBits"))
             {
                 this->rightBarcodeSettings.stopBits=QJO_rightBarcodeSettings["stopBits"].toInt();
             }
             if(QJO_rightBarcodeSettings.contains("parity"))
             {
                 this->rightBarcodeSettings.parity=QJO_rightBarcodeSettings["parity"].toString();
             }
             if(QJO_rightBarcodeSettings.contains("maxLength"))
             {
                 this->rightBarcodeSettings.maxLength=QJO_rightBarcodeSettings["maxLength"].toInt();
             }
             if(QJO_rightBarcodeSettings.contains("minLength"))
             {
                 this->rightBarcodeSettings.minLength=QJO_rightBarcodeSettings["minLength"].toInt();
             }
             if(QJO_rightBarcodeSettings.contains("prefix"))
             {
                 this->rightBarcodeSettings.prefix=QJO_rightBarcodeSettings["prefix"].toString();
             }
             if(QJO_rightBarcodeSettings.contains("sufix"))
             {
                 this->rightBarcodeSettings.suffix=QJO_rightBarcodeSettings["sufix"].toString();
             }
         }
         if (json.contains("valveNameMapping") && json["valveNameMapping"].isArray()) {
                  QVariantList valveNameMappingVariantArray = json["valveNameMapping"].toArray().toVariantList();

                  for (int i = 0; i < valveNameMappingVariantArray.size(); ++i) {
                      this->valveNameMapping[i]=valveNameMappingVariantArray.at(i).toString();
                  }
              }
         if(json.contains("plcToolingInfo")&&json["plcToolingInfo"].isObject())
         {
            QJsonObject QJO_plcToolingInfo=json["plcToolingInfo"].toObject();
            if(QJO_plcToolingInfo.contains("generator_enable")&&QJO_plcToolingInfo["generator_enable"].isArray())
            {
                QVariantList genEnableList=QJO_plcToolingInfo["generator_enable"].toArray().toVariantList();
                for(int i=0;i<genEnableList.size();i++)
                {
                    this->plcToolingInfo.generator_enable[i]=(genEnableList.at(i).toString().toUpper()=="TRUE")?true:false;
                }
            }
            if(QJO_plcToolingInfo.contains("pneumaticValvelist")&&QJO_plcToolingInfo["pneumaticValvelist"].isArray())
            {
                QJsonArray valveList_JsonArray=QJO_plcToolingInfo["pneumaticValvelist"].toArray();
                for(int i=0;i<valveList_JsonArray.size();i++)
                {
                    if(valveList_JsonArray.at(i).isObject())
                    {
                        QJsonObject qjobj1=valveList_JsonArray.at(i).toObject();
                        if(qjobj1.contains("actionConfirm"))
                        {
                            this->plcToolingInfo.pneumaticValvelist[i].actionConfirm=
                                    (qjobj1["actionConfirm"].toString().toUpper()=="TRUE")?true:false;
                        }
                        if(qjobj1.contains("enable"))
                        {
                            this->plcToolingInfo.pneumaticValvelist[i].enable=
                                    (qjobj1["enable"].toString().toUpper()=="TRUE")?true:false;
                        }
                        if(qjobj1.contains("startStep"))
                        {
                            this->plcToolingInfo.pneumaticValvelist[i].startStep=qjobj1["startStep"].toInt();

                        }
                        if(qjobj1.contains("stopStep"))
                        {
                            this->plcToolingInfo.pneumaticValvelist[i].stopStep=qjobj1["stopStep"].toInt();

                        }
                        if(qjobj1.contains("valveNO"))
                        {
                            this->plcToolingInfo.pneumaticValvelist[i].valveNO=qjobj1["valveNO"].toInt();

                        }
                        if(qjobj1.contains("valveType"))
                        {
                            this->plcToolingInfo.pneumaticValvelist[i].valveType=qjobj1["valveType"].toInt();

                        }
                    }

                }

            }
            if(QJO_plcToolingInfo.contains("servoHomingDir"))
            {
                this->plcToolingInfo.servoHomingDir=(QJO_plcToolingInfo["servoHomingDir"].toString().toUpper()=="CW")?true:false;
            }
            if(QJO_plcToolingInfo.contains("servoSpeed_high"))
            {
                this->plcToolingInfo.servoSpeed_high=QJO_plcToolingInfo["servoSpeed_high"].toInt();
            }
            if(QJO_plcToolingInfo.contains("servoSpeed_medium"))
            {
                this->plcToolingInfo.servoSpeed_medium=QJO_plcToolingInfo["servoSpeed_medium"].toInt();
            }
            if(QJO_plcToolingInfo.contains("servoSpeed_low"))
            {
                this->plcToolingInfo.servoSpeed_low=QJO_plcToolingInfo["servoSpeed_low"].toInt();
            }
            if(QJO_plcToolingInfo.contains("station_List")&&QJO_plcToolingInfo["station_List"].isArray())
            {
                QJsonArray stationList_JsonArray=QJO_plcToolingInfo["station_List"].toArray();
                for(int i=1;i<stationList_JsonArray.size();i++)
                {
                    if(stationList_JsonArray.at(i).isObject())
                    {
                        QJsonObject qjobj1=stationList_JsonArray.at(i).toObject();
                        if(qjobj1.contains("pos_lowerLimit"))
                        {
                            this->plcToolingInfo.station_List[i].pos_lowerLimit=qjobj1["pos_lowerLimit"].toInt();
                        }
                        if(qjobj1.contains("pos_setPoint"))
                        {
                            this->plcToolingInfo.station_List[i].pos_setPoint=qjobj1["pos_setPoint"].toInt();
                        }
                        if(qjobj1.contains("pos_upperLimit"))
                        {
                            this->plcToolingInfo.station_List[i].pos_upperLimit=qjobj1["pos_upperLimit"].toInt();
                        }
                        if(qjobj1.contains("speedType"))
                        {
                            this->plcToolingInfo.station_List[i].speedType=qjobj1["speedType"].toInt();
                        }
                        if(qjobj1.contains("stationNO"))
                        {
                            this->plcToolingInfo.station_List[i].stationNO=qjobj1["stationNO"].toInt();
                        }

                    }

                }
            }
            if(QJO_plcToolingInfo.contains("stepStationConnection")&&QJO_plcToolingInfo["stepStationConnection"].isArray())
            {
                //this->plcToolingInfo.stepStationConnection
                        QJsonArray stepStationConnection_JsonArray=QJO_plcToolingInfo["stepStationConnection"].toArray();
                        for(int i=0;i<stepStationConnection_JsonArray.size();i++)
                        {
                            this->plcToolingInfo.stepStationConnection[i]=stepStationConnection_JsonArray.at(i).toInt();

                        }

            }

            if(QJO_plcToolingInfo.contains("thruster_List")&&QJO_plcToolingInfo["thruster_List"].isArray())
            {

                QJsonArray thruster_List_JsonArray=QJO_plcToolingInfo["thruster_List"].toArray();
                for(int i=0;i<thruster_List_JsonArray.size();i++)
                {
                    if(thruster_List_JsonArray.at(i).isObject())
                    {
                        QJsonObject qjobj1=thruster_List_JsonArray.at(i).toObject();
                        if(qjobj1.contains("ChannelNO"))
                        {
                            this->plcToolingInfo.thruster_List[i].ChannelNO=qjobj1["ChannelNO"].toInt();
                        }
                        if(qjobj1.contains("enable"))
                        {
                            this->plcToolingInfo.thruster_List[i].enable=(qjobj1["enable"].toString().toUpper()=="TRUE")?true:false;
                        }
                        if(qjobj1.contains("GenNO"))
                        {
                            this->plcToolingInfo.thruster_List[i].GenNO=qjobj1["GenNO"].toInt();
                        }
                        if(qjobj1.contains("thrusterNO"))
                        {
                            this->plcToolingInfo.thruster_List[i].thrusterNO=qjobj1["thrusterNO"].toInt();
                        }

                    }

                }
            }
            if(QJO_plcToolingInfo.contains("toolingNO"))
            {
                this->plcToolingInfo.toolingNO=QJO_plcToolingInfo["toolingNO"].toInt();
            }
            if(QJO_plcToolingInfo.contains("weldPoint_List")&&QJO_plcToolingInfo["weldPoint_List"].isArray())
            {

                QJsonArray weldPoint_List_JsonArray=QJO_plcToolingInfo["weldPoint_List"].toArray();
                for(int i=0;i<weldPoint_List_JsonArray.size();i++)
                {
                    if(weldPoint_List_JsonArray.at(i).isObject())
                    {
                        QJsonObject qjobj1=weldPoint_List_JsonArray.at(i).toObject();
                        if(qjobj1.contains("enable"))
                        {
                            this->plcToolingInfo.weldPoint_List[i].enable=(qjobj1["enable"].toString().toUpper()=="TRUE")?true:false;
                        }
                        if(qjobj1.contains("GenNO"))
                        {
                            this->plcToolingInfo.weldPoint_List[i].GenNO=qjobj1["GenNO"].toInt();
                        }
                        if(qjobj1.contains("pointNO"))
                        {
                            this->plcToolingInfo.weldPoint_List[i].pointNO=qjobj1["pointNO"].toInt();
                        }
                        if(qjobj1.contains("stepNO"))
                        {
                            this->plcToolingInfo.weldPoint_List[i].stepNO=qjobj1["stepNO"].toInt();
                        }
                        if(qjobj1.contains("ThrusterNO"))
                        {
                            this->plcToolingInfo.weldPoint_List[i].ThrusterNO=qjobj1["ThrusterNO"].toInt();
                        }
                        if(qjobj1.contains("thrusterPressure_down"))
                        {
                            this->plcToolingInfo.weldPoint_List[i].thrusterPressure_down=qjobj1["thrusterPressure_down"].toInt();
                        }
//                        if(qjobj1.contains("thrusterPressure_up"))
//                        {
//                            this->plcToolingInfo.weldPoint_List[i].thrusterPressure_up=qjobj1["thrusterPressure_up"].toInt();
//                        }
                        if(qjobj1.contains("ultrasonicPara")&&qjobj1["ultrasonicPara"].isObject())
                        {
                            ;
                            QJsonObject QJObj_us_para=qjobj1["ultrasonicPara"].toObject();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B0_CommandByte.byteChar=0;
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B1_iQLinQCommStatus.byteChar=0;
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B20_trigByte_ProcLimits.byteChar=0;
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B2_trigByte_ProcessControl.byteChar=0;
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B46_trigByteHW_Setup.byteChar=0;
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B47_freqLockHold.byteChar=0;
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B63_trigByteTrigBypower.byteChar=0;
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.AfterBurstdelay0to999
                                        =QJObj_us_para["AfterBurstdelay0to999"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Afterburst_Duration0t999
                                    =QJObj_us_para["Afterburst_Duration0t999"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Amplitude20to100
                                    =QJObj_us_para["Amplitude20to100"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B21_EnableSuspectBadByte.byteChar
                                    =QJObj_us_para["B21_EnableSuspectBadByte"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B64_enableTrigByPower.byteChar
                                    =QJObj_us_para["B64_enableTrigByPower"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.BadPart_MaxEnergy_Limits
                                    =QJObj_us_para["BadPart_MaxEnergy_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.BadPart_MaxPower_Limits
                                    =QJObj_us_para["BadPart_MaxPower_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.BadPart_MaxTime_limits
                                    =QJObj_us_para["BadPart_MaxTime_limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.BadPart_MinEnergy_Limits
                                    =QJObj_us_para["BadPart_MinEnergy_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.BadPart_MinPower_Limits
                                    =QJObj_us_para["BadPart_MinPower_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Badpart_MinTime_Limits
                                    =QJObj_us_para["Badpart_MinTime_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Energy0to65535
                                    =QJObj_us_para["Energy0to65535"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Free_Run_Frequency
                                    =QJObj_us_para["Free_Run_Frequency"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Freq_Lower_Limit
                                    =QJObj_us_para["Freq_Lower_Limit"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Freq_Upper_Limit
                                    =QJObj_us_para["Freq_Upper_Limit"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Hold_TIme0to30000
                                    =QJObj_us_para["Hold_TIme0to30000"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Neg20_to_Pos20=0;
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Peak_Power
                                    =QJObj_us_para["Peak_Power"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Ramp_Down_TIme0to250
                                    =QJObj_us_para["Ramp_Down_TIme0to250"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Ramp_up_Time20to1250
                                    =QJObj_us_para["Ramp_up_Time20to1250"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.suspect_MinTime_Limits
                                    =QJObj_us_para["suspect_MinTime_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Suspect_MinPower_Limits
                                    =QJObj_us_para["Suspect_MinPower_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Suspect_MinEnergy_Limits
                                    =QJObj_us_para["Suspect_MinEnergy_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Suspect_MaxTime_Limits
                                    =QJObj_us_para["Suspect_MaxTime_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Suspect_MaxPower_Limits
                                    =QJObj_us_para["Suspect_MaxPower_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Suspect_MaxEnergy_Limits
                                    =QJObj_us_para["Suspect_MaxEnergy_Limits"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Trigger_Amplitude
                                    =QJObj_us_para[""].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Trigger_Power
                                    =QJObj_us_para["Trigger_Power"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Trigger_Timeout
                                    =QJObj_us_para["Trigger_Timeout"].toInt();
                            this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Weld_Time0to30000
                                    =QJObj_us_para["Weld_Time0to30000"].toInt();

                        }
                    }

                }
            }
            if(QJO_plcToolingInfo.contains("filmFeeder")&&QJO_plcToolingInfo["filmFeeder"].isObject())
            {
                QJsonObject qjobj_filmfeeder=QJO_plcToolingInfo["filmFeeder"].toObject();
                if(qjobj_filmfeeder.contains("enable"))
                {
                    this->plcToolingInfo.feeder.enable=qjobj_filmfeeder["enable"].toInt();
                }
                if(qjobj_filmfeeder.contains("direction"))
                {
                    this->plcToolingInfo.feeder.direction=(qjobj_filmfeeder["direction"].toString().toUpper()=="CW")?0:1;
                }
                if(qjobj_filmfeeder.contains("distance_1"))
                {
                    this->plcToolingInfo.feeder.distance_1=qjobj_filmfeeder["distance_1"].toInt();
                }
                if(qjobj_filmfeeder.contains("distance_2"))
                {
                    this->plcToolingInfo.feeder.distance_2=qjobj_filmfeeder["distance_2"].toInt();
                }
                if(qjobj_filmfeeder.contains("interval"))
                {
                    this->plcToolingInfo.feeder.interval=qjobj_filmfeeder["interval"].toInt();
                }
                if(qjobj_filmfeeder.contains("speed"))
                {
                    this->plcToolingInfo.feeder.speed=qjobj_filmfeeder["speed"].toInt();
                }

            }
        //partSensorBypass_G1
            if(QJO_plcToolingInfo.contains("partSensorBypass_G1")&&QJO_plcToolingInfo["partSensorBypass_G1"].isArray())
            {
                QVariantList partSensorList_Bypass=QJO_plcToolingInfo["partSensorBypass_G1"].toArray().toVariantList();
                for(int i=0;i<partSensorList_Bypass.size();i++)
                {
                    if(partSensorList_Bypass.at(i).toString().toUpper()=="BYPASS")
                    {
                       this->plcToolingInfo.partSensorBypass.extendBypass.bits.setBit(i);
                    }

                    else
                    {
                       this->plcToolingInfo.partSensorBypass.extendBypass.bits.resetBit(i);
                    }

                }

            }
        //partSensorBypass_G2
            if(QJO_plcToolingInfo.contains("partSensorBypass_G2")&&QJO_plcToolingInfo["partSensorBypass_G2"].isArray())
            {
                QVariantList partSensorList_Bypass=QJO_plcToolingInfo["partSensorBypass_G2"].toArray().toVariantList();
                for(int i=0;i<partSensorList_Bypass.size();i++)
                {
                    if(partSensorList_Bypass.at(i).toString().toUpper()=="BYPASS")
                    {
                       this->plcToolingInfo.partSensorBypass.retractBypass.bits.setBit(i);
                    }

                    else
                    {
                       this->plcToolingInfo.partSensorBypass.retractBypass.bits.resetBit(i);
                    }

                }

            }
        //valve SensorBypass
            if(QJO_plcToolingInfo.contains("valveSensorBypass"))
            {
                QJsonObject QJO_valveSensorBypass=QJO_plcToolingInfo["valveSensorBypass"].toObject();

                for(int i=0;i<7;i++)
                {
                   if(QJO_valveSensorBypass.contains("ExtendSensor_Bypass_Valve_"+QString::number(i))
                           &&QJO_valveSensorBypass["ExtendSensor_Bypass_Valve_"+QString::number(i)].isArray())
                   {
                       QVariantList sensorBypassVariantList=
                               QJO_valveSensorBypass["ExtendSensor_Bypass_Valve_"+QString::number(i)].toArray().toVariantList();
                       for(int j=0;j<7;j++)
                       {
                           if(sensorBypassVariantList.at(j).toString().toUpper()=="BYPASS"?true:false)
                           {
                               this->plcToolingInfo.valveSensorBypass[i].extendBypass.bits.setBit(j);
                           }
                           else
                           {
                              this->plcToolingInfo.valveSensorBypass[i].extendBypass.bits.resetBit(j);
                           }

                       }

                   }
                   if(QJO_valveSensorBypass.contains("RetractSensor_Bypass_Valve_"+QString::number(i))
                           &&QJO_valveSensorBypass["RetractSensor_Bypass_Valve_"+QString::number(i)].isArray())
                   {
                       QVariantList sensorBypassVariantList=
                               QJO_valveSensorBypass["RetractSensor_Bypass_Valve_"+QString::number(i)].toArray().toVariantList();
                       for(int j=0;j<7;j++)
                       {
                           if(sensorBypassVariantList.at(j).toString().toUpper()=="BYPASS"?true:false)
                           {
                               this->plcToolingInfo.valveSensorBypass[i].retractBypass.bits.setBit(j);
                           }
                           else
                           {
                              this->plcToolingInfo.valveSensorBypass[i].retractBypass.bits.resetBit(j);
                           }

                       }

                   }

                }

            }
        //...................
         }

     }
}
void clsTooling::writeJason(QJsonObject &json)
{

          //qDebug()<<"start to write Json";
          json["toolingName"] =this->toolingName;
          json["toolingImageSource"]=this->toolingImageSource;
          json["pointNameMapping"]=QJsonArray::fromStringList(this->pointNameMapping);
          json["pointBarcodeMapping"]=QJsonArray::fromStringList(this->pointBarcodeMapping);
          json["valveNameMapping"]=QJsonArray::fromStringList(this->valveNameMapping);

          //json["barcodeSettings_left"]
          QJsonObject qobj_leftBarcodeSettings,qobj_rightBarcodeSettings;
          qobj_leftBarcodeSettings["enable"]=this->leftBarcodeSettings.enable?"TRUE":"FALSE";
          qobj_leftBarcodeSettings["portName"]=this->leftBarcodeSettings.portName;
          qobj_leftBarcodeSettings["baud"]=this->leftBarcodeSettings.baud;
          qobj_leftBarcodeSettings["dataBits"]=this->leftBarcodeSettings.dataBits;
          qobj_leftBarcodeSettings["stopBits"]=this->leftBarcodeSettings.stopBits;
          qobj_leftBarcodeSettings["parity"]=this->leftBarcodeSettings.parity;
          qobj_leftBarcodeSettings["maxLength"]=this->leftBarcodeSettings.maxLength;
          qobj_leftBarcodeSettings["minLength"]=this->leftBarcodeSettings.minLength;
          qobj_leftBarcodeSettings["prefix"]=this->leftBarcodeSettings.prefix;
          qobj_leftBarcodeSettings["sufix"]=this->leftBarcodeSettings.suffix;

          qobj_rightBarcodeSettings["enable"]=this->rightBarcodeSettings.enable?"TRUE":"FALSE";
          qobj_rightBarcodeSettings["portName"]=this->rightBarcodeSettings.portName;
          qobj_rightBarcodeSettings["baud"]=this->rightBarcodeSettings.baud;
          qobj_rightBarcodeSettings["dataBits"]=this->rightBarcodeSettings.dataBits;
          qobj_rightBarcodeSettings["stopBits"]=this->rightBarcodeSettings.stopBits;
          qobj_rightBarcodeSettings["parity"]=this->rightBarcodeSettings.parity;
          qobj_rightBarcodeSettings["maxLength"]=this->rightBarcodeSettings.maxLength;
          qobj_rightBarcodeSettings["minLength"]=this->rightBarcodeSettings.minLength;
          qobj_rightBarcodeSettings["prefix"]=this->rightBarcodeSettings.prefix;
          qobj_rightBarcodeSettings["sufix"]=this->rightBarcodeSettings.suffix;

          json["barcodeSettings_left"]=qobj_leftBarcodeSettings;
          json["barcodeSettings_right"]=qobj_rightBarcodeSettings;

          QJsonObject plcToolingInfo,thrusterItem,weldPointItem,stationItem,valveItem;
          QJsonArray thrusterArray,weldPointArray,stationArray,valveArray,stepStaionConnectionArray;
          plcToolingInfo["servoHomingDir"]=this->plcToolingInfo.servoHomingDir?"CW":"CCW";
          plcToolingInfo["servoSpeed_high"]=this->plcToolingInfo.servoSpeed_high;
          plcToolingInfo["servoSpeed_medium"]=this->plcToolingInfo.servoSpeed_medium;
          plcToolingInfo["servoSpeed_low"]=this->plcToolingInfo.servoSpeed_low;
          plcToolingInfo["toolingNO"]=this->plcToolingInfo.toolingNO;
          //qDebug()<<"write Json,555";
          QStringList  generator_enable_list;
          for(int i=0;i<8;i++)
          {
             if(i>0&&i<5)
              generator_enable_list.append(this->plcToolingInfo.generator_enable[i]?"TRUE":"FALSE");
             else
              generator_enable_list.append("FALSE");
          }

          plcToolingInfo["generator_enable"]=QJsonArray::fromStringList(generator_enable_list);

          //valve list to Json
          for(int i=0;i<7;i++)
          {
            valveItem["actionConfirm"]=this->plcToolingInfo.pneumaticValvelist[i].actionConfirm?"TRUE":"FALSE";
            valveItem["enable"]=this->plcToolingInfo.pneumaticValvelist[i].enable?"TRUE":"FALSE";
            valveItem["startStep"]=this->plcToolingInfo.pneumaticValvelist[i].startStep;
            valveItem["stopStep"]=this->plcToolingInfo.pneumaticValvelist[i].stopStep;
            valveItem["valveNO"]=this->plcToolingInfo.pneumaticValvelist[i].valveNO;
            valveItem["valveType"]=this->plcToolingInfo.pneumaticValvelist[i].valveType;
            if(!valveItem.isEmpty())
            {
                valveArray.append(valveItem);
                valveItem.remove("actionConfirm");
                valveItem.remove("enable");
                valveItem.remove("startStep");
                valveItem.remove("stopStep");
                valveItem.remove("valveNO");
                valveItem.remove("valveType");
            }

          }

          plcToolingInfo["pneumaticValvelist"]=valveArray;
          // station list to Json

          for(int i=0;i<17;i++)
          {
              stationItem["pos_lowerLimit"]=this->plcToolingInfo.station_List[i].pos_lowerLimit;
              stationItem["pos_setPoint"]=this->plcToolingInfo.station_List[i].pos_setPoint;
              stationItem["pos_upperLimit"]=this->plcToolingInfo.station_List[i].pos_upperLimit;
              stationItem["speedType"]=this->plcToolingInfo.station_List[i].speedType;
              stationItem["stationNO"]=this->plcToolingInfo.station_List[i].stationNO;
              if(!stationItem.isEmpty())
              {
                 stationArray.append(stationItem);
                 stationItem.remove("pos_lowerLimit");
                 stationItem.remove("pos_setPoint");
                 stationItem.remove("pos_upperLimit");
                 stationItem.remove("speedType");
                 stationItem.remove("stationNO");

              }
          }
          plcToolingInfo["station_List"]=stationArray;
          //stepStation conection to Json
          //qDebug()<<"write Json,608";
          for(int i=0;i<17;i++)
          {
              stepStaionConnectionArray.append(this->plcToolingInfo.stepStationConnection[i]);
          }
          plcToolingInfo["stepStationConnection"]=stepStaionConnectionArray;
          //thruster list to Json
          for(int i=0;i<17;i++)
          {
              thrusterItem["ChannelNO"]=this->plcToolingInfo.thruster_List[i].ChannelNO;
              thrusterItem["enable"]=this->plcToolingInfo.thruster_List[i].enable?"TRUE":"FALSE";
              thrusterItem["GenNO"]=this->plcToolingInfo.thruster_List[i].GenNO;
              thrusterItem["thrusterNO"]=this->plcToolingInfo.thruster_List[i].thrusterNO;
              if(!thrusterItem.isEmpty())
              {
                  thrusterArray.append(thrusterItem);
                  thrusterItem.remove("ChannelNO");
                  thrusterItem.remove("enable");
                  thrusterItem.remove("GenNO");
                  thrusterItem.remove("thrusterNO");
              }


          }
           plcToolingInfo["thruster_List"]=thrusterArray;
          // weld point list to Json
          for(int i=0;i<17;i++)
          {
            weldPointItem["enable"]=this->plcToolingInfo.weldPoint_List[i].enable?"TRUE":"FALSE";
            weldPointItem["GenNO"]=this->plcToolingInfo.weldPoint_List[i].GenNO;
            weldPointItem["pointNO"]=this->plcToolingInfo.weldPoint_List[i].pointNO;
            weldPointItem["stepNO"]=this->plcToolingInfo.weldPoint_List[i].stepNO;
            weldPointItem["ThrusterNO"]=this->plcToolingInfo.weldPoint_List[i].ThrusterNO;
            weldPointItem["thrusterPressure_down"]=this->plcToolingInfo.weldPoint_List[i].thrusterPressure_down;
            //weldPointItem["thrusterPressure_up"]=this->plcToolingInfo.weldPoint_List[i].thrusterPressure_up;
            if(true)
            {
                QJsonObject US_Para;
                US_Para["AfterBurstdelay0to999"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.AfterBurstdelay0to999;
                US_Para["Afterburst_Duration0t999"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Afterburst_Duration0t999;
                US_Para["Amplitude20to100"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Amplitude20to100;
                US_Para["B0_CommandByte"]=0;
                US_Para["B1_iQLinQCommStatus"]=0;
                US_Para["B20_trigByte_ProcLimits"]=0;
                US_Para["B21_EnableSuspectBadByte"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B21_EnableSuspectBadByte.byteChar;
                US_Para["B2_trigByte_ProcessControl"]=0;
                US_Para["B46_trigByteHW_Setup"]=0;
                US_Para["B47_freqLockHold"]=0;
                US_Para["B63_trigByteTrigBypower"]=0;
                US_Para["B64_enableTrigByPower"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.B64_enableTrigByPower.byteChar;
                US_Para["BadPart_MaxEnergy_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.BadPart_MaxEnergy_Limits;
                US_Para["BadPart_MaxPower_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.BadPart_MaxPower_Limits;
                US_Para["BadPart_MaxTime_limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.BadPart_MaxTime_limits;
                US_Para["BadPart_MinEnergy_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.BadPart_MinEnergy_Limits;
                US_Para["BadPart_MinPower_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.BadPart_MinPower_Limits;
                US_Para["Badpart_MinTime_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Badpart_MinTime_Limits;
                US_Para["Energy0to65535"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Energy0to65535;
                US_Para["Free_Run_Frequency"]=(int)this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Free_Run_Frequency;
                US_Para["Freq_Lower_Limit"]=(int)this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Freq_Lower_Limit;
                US_Para["Freq_Upper_Limit"]=(int)this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Freq_Upper_Limit;
                US_Para["Hold_TIme0to30000"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Hold_TIme0to30000;
                US_Para["Neg20_to_Pos20"]=0;
                US_Para["Peak_Power"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Peak_Power;
                US_Para["Ramp_Down_TIme0to250"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Ramp_Down_TIme0to250;
                US_Para["Ramp_up_Time20to1250"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Ramp_up_Time20to1250;
                US_Para["suspect_MinTime_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.suspect_MinTime_Limits;
                US_Para["Suspect_MinPower_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Suspect_MinPower_Limits;
                US_Para["Suspect_MinEnergy_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Suspect_MinEnergy_Limits;
                US_Para["Suspect_MaxTime_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Suspect_MaxTime_Limits;
                US_Para["Suspect_MaxPower_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Suspect_MaxPower_Limits;
                US_Para["Suspect_MaxEnergy_Limits"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Suspect_MaxEnergy_Limits;
                US_Para["Trigger_Amplitude"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Trigger_Amplitude;
                US_Para["Trigger_Power"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Trigger_Power;
                US_Para["Trigger_Timeout"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Trigger_Timeout;
                US_Para["Weld_Time0to30000"]=this->plcToolingInfo.weldPoint_List[i].ultrasonicPara.Weld_Time0to30000;
                weldPointItem["ultrasonicPara"]=US_Para;
            }
            weldPointArray.append(weldPointItem);
            weldPointItem.remove("enable");
            weldPointItem.remove("GenNO");
            weldPointItem.remove("pointNO");
            weldPointItem.remove("stepNO");
            weldPointItem.remove("ThrusterNO");
            weldPointItem.remove("thrusterPressure_down");
            //weldPointItem.remove("thrusterPressure_up");
            weldPointItem.remove("ultrasonicPara");

          }
          plcToolingInfo["weldPoint_List"]=weldPointArray;
          //part sensors bypass
          QStringList  sensor_bypass_list;
          for(int i=0;i<7;i++)
          {
             sensor_bypass_list.append(this->plcToolingInfo.partSensorBypass.extendBypass.bits.getBit(i)?"BYPASS":"ENABLE");
          }
          plcToolingInfo["partSensorBypass_G1"]=QJsonArray::fromStringList(sensor_bypass_list);
          sensor_bypass_list.clear();
          for(int i=0;i<7;i++)
          {
             sensor_bypass_list.append(this->plcToolingInfo.partSensorBypass.retractBypass.bits.getBit(i)?"BYPASS":"ENABLE");
          }
          plcToolingInfo["partSensorBypass_G2"]=QJsonArray::fromStringList(sensor_bypass_list);
          sensor_bypass_list.clear();
          //valve Sensor bypass
          QJsonObject qjsobj_valveSensorBypass;
          for(int j=0;j<7;j++)
          {
              sensor_bypass_list.clear();
              for(int k=0;k<7;k++)
              {
                  sensor_bypass_list.append(this->plcToolingInfo.valveSensorBypass[j].extendBypass.bits.getBit(k)?"BYPASS":"ENABLE");
              }
              qjsobj_valveSensorBypass["ExtendSensor_Bypass_Valve_"+QString::number(j)]=QJsonArray::fromStringList(sensor_bypass_list);
              sensor_bypass_list.clear();
              for(int l=0;l<7;l++)
              {
                  sensor_bypass_list.append(this->plcToolingInfo.valveSensorBypass[j].retractBypass.bits.getBit(l)?"BYPASS":"ENABLE");
              }
              qjsobj_valveSensorBypass["RetractSensor_Bypass_Valve_"+QString::number(j)]=QJsonArray::fromStringList(sensor_bypass_list);
              sensor_bypass_list.clear();
          }
          plcToolingInfo["valveSensorBypass"]=qjsobj_valveSensorBypass;
          //film feeder
          QJsonObject qjobj_filmfeeder;
          qjobj_filmfeeder["enable"]=this->plcToolingInfo.feeder.enable;
          qjobj_filmfeeder["direction"]=this->plcToolingInfo.feeder.direction?"CW":"CCW";
          qjobj_filmfeeder["distance_1"]=(qint32)this->plcToolingInfo.feeder.distance_1;
          qjobj_filmfeeder["distance_2"]=this->plcToolingInfo.feeder.distance_2;
          qjobj_filmfeeder["interval"]=this->plcToolingInfo.feeder.interval;
          qjobj_filmfeeder["speed"]=(qint32)this->plcToolingInfo.feeder.speed;
          plcToolingInfo["filmFeeder"]=qjobj_filmfeeder;
          json["plcToolingInfo"]=plcToolingInfo;
}
bool clsTooling::saveToDisk(QString fileName)
{
         QFile saveFile(fileName);
         if (!saveFile.open(QIODevice::WriteOnly)) {
             qWarning()<<tr("Couldn't open save file,fileName:%1").arg(fileName);
             //qWarning("Couldn't open save file.");
             return false;
         }
         else
         {
             qDebug()<<"file opened , file name:"<<fileName;
         }
         QJsonObject toolingSettings;
         qDebug()<<"before write Jason";
         this->writeJason(toolingSettings);
         qDebug()<<"after write Jason";
         QJsonDocument saveDoc(toolingSettings);
         saveFile.write(true? saveDoc.toJson()
             : saveDoc.toBinaryData());
         saveFile.close();
         return true;
}
bool clsTooling::loadFromDisk(QString fileName)
{
    QFile loadFile(fileName);

         if (!loadFile.open(QIODevice::ReadOnly)) {
             qWarning()<<tr("Couldn't open save file,fileName:%1").arg(fileName);
             //qWarning("Couldn't open save file.");
             return false;
         }

         QByteArray saveData = loadFile.readAll();
         loadFile.close();
         QJsonDocument loadDoc(true
             ? QJsonDocument::fromJson(saveData)
             : QJsonDocument::fromBinaryData(saveData));

         this->readJason(loadDoc.object());
         return true;
}
QByteArray clsTooling::prepareCommand_getGenEnableStatusFromPLC()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x65;//commandNO low byte,101
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte

    return dataToTcpCommObj;
}
QByteArray clsTooling::prepareCommand_getThrusterConfigFromPLC()
{
    QByteArray dataToTcpCommObj;
   dataToTcpCommObj[0]=0x00;//length high byte
   dataToTcpCommObj[1]=0x06;//length low byte
   dataToTcpCommObj[2]=0x00;//commandNO high byte
   dataToTcpCommObj[3]=0x68;//commandNO low byte,104
   dataToTcpCommObj[4]=0x00;//reserve byte
   dataToTcpCommObj[5]=0x00;//reserve byte
   return dataToTcpCommObj;
}
QByteArray clsTooling::prepareCommand_getPointUSParaFromPLC(quint8 pointNO, quint8 count)
{
    QByteArray ar2;
    ar2[0]=0x00;//length high byte
    ar2[1]=0x0A;//length low byte
    ar2[2]=0x00;//commandNO high byte
    ar2[3]=0x6B;//commandNO low byte,107
    ar2[4]=0x00;//reserve byte
    ar2[5]=0x00;//reserve byte
    ar2[6]=pointNO;//pointNO
    ar2[7]=count;//point count,can query more than 1 point's data
    ar2[8]=0x00;//reserve byte
    ar2[9]=0x00;//reserve byte
    return ar2;
}
QByteArray clsTooling::prepareCommand_getstationParaFromPLC()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x67;//commandNO low byte,103
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    return dataToTcpCommObj;
}
QByteArray clsTooling::prepareCommand_getStepStationConnectionFromPLC()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x6A;//commandNO low byte,106
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    return dataToTcpCommObj;
}
QByteArray clsTooling::prepareCommand_getValveConfigFromPLC()
{
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x6C;//commandNO low byte,108
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    return dataToTcpCommObj;
}
QByteArray clsTooling::prepareCommand_getFilmFeederParaFromPLC()
{
    //query film feeder  config
    QByteArray dataToTcpCommObj;
    dataToTcpCommObj[0]=0x00;//length high byte
    dataToTcpCommObj[1]=0x06;//length low byte
    dataToTcpCommObj[2]=0x00;//commandNO high byte
    dataToTcpCommObj[3]=0x7C;//commandNO low byte,124
    dataToTcpCommObj[4]=0x00;//reserve byte
    dataToTcpCommObj[5]=0x00;//reserve byte
    return dataToTcpCommObj;
}
QByteArray clsTooling::prepareCommand_uploadWholeSettingFromPLC()
{
    QByteArray ar1;
    ar1.append(this->prepareCommand_getGenEnableStatusFromPLC());
    qDebug()<<"ar1.size, after exec prepareCommand_getGenEnableStatusFromPLC"<<ar1.size();
    ar1.append(this->prepareCommand_getPointUSParaFromPLC(1,16));
    qDebug()<<"ar1.size, after exec prepareCommand_getPointUSParaFromPLC"<<ar1.size();
    ar1.append(this->prepareCommand_getstationParaFromPLC());
    qDebug()<<"ar1.size, after exec prepareCommand_getstationParaFromPLC"<<ar1.size();

    ar1.append(this->prepareCommand_getStepStationConnectionFromPLC());
    qDebug()<<"ar1.size, after exec prepareCommand_getStepStationConnectionFromPLC"<<ar1.size();

    ar1.append(this->prepareCommand_getThrusterConfigFromPLC());
    qDebug()<<"ar1.size, after exec prepareCommand_getThrusterConfigFromPLC"<<ar1.size();

    ar1.append(this->prepareCommand_getValveConfigFromPLC());
    qDebug()<<"ar1.size, after exec prepareCommand_getValveConfigFromPLC"<<ar1.size();
    ar1.append(this->prepareCommand_getFilmFeederParaFromPLC());
    qDebug()<<"ar1.size, after exec prepareCommand_getFilmFeederParaFromPLC"<<ar1.size();
    //get part sensor bypass info
    ar1.append(this->prepareCommand_getSensorBypassStatusFromPLC(0x01));
    qDebug()<<"after exec prepareCommand_getSensorBypassStatusFromPLC(0x01),get part sensor bypass info,ar1.size:"<<ar1.size();
    ar1.append(this->prepareCommand_getSensorBypassStatusFromPLC(0x02));
    qDebug()<<"after exec prepareCommand_getSensorBypassStatusFromPLC(0x02),get valve sensor bypass info,ar1.size:"<<ar1.size();

    return ar1;
}
QByteArray clsTooling::prepareCommand_set_get_toolID(bool set_get_flag)
{

    QByteArray dataToTcpCommObj;
    //get toolID from PLC
    if(!set_get_flag)
    {

        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7A;//commandNO low byte,122
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x00;//0==get,1==set
        dataToTcpCommObj[7]=0x00;//reserve byte
        dataToTcpCommObj[8]=0x00;//reserve byte
        dataToTcpCommObj[9]=0x00;//reserve byte
    }
    else
    {
        dataToTcpCommObj[0]=0x00;//length high byte
        dataToTcpCommObj[1]=0x0A;//length low byte
        dataToTcpCommObj[2]=0x00;//commandNO high byte
        dataToTcpCommObj[3]=0x7A;//commandNO low byte,122
        dataToTcpCommObj[4]=0x00;//reserve byte
        dataToTcpCommObj[5]=0x00;//reserve byte
        dataToTcpCommObj[6]=0x01;//0==get,1==set
        dataToTcpCommObj[7]=this->plcToolingInfo.toolingNO;//editting ID
        dataToTcpCommObj[8]=0x00;//reserve byte
        dataToTcpCommObj[9]=0x00;//reserve byte
    }
    return dataToTcpCommObj;
}
QByteArray clsTooling::prepareCommand_getSensorBypassStatusFromPLC(quint8 sensorGroup)
{

   QByteArray ar1;
   if(sensorGroup!=0x01&&sensorGroup!=0x02)
   {
       qDebug()<<"invalid command on getting sensor bypass info";
       return ar1;
   }
   ar1[0]=0x00;//length high byte
   ar1[1]=0x0C;//length low byte
   ar1[2]=0x00;//commandNO high byte
   ar1[3]=0x70;//commandNO low byte,112
   ar1[4]=0x00;//reserve byte
   ar1[5]=0x00;//reserve byte
   ar1[6]=0x02;//0x01=set,0x02=get
   ar1[7]=sensorGroup;//0x01=part sensor,0x02=valve sensor
   ar1[8]=0x00;//valve NO
   ar1[9]=0x00;//sensors enable status in Byte
   ar1[10]=0x00;//sensors enable status in Byte
   ar1[11]=0x00;//reserve
   return ar1;
}
QByteArrayList clsTooling::prepareCommand_downloadWholeSettingToPLC()
{
    QByteArrayList ar1;
    QByteArray ar2;
    //preparation for gen status downloading
    if(true)
    {
        ar2[0]=0x00;//length high byte
        ar2[1]=0x0A;//length low byte
        ar2[2]=0x00;//commandNO high byte
        ar2[3]=0x6F;//commandNO low byte,111
        ar2[4]=0x00;//reserve byte
        ar2[5]=0x00;//reserve byte
        bytebits bb1;
        bb1.byteChar=0;
        for(int i=1;i<=4;i++)
        {
            if(this->plcToolingInfo.generator_enable[i])
                bb1.bits.setBit(i);
            else
                bb1.bits.resetBit(i);
        }
        //bb1.bits.b1=this->plcToolingInfo.generator_enable[1];
        //bb1.bits.b2=this->plcToolingInfo.generator_enable[2];
        //bb1.bits.b3=this->plcToolingInfo.generator_enable[3];
        //bb1.bits.b4=this->plcToolingInfo.generator_enable[4];

        ar2[6]=bb1.byteChar;//gen enable/disable status
        ar2[7]=0x00;//reserve
        ar1.append(ar2);
    }
    //preparation for thruster config dowloading
    if(true)
    {
        ar2.clear();
        ar2[0]=0x00;//length high byte
        ar2[1]=0x0C;//length low byte
        ar2[2]=0x00;//commandNO high byte
        ar2[3]=0x72;//commandNO low byte,114
        ar2[4]=0x00;//reserve byte
        ar2[5]=0x00;//reserve byte
        ar2[6]=0x00;//thruster NO
        ar2[7]=0x00;//genNO
        ar2[8]=0x00;//channel NO
        ar2[9]=0x00;//enable/disable
        ar2[10]=0x00;//reserve byte
        ar2[11]=0x00;
        for(int i=1;i<17;i++)
        {
            ar2[6]=i;
            ar2[7]=this->plcToolingInfo.thruster_List[i].GenNO;
            ar2[8]=(this->plcToolingInfo.thruster_List[i].ChannelNO>0)?(this->plcToolingInfo.thruster_List[i].ChannelNO-1):0;
            ar2[9]=this->plcToolingInfo.thruster_List[i].enable?1:0;
            ar1.append(ar2);

        }

    }
    qDebug()<<"ar1.size after add thruster config ,before add weld point config para:"<<ar1.size();
    //preparation for weld point config&&US Para
    if(true)
    {
        ar2.clear();
        for(int i=1;i<17;i++)
        {
            //point config&&us para

            //qDebug()<<"ar1.size after add weld point config para ,before add point us para speed para:"<<ar1.size();

            ar2.clear();
            weldPoint wp2;
            wp2=this->plcToolingInfo.weldPoint_List[i];
            wp2.pointNO=i;
            wp2.ThrusterNO=this->plcToolingInfo.weldPoint_List[i].ThrusterNO;//thrusterNO
            wp2.stepNO=this->plcToolingInfo.weldPoint_List[i].stepNO;//stepNO;
            wp2.enable=this->plcToolingInfo.weldPoint_List[i].enable?1:0;//enable/disable
            wp2.thrusterPressure_down=this->plcToolingInfo.weldPoint_List[i].thrusterPressure_down*(3868-228)/500+228;
            //wp2.thrusterPressure_down=BigLittleSwap16(wp2.thrusterPressure_down);
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
            if(true)//important, make the vars inside it destroied automatically
            {
                weldPoint* wp1=&wp2;
                char *ptrCharData=(char*)wp1;
                //char* ptrCharData= (char*)(&wp2);
                QByteArray ar3(ptrCharData,sizeof(weldPoint));
                ar2[0]=0x00;//length high byte
                ar2[1]=0x56;//length low byte
                ar2[2]=0x00;//commandNO high byte
                ar2[3]=0x75;//commandNO low byte,117
                ar2[4]=0x00;//reserve byte
                ar2[5]=0x00;//reserve byte
                ar2.append(ar3);
                ar1.append(ar2);
                ar2.clear();
                ar3.clear();
                wp1=nullptr;
                ptrCharData=nullptr;
            }
        }

    }
    qDebug()<<"ar1.size after add weld point para ,before add servo speed para:"<<ar1.size();
    //preparation for Station setting downloading
    //servo para--speed setting
    if(true)
    {
        ar2.clear();
        ar2[0]=0x00;//length high byte
        ar2[1]=0x18;//length low byte
        ar2[2]=0x00;//commandNO high byte
        ar2[3]=0x77;//commandNO low byte,119
        ar2[4]=0x00;//reserve byte
        ar2[5]=0x00;//reserve byte
        dWordBytes dw1;
        dw1.DWordVar=this->plcToolingInfo.servoSpeed_high*5;//servo speed high
        ar2[6]=dw1.bytesVar.B0;
        ar2[7]=dw1.bytesVar.B1;
        ar2[8]=dw1.bytesVar.B2;
        ar2[9]=dw1.bytesVar.B3;
        dw1.DWordVar=this->plcToolingInfo.servoSpeed_medium*5;//servo speed medium
        ar2[10]=dw1.bytesVar.B0;
        ar2[11]=dw1.bytesVar.B1;
        ar2[12]=dw1.bytesVar.B2;
        ar2[13]=dw1.bytesVar.B3;
        dw1.DWordVar=this->plcToolingInfo.servoSpeed_low*5;//servo speed low
        ar2[14]=dw1.bytesVar.B0;
        ar2[15]=dw1.bytesVar.B1;
        ar2[16]=dw1.bytesVar.B2;
        ar2[17]=dw1.bytesVar.B3;
        dw1.DWordVar=0;//default speed;
        ar2[18]=dw1.bytesVar.B0;
        ar2[19]=dw1.bytesVar.B1;
        ar2[20]=dw1.bytesVar.B2;
        ar2[21]=dw1.bytesVar.B3;
        ar2[22]=this->plcToolingInfo.servoHomingDir?1:0;//homing Dir

        ar2[23]=0x00;
        ar1.append(ar2);
        ar2.clear();
        qDebug()<<"ar1.size after add servo speed para ,before add station info:"<<ar1.size();
    }

    // stations info

    if(true)
    {
        ar2[0]=0x00;//length high byte
        ar2[1]=0x18;//length low byte
        ar2[2]=0x00;//commandNO high byte
        ar2[3]=0x71;//commandNO low byte,113
        ar2[4]=0x00;//reserve byte
        ar2[5]=0x00;//reserve byte

        dWordBytes dw1;

        ar2[6]=0x00;//stationNO
        ar2[7]=0x00;//speedType
        dw1.DWordVar=0x00;//posSetPoint
        ar2[8]=dw1.bytesVar.B0;
        ar2[9]=dw1.bytesVar.B1;
        ar2[10]=dw1.bytesVar.B2;
        ar2[11]=dw1.bytesVar.B3;
        dw1.DWordVar=0x00;//posUpperLimit
        ar2[12]=dw1.bytesVar.B0;
        ar2[13]=dw1.bytesVar.B1;
        ar2[14]=dw1.bytesVar.B2;
        ar2[15]=dw1.bytesVar.B3;
        dw1.DWordVar=0x00;//posLowerLimit
        ar2[16]=dw1.bytesVar.B0;
        ar2[17]=dw1.bytesVar.B1;
        ar2[18]=dw1.bytesVar.B2;
        ar2[19]=dw1.bytesVar.B3;

        ar2[20]=0x00;
        ar2[21]=0x00;
        ar2[22]=0x00;
        ar2[23]=0x00;
        for(int i=1;i<17;i++)
        {
           ar2[6]=i;
           ar2[7]=this->plcToolingInfo.station_List[i].speedType;
           dw1.DWordVar=this->plcToolingInfo.station_List[i].pos_setPoint;//posSetPoint
           ar2[8]=dw1.bytesVar.B0;
           ar2[9]=dw1.bytesVar.B1;
           ar2[10]=dw1.bytesVar.B2;
           ar2[11]=dw1.bytesVar.B3;
           dw1.DWordVar=this->plcToolingInfo.station_List[i].pos_upperLimit;//posUpperLimit
           ar2[12]=dw1.bytesVar.B0;
           ar2[13]=dw1.bytesVar.B1;
           ar2[14]=dw1.bytesVar.B2;
           ar2[15]=dw1.bytesVar.B3;
           dw1.DWordVar=this->plcToolingInfo.station_List[i].pos_lowerLimit;//posLowerLimit
           ar2[16]=dw1.bytesVar.B0;
           ar2[17]=dw1.bytesVar.B1;
           ar2[18]=dw1.bytesVar.B2;
           ar2[19]=dw1.bytesVar.B3;
           //append ar2 to ar1
           ar1.append(ar2);

        }

    }
   qDebug()<<"ar1.size after add station info ,before add valve info:"<<ar1.size();
   //step station connection
   if(true)
   {

       ar2[0]=0x00;//length high byte
       ar2[1]=0x0A;//length low byte
       ar2[2]=0x00;//commandNO high byte
       ar2[3]=0x74;//commandNO low byte,116
       ar2[4]=0x00;//reserve byte
       ar2[5]=0x00;//reserve byte
       ar2[6]=0x00;//stepNO
       ar2[7]=0x00;//StationNO
       ar2[8]=0x00;
       ar2[9]=0x00;
       for(int i=1;i<17;i++)
       {
           ar2[6]=i;
           ar2[7]=this->plcToolingInfo.stepStationConnection[i];
           ar1.append(ar2);
       }
   }
   //preparation for Valve setting downloading
    if(true)
    {
        ar2.clear();
        ar2[0]=0x00;//length high byte
        ar2[1]=0x0C;//length low byte
        ar2[2]=0x00;//commandNO high byte
        ar2[3]=0x76;//commandNO low byte,118
        ar2[4]=0x00;//reserve byte
        ar2[5]=0x00;//reserve byte
        ar2[6]=0x00;//valveNO
        ar2[7]=0x00;//startStep
        ar2[8]=0x00;//stopStep
        ar2[9]=0x00;//actionConfirm
        ar2[10]=0x00;//reserve
        ar2[11]=0x00;//reserve
        for(int i=1;i<6;i++)
        {
            ar2[6]=i;
            ar2[7]=this->plcToolingInfo.pneumaticValvelist[i].startStep;
            ar2[8]=this->plcToolingInfo.pneumaticValvelist[i].stopStep;
            ar2[9]=this->plcToolingInfo.pneumaticValvelist[i].actionConfirm?1:0;
            ar2[10]=this->plcToolingInfo.pneumaticValvelist[i].enable?1:0;
            ar2[11]=this->plcToolingInfo.pneumaticValvelist[i].valveType;
            ar1.append(ar2);
        }

    }

    //preparation for film feeder setting downloading
    if(true)
    {
      ar2.clear();
      ar2[0]=0x00;//length high byte
      ar2[1]=0x18;//length low byte
      ar2[2]=0x00;//commandNO high byte
      ar2[3]=0x7B;//commandNO low byte,123
      ar2[4]=0x00;//reserve byte
      ar2[5]=0x00;//reserve byte
      ar2[6]=this->plcToolingInfo.feeder.enable;//filmfeeder Enable
      ar2[7]=this->plcToolingInfo.feeder.direction;//filmfeeder dir
      dWordBytes dw1;
      dw1.DWordVar=plcToolingInfo.feeder.speed;//filmfeeder speed
      ar2[8]=dw1.bytesVar.B0;
      ar2[9]=dw1.bytesVar.B1;
      ar2[10]=dw1.bytesVar.B2;
      ar2[11]=dw1.bytesVar.B3;
      dw1.DWordVar=this->plcToolingInfo.feeder.distance_1;//distance
      ar2[12]=dw1.bytesVar.B0;
      ar2[13]=dw1.bytesVar.B1;
      ar2[14]=dw1.bytesVar.B2;
      ar2[15]=dw1.bytesVar.B3;
      dw1.DWordVar=this->plcToolingInfo.feeder.distance_2;//distance_2
      ar2[16]=dw1.bytesVar.B0;
      ar2[17]=dw1.bytesVar.B1;
      ar2[18]=dw1.bytesVar.B2;
      ar2[19]=dw1.bytesVar.B3;
      ar2[20]=this->plcToolingInfo.feeder.interval;//interval
      ar2[21]=0;//reserve
      ar2[22]=0;//reserve
      ar2[23]=0;//reserve
      ar1.append(ar2);
    }
    //preparation for part sensor bypass
    if(true)
    {
        ar2.clear();
        ar2[0]=0x00;//length high byte
        ar2[1]=0x0C;//length low byte
        ar2[2]=0x00;//commandNO high byte
        ar2[3]=0x70;//commandNO low byte,112
        ar2[4]=0x00;//reserve byte
        ar2[5]=0x00;//reserve byte
        ar2[6]=0x01;//0x01=set,0x02=get
        ar2[7]=0x01;//0x01=part sensor,0x02=valve sensor
        ar2[8]=0x00;//valve NO
        ar2[9]=this->plcToolingInfo.partSensorBypass.extendBypass.byteChar;
        qDebug()<<tr("part sensor-G1 bypass status, byteChar:%1").arg((quint8)ar2[9]);
        ar2[10]=this->plcToolingInfo.partSensorBypass.retractBypass.byteChar;
        qDebug()<<tr("part sensor-G1 bypass status, byteChar:%1").arg((quint8)ar2[10]);
        ar2[11]=0x00;//reserve
        ar1.append(ar2);
    }
    //preparation for valve sensor bypass
    if(true)
    {
        ar2.clear();
        ar2[0]=0x00;//length high byte
        ar2[1]=0x0C;//length low byte
        ar2[2]=0x00;//commandNO high byte
        ar2[3]=0x70;//commandNO low byte,112
        ar2[4]=0x00;//reserve byte
        ar2[5]=0x00;//reserve byte
        ar2[6]=0x01;//0x01=set,0x02=get
        ar2[7]=0x02;//0x01=part sensor,0x02=valve sensor
        ar2[8]=0x00;//valve NO
        ar2[9]=0x00;//sensors enable status in Byte
        ar2[10]=0x00;//sensors enable status in Byte
        ar2[11]=0x00;//reserve

        for(int i=1;i<6;i++)
        {
            ar2[8]=i;
            ar2[9]=this->plcToolingInfo.valveSensorBypass[i].extendBypass.byteChar;
            ar2[10]=this->plcToolingInfo.valveSensorBypass[i].retractBypass.byteChar;
            qDebug()<<tr("valve[%1] sensor bypass, extend:%2,retract:%3").arg(i)
                      .arg((quint8)ar2[9]).arg((quint8)ar2[10]);
            ar1.append(ar2);
        }
    }
    //set toolID;
    if(true)
    {
        ar1.append(this->prepareCommand_set_get_toolID(true));
    }
    qDebug()<<"whole settings.size:"<<ar1.size();
    return ar1;
}
