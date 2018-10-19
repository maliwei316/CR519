#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <tcp_comm.h>
#include "bitsoperation.h"
#include "clstooling.h"
#include <QTimer>
#include <QLabel>
#include "clsbarcode.h"
#include "db_handler.h"
#include <QProgressDialog>

typedef struct _pageInfo
{
        quint16 previousPage_Index_mainStackWidget;
        quint16 targetPage_Index_mainStackWidget;
        quint16 currentPage_Index_mainStackWidget;

}pageInfo;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    void setPLCValueVisible(bool flag);
    void updateWeldPoitDisplay_Editting(const weldPoint& wp2);
    void updateWeldPoitDisplay_Current(const weldPoint& wp2);
    void updateDisplay_All_EdittingValue();
    void updateDisplay_ALL_CurrentValue();
    void handleAlarm(plcItem item);
    void getBarcode_left();
    void getBarcode_right();
    void changePage(quint16 targetPageIndex);
    void handleBarcodeError(QSerialPort::SerialPortError error);
    void moveCycleDataToHistory();
    bool barcodeInit(clsBarcode* clsbarcode, const barcodeSetting& barcode_settings);
    void lockUnlockPLCFromHMI(bool lockFlag);
    void requestPointCycleDataFromPLC(dWordBytes pointsBits_dword);
    void startDownloadUploadWholeSettings(quint8 direction);
    void execDownloadUploadWholeSettings(quint8 cmdIndex,quint8 direction);
    ~MainWindow();
signals:
    void logRequest(QString logContents,quint16 logID,quint8 logLevel);
    void sendDataToTCPCommObj(QByteArray dataToTCPCommObj);
    void checkTcpConnectionStatus();
    void checkModbusConnectionStatus();
    void updateAlarmText(QString &alarmText);
    void moveAlarmToHistory(alarmItem item_alarm);
    void receivedPointCycleData(pointCycleData);
    void parameterEditableStausChanged(bool editable);
    void writeDatabaseRequired(QString sqlquery);
    void readDatabaseRequired(QString sqlquery,quint8 sqlType);
public slots:
  void receiveDataFromTCPCommObj(QByteArray dataFromTcpCommObj);

  void OnUploadWholeSettingsTimeout();
  void OnLogInTimeout();
  void onMoveAlarmToHistory(alarmItem alarm);
  void onReceivedPointCycleData(pointCycleData data1);
  void OnDownloadWholeSettingsTimeout();
  void OnTcpCommConnectionStateChanged(QAbstractSocket::SocketState state,quint8 ConnectionID);
  void OnTimer_mainWindow_Timeout();
  void OnPLCItemsChanged_Modbus(QVariantList changedItems);
  void onUpdateAlarmText(QString text);  
  void execLogging(QString logContents,quint16 logID,quint8 logLevel);
  void onParameterEditableStausChanged(bool editable);
