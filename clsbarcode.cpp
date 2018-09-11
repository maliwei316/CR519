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

QString clsBarcode::autoGenerateBarcode(QString L_R)
{
  QString datetime=QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss");
  return L_R.append(datetime);
}
