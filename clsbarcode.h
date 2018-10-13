#ifndef CLSBARCODE_H
#define CLSBARCODE_H
#include <QSerialPort>
#include <QSerialPortInfo>
#include "bitsoperation.h"

class clsBarcode : public QSerialPort
{
    Q_OBJECT
public:
    explicit clsBarcode(QObject *parent = nullptr);
    QString autoGenerateBarcode(QString L_R="AUTO_");


signals:

public slots:

public:




};

#endif // CLSBARCODE_H
