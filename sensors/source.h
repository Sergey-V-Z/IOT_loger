#ifndef SOURSE_H
#define SOURSE_H

#include <QObject>
#include <QDateTime>
#include"QSettings"
#include <QFile>
#include"QFileInfo"
#include"QDir"
#include <QDebug>
#include "QJsonObject"
#include "QJsonDocument"
#include "QJsonValue"
#include"QJsonArray"

class Source: public QObject{
    Q_OBJECT
public:

    //Контструктор принимает указатели на обект файла для записи и указатель на обьект (времени)
    Source(QString id, QSettings *settings, QDateTime *date, QObject *parent = nullptr);
    ~Source();

    //Возвращает ID в строковом формате
    QString getID();
    //Возвращает путь к файлу
    QString getPath();

    QStringList getNewSettings();
    void  clearNewSettings();
    QStringList getActualSettings();

    //Запись строки с настройками датчика
    bool saveSettings(QStringList settings);
    //Запись в файл
    void writeToFile(QString *buff);
    //Запись шаблонов имен переменных
    void setTemplate(QString templates);
    bool isConnect();
    bool needToUpdateSet();

private:
    void writeBoofToFile(QString data);
    bool NeedUpdateSet = false;
    bool Connect = false;
    //настройки датчиков
    int buffCount = 0;
    int currentBufferCount = 0;
    //буфер накопления данных от датчиков
    QString bufferData;

    QString currentFileName;
    QString ID;
    QStringList actualSettings;
    QStringList newSettings;
    QStringList templatesName;
    QString sensorName;
    QString Path;
    QString logFolder;
    QFile* log_file;
    QDateTime* Date;
    bool fileJSON = true;
    int getTemplateOfFile(QString file);
    QJsonObject parsString(QString raw);
public slots:
    void  settingsSet(QString set);
    void  updateSet();
signals:
    void setPoint(QJsonObject obj, QJsonObject nameFile);
    void settingsJSON(QJsonObject obj);
};

#endif // SOURSE_H
