#ifndef CLSTOOLING_H
#define CLSTOOLING_H
#include <bitsoperation.h>
#include <QObject>

class clsTooling : public QObject
{
    Q_OBJECT
public:
    explicit clsTooling(QObject *parent = nullptr);
    void readJason(const QJsonObject &json);
    void writeJason(QJsonObject &json);
    bool saveToDisk(QString fileName);
    bool loadFromDisk(QString fileName);
    QByteArray prepareCommand_getGenEnableStatusFromPLC();
    QByteArray prepareCommand_getThrusterConfigFromPLC();
    QByteArray prepareCommand_getPointUSParaFromPLC(quint8 pointNO,quint8 count=1);
    QByteArray prepareCommand_getstationParaFromPLC();
    QByteArray prepareCommand_getStepStationConnectionFromPLC();
    QByteArray prepareCommand_getValveConfigFromPLC();
    QByteArray prepareCommand_uploadWholeSettingFromPLC();
    QByteArrayList prepareCommand_downloadWholeSettingToPLC();
signals:

public slots:
public:
    QString toolingName;
    QString toolingImageSource;
    QStringList pointNameMapping;
    QStringList valveNameMapping;
    toolingInfo_in_PLC plcToolingInfo;
};

#endif // CLSTOOLING_H
