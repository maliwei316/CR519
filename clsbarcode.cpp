#include "clsbarcode.h"
#include <QTextStream>
#include <QDateTime>
#include <QSerialPort>
#include <QDebug>

clsBarcode::clsBarcode(QObject *parent) : QSerialPort(parent)
{
  //connect(this,&QSerialPort::readyRead,this,&clsBarcode::getBarcode);
  //connect(this,&QSerialPort::errorOccurred,this,&clsBarcode::hanldeError);
}
QString clsBarcode::getBarcode()
{
    if(this->bytesAvailable())
    {
        QString newGotBarcode(this->readAll());
        qDebug()<<"new got barcode:"<<newGotBarcode;
        return newGotBarcode;
    }
}
void clsBarcode::hanldeError(QSerialPort::SerialPortError)
{
    qDebug()<<"QSerialPort Error";
}
QString clsBarcode::autoGenerateBarcode(QString L_R)
{
  QString datetime=QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss");
  return L_R.append(datetime);
}