private slots:


    void on_btn_setThrusterConfig_clicked();

    void on_btn_QueryThrusterConfig_2_clicked();

    void on_btn_undoThrusterConfig_clicked();

    void on_btn_setStationToStep_clicked();

    void on_btn_QueryStepStation_clicked();

    void on_btn_undoStepsStationEdit_clicked();

    void on_btn_setValveConfig_clicked();

    void on_btn_QueryValveConfig_clicked();

    void on_btn_undoValveConfig_clicked();

    void on_btn_setPointPara_clicked();

    void on_btn_getPointPara_clicked();


    void on_btn_setServoPara_clicked();

    void on_btn_setStationPara_clicked();

    void on_btn_QuerystationPara_clicked();

    void on_actionTooling_Config_triggered();



    void on_btn_undoStationPara_clicked();

    void on_btn_undoServoSetting_clicked();



    void on_actionWeld_By_Manual_triggered();

    void on_WeldByManual_PointNO_valueChanged(int arg1);

    void on_weldByManual_GOTO_clicked();

    void on_weldByManual_Down_clicked();

    void on_weldByManual_runWeld_clicked();

    void on_btn_pointPara_TEST_toggled(bool checked);



    void on_servo_enable_btn_clicked();

    void on_servo_disable_btn_clicked();

    void on_servoACKbtn_clicked();

    void on_servo_Jog1_btn_toggled(bool checked);

    void on_servo_home_btn_toggled(bool checked);

    void on_stationConfig_servo_GOTO_clicked();

    void on_btn_setGenEnable_clicked();

    void on_btn_getGenEnableStatus_clicked();

    void on_btn_undoGenEnable_clicked();

    void on_toolID_Editting_valueChanged(int arg1);

    void on_toolName_lineEdit_textEdited(const QString &arg1);

    void on_btn_selectToolingImage_clicked();

    void on_btn_saveToDisk_clicked();



    void on_toolID_fromPLC_valueChanged(int arg1);

    void on_checkBox_GenEnable_1_stateChanged(int arg1);

    void on_checkBox_GenEnable_2_stateChanged(int arg1);

    void on_checkBox_GenEnable_3_stateChanged(int arg1);

    void on_checkBox_GenEnable_4_stateChanged(int arg1);

    void on_spinBox_thrusterEnable_valueChanged(int arg1);

    void on_spinBox_GenNO_valueChanged(int arg1);

    void on_spinBox_channel_valueChanged(int arg1);

    void on_spinBox_weldPointEnable_valueChanged(int arg1);

    void on_spinBox_thrusterNOWPC_valueChanged(int arg1);

    void on_spinBox_stepNO_WPC_valueChanged(int arg1);

    void on_spinBox_pointPara_amp_valueChanged(int arg1);

    void on_spinBox_pointPara_trigAmp_valueChanged(int arg1);

    void on_spinBox_pointPara_trigPower_valueChanged(int arg1);

    void on_spinBox_pointPara_trigTimeout_valueChanged(int arg1);

    void on_spinBox_pointPara_weldTime_valueChanged(int arg1);

    void on_spinBox_pointPara_weldEnergy_valueChanged(int arg1);

    void on_spinBox_pointPara_peakPower_valueChanged(int arg1);

    void on_spinBox_pointPara_holdTime_valueChanged(int arg1);

    void on_spinBox_pointPara_trigByPower_valueChanged(int arg1);

    void on_spinbox_pointPara_freeRunFreq_valueChanged(int arg1);

    void on_spinbox_pointPara_freq_u_limit_valueChanged(int arg1);

    void on_spinbox_pointPara_freq_l_limit_valueChanged(int arg1);

    void on_spinBox_pointPara_rampupTime_valueChanged(int arg1);

    void on_spinBox_pointPara_rampdownTime_valueChanged(int arg1);

    void on_spinBox_pointPara_afterburst_duration_valueChanged(int arg1);

    void on_spinBox_pointPara_afterburstDelay_valueChanged(int arg1);

    void on_spinBox_pointPara_downPressure_valueChanged(int arg1);

    //void on_spinBox_pointPara_upPressure_valueChanged(int arg1);

    void on_spinBox_timeLowerLimit_BAD_valueChanged(int arg1);

    void on_spinBox_powerLowerLimit_BAD_valueChanged(int arg1);

    void on_spinBox_energyLowerLimit_BAD_valueChanged(int arg1);

    void on_spinBox_timeLowerLimit_Suspect_valueChanged(int arg1);

    void on_spinBox_powerLowerLimit_Suspect_valueChanged(int arg1);

    void on_spinBox_energyLowerLimit_Suspect_valueChanged(int arg1);

    void on_spinBox_timeUpperLimit_Suspect_valueChanged(int arg1);

    void on_spinBox_powerUpperLimit_Suspect_2_valueChanged(int arg1);

    void on_spinBox_energyUpperLimit_Suspect_2_valueChanged(int arg1);

    void on_spinBox_timeUpperLimit_BAD_valueChanged(int arg1);

    void on_spinBox_powerUpperLimit_BAD_valueChanged(int arg1);

    void on_spinBox_energyUpperLimit_BAD_valueChanged(int arg1);

    void on_checkBox_LimitsEnable_BAD_stateChanged(int arg1);

    void on_checkBox_LimitsEnable_SUSPECT_stateChanged(int arg1);

    void on_stationPara_upperLimit_valueChanged(int arg1);

    void on_stationPara_lowerLimit_valueChanged(int arg1);

    void on_stationPara_setPoint_valueChanged(int arg1);

    void on_stationPara_speedType_currentIndexChanged(int index);

    void on_stationPara_HighSpeed_valueChanged(int arg1);

    void on_stationPara_MediumSpeed_valueChanged(int arg1);

    void on_stationPara_LowSpeed_valueChanged(int arg1);

    void on_servoPara_CWCCW_currentIndexChanged(int index);

    void on_spinBox_stationNO_valueChanged(int arg1);

    void on_PointName_display_textEdited(const QString &arg1);

    void on_spinBox_PointNO_pointPara_valueChanged(int arg1);

    void on_spinBox_thrusterNO_valueChanged(int arg1);

    void on_spinBox_stepNO_valueChanged(int arg1);

    void on_valveName_lineEdit_textEdited(const QString &arg1);

    void on_spinBox_valveStartStep_valueChanged(int arg1);

    void on_spinBox_valveNO_valueChanged(int arg1);

    void on_spinBox_valveEnable_valueChanged(int arg1);

    void on_comboBox_valveType_currentIndexChanged(int index);

    void on_spinBox_valveActionConfirm_valueChanged(int arg1);

    void on_spinBox_valveEndStep_valueChanged(int arg1);

    void on_btn_setToAllPoints_clicked();

    void on_btn_uploadFromPLC_clicked();

    void on_stationPara_stationNO_valueChanged(int arg1);

    void on_btn_downLoadToPLC_clicked();



    void on_btn_PLC2Editting_WeldPoint_clicked();

    void on_pushButton_Calc_ItemID_clicked();

    void on_pushButton_registText_clicked();

    void on_spinBox_textID_reg_editingFinished();

    void on_pushButton_registText_remove_clicked();

    void on_pushButton_weldByManual_checkAlarm_clicked();

    void on_pushButton_leaveAlarmPage_clicked();

    void on_comboBox_barcode_assignment_currentIndexChanged(const QString &arg1);

    void on_checkBox_barcode_left_enable_stateChanged(int arg1);

    void on_checkBox_barcode_right_enable_stateChanged(int arg1);

    void on_comboBox_BarcodePort_Left_currentIndexChanged(const QString &arg1);

    void on_comboBox__BarcodePort_Right_currentIndexChanged(const QString &arg1);

    void on_comboBox_Baud_currentIndexChanged(const QString &arg1);

    void on_spinBox_barcodeDataBits_valueChanged(int arg1);

    void on_spinBox_barcodeStopBits_valueChanged(int arg1);

    void on_comboBox_parity_currentIndexChanged(const QString &arg1);

    void on_spinBox_barcodeMaxLength_valueChanged(int arg1);

    void on_spinBox_BarcodeMinLength_valueChanged(int arg1);

    void on_lineEdit_barcode_prefix_textEdited(const QString &arg1);

    void on_lineEdit_barcode_suffix_textEdited(const QString &arg1);

    void on_pushButton_toolingSettingRefresh_clicked();

    void on_spinBox_filmFeeder_enable_valueChanged(int arg1);

    void on_spinBox_filmFeeder_speed_valueChanged(int arg1);

    void on_spinBox_filmFeeder_distance_valueChanged(int arg1);

    void on_spinBox_filmFeeder_distance_2_valueChanged(int arg1);

    void on_spinBox_filmFeeder_Interval_valueChanged(int arg1);

    void on_comboBox_filmFeeder_Dir_currentIndexChanged(int index);

    void on_pushButton_toolingSettingSearch_clicked();

    void on_checkBox_PartSensor_1_stateChanged(int arg1);

    void on_checkBox_PartSensor_2_stateChanged(int arg1);

    void on_checkBox_PartSensor_3_stateChanged(int arg1);

    void on_checkBox_PartSensor_4_stateChanged(int arg1);

    void on_checkBox_PartSensor_5_stateChanged(int arg1);

    void on_checkBox_PartSensor_6_stateChanged(int arg1);

    void on_btn_Edit2PLC_FilmFeeder_clicked();

    void on_btn_refresh_FilmFeeder_clicked();

    void on_btn_PLC2Edit_FilmFeeder_clicked();

    void on_Clamper_Extend_clicked();

    void on_Clamper_Free_clicked();

    void on_Clamper_Retract_clicked();

    void on_actionautoRun_triggered();

    void on_ToolingChange_valveNO_valueChanged(int arg1);

    void on_Clamper_Extend_toolingChange_clicked();

    void on_Clamper_Free_ToolingChange_clicked();

    void on_Clamper_Retract_ToolingChange_clicked();

    void on_pushButton_Advance_clicked();

    void on_btn_alarmPage_clicked();

    void on_btn_IOTable_clicked();

    void on_btn_IO_Table_Quit_clicked();


    void on_btn_Edit2PLC_PartSensor_clicked();

    void on_btn_refresh_PartSensor_clicked();

    void on_btn_PLC2Edit_PartSensor_clicked();

    void on_sensorBypass_valveNO_valueChanged(int arg1);

    void on_btn_refresh_valveSensor_clicked();

    void on_btn_Edit2PLC_valveSensor_clicked();

    void on_btn_PLC2Edit_valveSensor_clicked();

    void on_checkBox_PartSensor_7_stateChanged(int arg1);

    void on_checkBox_PartSensor_8_stateChanged(int arg1);

    void on_checkBox_PartSensor_9_stateChanged(int arg1);

    void on_checkBox_PartSensor_10_stateChanged(int arg1);

    void on_checkBox_PartSensor_11_stateChanged(int arg1);

    void on_checkBox_PartSensor_12_stateChanged(int arg1);

    void on_checkBox_ValveExtendSensorBypass_1_stateChanged(int arg1);

    void on_checkBox_ValveExtendSensorBypass_2_stateChanged(int arg1);

    void on_checkBox_ValveExtendSensorBypass_3_stateChanged(int arg1);

    void on_checkBox_ValveExtendSensorBypass_4_stateChanged(int arg1);

    void on_checkBox_ValveExtendSensorBypass_5_stateChanged(int arg1);

    void on_checkBox_ValveExtendSensorBypass_6_stateChanged(int arg1);

    void on_checkBox_ValveRetractSensorBypass_1_stateChanged(int arg1);

    void on_checkBox_ValveRetractSensorBypass_2_stateChanged(int arg1);

    void on_checkBox_ValveRetractSensorBypass_3_stateChanged(int arg1);

    void on_checkBox_ValveRetractSensorBypass_4_stateChanged(int arg1);

    void on_checkBox_ValveRetractSensorBypass_5_stateChanged(int arg1);

    void on_checkBox_ValveRetractSensorBypass_6_stateChanged(int arg1);

    void on_pushButton_logIN_leave_clicked();

    void on_pushButton_logIN_GO_clicked();
    void on_stackedWidget_mainProgram_currentChanged(int arg1);
    void on_btn_filmfeeder_RunRelative_clicked();
    void on_btn_filmfeeder_RunRelative__weldByManual_clicked();
    void on_btn_filmfeeder_JOG1_weldByManual_toggled(bool checked);
    void on_btn_filmfeeder_JOG2_weldByManual_toggled(bool checked);
    void on_btn_filmfeeder_RunSpeed__weldByManual_toggled(bool checked);
    void on_btn_filmfeeder_JOG1_toggled(bool checked);
    void on_btn_filmfeeder_JOG2_toggled(bool checked);
    void on_btn_filmfeeder_RunSpeed_toggled(bool checked);

    void on_actionWeldByManual_triggered();

    void on_actionToolingChange_triggered();

    void on_btn_QueryServoPara_clicked();

    void on_btn_toLoadPos_clicked();

    void on_btn_CheckAlarm_clicked();

    void on_btn_filmfeeder_RunRelative_2_clicked();

    void on_btn_filmfeeder_RunRelative__weldByManual_2_clicked();

    void on_btn_leave_toolingConfig_clicked();

    void on_btn_toToolingChange_clicked();

    void on_btn_toToolingGonfig_clicked();

    void on_btn_leave_toolingChange_clicked();

    void on_btn_pointBypass_toPLC_clicked();

    void on_btn_pointBypass_fromPLC_clicked();

    void on_btn_pointBypass_refresh_clicked();

    void on_btn_pointBypass_clicked();

    void on_btn_run_monitor_clicked();

    void on_pushButton_leaveMachineInfo_clicked();

    void on_pushButton_saveMachineInfo_clicked();

    void on_actionMachine_Infomation_triggered();


    void on_pushButton_registText_import_clicked();

    void on_pushButton_registText_export_clicked();

    void on_pushButton_language_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clearHistoryAlarm_clicked();

    void on_btn_partCounterReset_clicked();

    void on_servo_Jog2_btn_toggled(bool checked);


    void on_tab_stepStation_currentChanged(int index);

    void on_tab_toolingConfig_currentChanged(int index);

    void on_btn_historyData_clicked();

    void on_pushButton_chooseDatabase_clicked();

    void on_pushButton_historyCycleData_search_clicked();

    void on_tableView_partCycleData_clicked(const QModelIndex &index);

    void on_pushButton_paraChecking_clicked();

    void on_pushButton_removeImage_clicked();

    void on_pushButton_MachineInfo_login_clicked();

    void on_actionExit_triggered();

    void on_pushButton_historyCycleData_export_clicked();

    void on_btn_IO_Table_Previos_clicked();

    void on_btn_IO_Table_Next_clicked();

    void on_btn_realTimeData_clicked();

    void on_WeldByManual_valveNO_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    void customEvent(QEvent *e); //该函数是父类QObject的虚函数
    void updatePlcItemDisplayEventHandler(QEvent *e);
    void updatePLCItem(plcItem item);
    void switchItemOnOff(QLabel *targetLabel, bool onOff,bool alarmFlag=false);
    void setPixmapForLabel(QLabel* label,QString imageSource);

