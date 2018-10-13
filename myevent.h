#ifndef MYEVENT_H
#define MYEVENT_H
#include <QEvent>
//#include <QtCore>
#include <QObject>
#include <QSqlQuery>

#include <bitsoperation.h>
class myEvent_writeDB:public QEvent
{

public:
    myEvent_writeDB(QEvent::Type myEventType, QString sqlQuery):QEvent(myEventType)
    {
      this->sqlquery=sqlQuery;
    }
    //MyEvent(Type MyEventType):QEvent(MyEventType){}
public:
    QString sqlquery;
};
class myEvent_batchWriteDB:public QEvent
{

public:
    myEvent_batchWriteDB(QEvent::Type myEventType, QString prepareStr,QVariantList addressList,QVariantList valueList):QEvent(myEventType)
    {
      this->prepareStr=prepareStr;
      this->addressList=addressList;
        this->valueList=valueList;
    }
    //MyEvent(Type MyEventType):QEvent(MyEventType){}
public:
    QString prepareStr;
    QVariantList addressList;
    QVariantList valueList;
};

class myEvent_readDB:public QEvent
{

public:
    myEvent_readDB(QEvent::Type myEventType, QString sqlQuery,quint8 type):QEvent(myEventType)
    {
      this->sqlquery=sqlQuery;
      this->type=type;//1,search by barcode;2,search by time
    }
public:
    QString sqlquery;
    quint8  type;
};
class myEvent_updateDisplay:public QEvent
{

public:
    myEvent_updateDisplay(QEvent::Type myEventType, plcItem item):QEvent(myEventType)
    {
      myItem=item;
    }
public:
    plcItem myItem;
};
class myEvent_updateHistoryCycleData:public QEvent
{

public:
    myEvent_updateHistoryCycleData(QEvent::Type myEventType, QSqlQuery sql_cycleData,quint8 sql_cycleDataType):QEvent(myEventType)
    {
      cycleData=sql_cycleData;
      cycleDataType=sql_cycleDataType;
    }
public:
    QSqlQuery cycleData;
    quint8  cycleDataType;
};
//class Worker : public QObject
//{
// Q_OBJECT

//public:
// Worker() {
// connect(&timer, SIGNAL (timeout()), this, SLOT (doWork()));
// timer.start(1000);
// }

//private slots:
// void doWork() {
// /* … */
// }

//private:
// QTimer timer;
//};
//void controllerModbus::handleResults(const QString tableName,const quint16 startAddress, quint16 length)
// {
    //qDebug()<<"OK at controllerModbus.cpp line 27";
//       if(!q1.exec(QObject::tr("select address,value from %1 where address between %2 AND %3" ).arg(tableName)
//                   .arg(startAddress).arg(startAddress+length-1)))
//       {
//           qDebug()<<q1.lastError();
//       }
//       else
//       {
//           //qDebug()<<"q1.size:"<<q1.size();
//           while (q1.next())
//           {
//               int address= q1.value(0).toInt();
//               int value = q1.value(1).toInt();
//               qDebug()<<QString("data from database1  Address:%1  Value:%2").arg(address).arg(value);

//           }

//       }

// }

#endif // MYEVENT_H
