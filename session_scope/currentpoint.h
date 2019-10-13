#ifndef CURRENTPOINT_H
#define CURRENTPOINT_H

#include "httprequesthandler.h"
#include"QJsonDocument"
#include"QJsonObject"
#include"QJsonArray"
#include"QSettings"
#include"QFileInfo"
#include <QFile>
#include<QDir>
#include<QDate>
#include<QDateTime>

using namespace stefanfrings;


class CurrentPoint : public HttpRequestHandler
{
    Q_OBJECT
public:
    CurrentPoint(QSettings *settings, QObject* parent=nullptr);
    ~CurrentPoint();
    void service(HttpRequest&  request, HttpResponse&  response);

    static int countObj;

public slots:
    void setCurrenJSON(QJsonObject currentPoint, QJsonObject nameFile);
    void setSettingsJSON(QJsonObject Settings);
signals:
    void updateSetSensor();
    void currentDataSignal(QJsonObject data);
private:
    static QJsonObject *currentJSON;
    static QJsonObject *currentSettings;
    static QJsonObject *currentFileNames;
    QFile *aliaseFile;
//    static QJsonArray *currentAllFiles;


    QSettings *settingSensors;
    QString logFolder;
    QStringList getFilesList(QString mask);
    QStringList sortDateList(QStringList list);
    QJsonObject  createAliase(QJsonObject settingsJSON);
    void writeAliasInFile(QJsonObject aliase);
};

#endif // CURRENTPOINT_H