public:
    machineInfo machineInfo1;
    weldPoint* wp1;
    quint8 toolID_PLC;
    quint8 toolID_editing;

    clsTooling* tempTooling_editting;
    clsTooling* tooling_current;
    bool ignoreToolIDConflict=false;
    bool isShowing_toolIDConflictPopUpDialog;
    bool tcpConnectionStatus_send;
    bool tcpConnectionStatus_receive;
    bool modbusConnectionStatus_502;
    bool modbusConnectionStatus_503;
    bool modbusConnectionStatus_504;
    bool uploadingWholeSettingFromPLCInProcess;
    QTimer timer_mainWindow;
    bool machineInfoReady;
    bool checking_tcpConnectionStatus;
    bool checking_modbusConnectionStatus;
    QMap<quint32,alarmItem> currentAlarms;
    QMap<quint32,QString> systemRegisteredTextList;
    clsBarcode *clsBarcode_left;
    clsBarcode *clsBarcode_right;
    pageInfo pageInfo1;

    QString barcode_previous_left;
    QString barcode_in_use_left;
    QString barcode_to_use_left;

    QString barcode_previous_right;
    QString barcode_in_use_right;
    QString barcode_to_use_right;
    bool logInStatus;
    bool parameterEditable;
    QStringList recentRunningLogs;
    plcVarTable plcVars;
    quint8 LanguageFlag;
    partCycleData cycleData_leftPart;
    partCycleData cycleData_rightPart;
    QByteArrayList downloadWholeSettingArrayList;
    QByteArrayList uploadWholeSettingArrayList;
    QProgressDialog* ptrprogressDialog;
    DB_Handler dbh_1;
    QSqlQueryModel *model_partData;
    QSqlQueryModel *model_pointsData;

    dWordBytes pointCycleDataStatus_p1_p32;

};

#endif // MAINWINDOW_H
