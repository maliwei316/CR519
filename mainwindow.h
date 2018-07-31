#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <tcp_comm.h>
#include "bitsoperation.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();
signals:

    void sendDataToTCPCommObj(QByteArray dataToTCPCommObj);
public slots:
  void receiveDataFromTCPCommObj(QByteArray dataFromTcpCommObj);
private slots:

    void on_btn_setQtys_clicked();

    void on_btn_getQtys_clicked();

    void on_btn_QtyConfigUndo_clicked();

    void on_btn_setThrusterConfig_clicked();

    void on_btn_QueryThrusterConfig_2_clicked();

    void on_btn_undoThrusterConfig_clicked();

    void on_btn_setStationToStep_clicked();

    void on_btn_QueryStepStation_clicked();

    void on_btn_undoStepsStationEdit_clicked();

    void on_btn_setWeldPointConfig_clicked();

    void on_btn_QueryWeldPointConfig_clicked();

    void on_btn_undoWeldPointConfig_clicked();

    void on_btn_setPartClamperConfig_clicked();

    void on_btn_QueryClamperConfig_clicked();

    void on_btn_undoClamperConfig_clicked();

    void on_btn_setPointPara_clicked();

    void on_btn_getPointPara_clicked();

    void on_btn_undoPointPara_clicked();

    void on_btn_setServoPara_clicked();

    void on_btn_setStationPara_clicked();

    void on_btn_QuerystationPara_clicked();

    void on_actionTooling_Config_triggered();

    void on_actionPoint_Config_triggered();

    void on_btn_undoStationPara_clicked();

    void on_btn_undoServoSetting_clicked();

    void on_pushButton_servo_home_pressed();

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

private:
    Ui::MainWindow *ui;
public:
    weldPoint* wp1;


};

#endif // MAINWINDOW_H
