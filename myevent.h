#ifndef MYEVENT_H
#define MYEVENT_H
#include <QEvent>
//#include <QtCore>
#include <QObject>
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
    myEvent_readDB(QEvent::Type myEventType, quint16 requestObjID,QString tableName,quint16 address):QEvent(myEventType)
    {
      this->tableName=tableName;
      this->requestObjID=requestObjID;
      this->address=address;
    }
public:
    QString tableName;
    quint16 requestObjID;
    quint16 address;
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
