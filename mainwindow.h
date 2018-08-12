#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <tcp_comm.h>
#include "bitsoperation.h"
#include "clstooling.h"
#include <QTimer>
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

    ~MainWindow();
signals:

    void sendDataToTCPCommObj(QByteArray dataToTCPCommObj);
    void checkTcpConnectionStatus();
public slots:
  void receiveDataFromTCPCommObj(QByteArray dataFromTcpCommObj);
  void OnUploadWholeSettingsTimeout();

  void OnDownloadWholeSettingsTimeout();
  void OnTcpCommConnectionStateChanged(QAbstractSocket::SocketState state,quint8 ConnectionID);
  void OnTimer_mainWindow_Timeout();

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

    void on_servo_Jog1_btn_pressed();

    void on_servo_Jog1_btn_released();

    void on_servo_Jog2_btn_pressed();

    void on_servo_Jog2_btn_released();

    void on_servo_home_btn_pressed();

    void on_servo_home_btn_released();

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

    void on_spinBox_pointPara_upPressure_valueChanged(int arg1);

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

private:
    Ui::MainWindow *ui;
public:
    weldPoint* wp1;
    quint8 toolID_PLC;
    quint8 toolID_editing;

    clsTooling* tempTooling_editting;
    clsTooling* tooling_current;
    bool tcpConnectionStatus_send;
    bool tcpConnectionStatus_receive;
    bool modbusConnectionStatus;
    bool uploadingWholeSettingFromPLCInProcess;
    QTimer timer_mainWindow;
    bool checking_tcpConnectionStatus;

};

#endif // MAINWINDOW_H
