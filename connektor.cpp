#include "connektor.h"

Connektor::Connektor(server* serv, RequestMapper* mapper, QObject *parent) : QObject(parent)
{
    settingsControl *setControler = mapper->settingsController;
    CurrentPoint* point = mapper->currentPoint;
    sseCtrl* eventController = mapper->eventController;

    int countSens = serv->Sensors.count(); // сколько сенсоров существует
    QStringList listID = serv->Sensors.keys(); // получаем все ключи сенсоров из MAP массива

    // сигнал при приходе данных от сенсора  и слот в sse контроллере для передачи клиенту
    connect (point, SIGNAL (currentDataSignal(QJsonObject)),eventController,SLOT (currentDataSlot(QJsonObject)));

    // в цикле перебираем все обекты сенсоров (serv->Sensors) и подключаем к ним или от них сигралы
    for (int i = 0; i < countSens; ++i) {
        connect(serv->Sensors.value(listID[i]), SIGNAL(setPoint(QJsonObject, QJsonObject)), point, SLOT(setCurrenJSON(QJsonObject, QJsonObject)));
        connect(serv->Sensors.value(listID[i]), SIGNAL(settingsJSON(QJsonObject)), point, SLOT(setSettingsJSON(QJsonObject)));
        connect (point, SIGNAL (updateSetSensor()), serv->Sensors.value(listID[i]),SLOT (updateSet())); //принудительный запрос  настроек
        connect (setControler, SIGNAL (settingsSouse(QString)), serv->Sensors.value(listID[i]), SLOT(settingsSet(QString )));
    }
//    connect(serv->Sensors.value("id=1"), SIGNAL(setPoint(QJsonObject)), point, SLOT(setCurrenJSON(QJsonObject)));
}
