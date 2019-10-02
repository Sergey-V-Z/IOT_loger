#ifndef DESCRIPTIONOFSENSOR_H
#define DESCRIPTIONOFSENSOR_H
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QHash>
#include <QRegExp>

class DescriptionOfSensor
{
public:
    DescriptionOfSensor(QString pathFile);
    // возврощает шаблон описывающий данные в тойже последовательности что и приходящие от сенсора
    QString getTemplates(QString id);
    // возврощает ключи (id) всех сенсоров
    QStringList keys();
    //возврощает описание одного сенсора
    QJsonObject getDescriptionJSON(QString id);
    //возврощает описание всех сенсоров
    QJsonObject getDescriptionsFileJSON();
    // установка нового описания сенсора или сенсоров приномает  JSON в текстовом формате
    bool setDescription(QByteArray rawJSON);
    // обновляет данные читая их из файла
    void refresh();
private:
    // получает данные из файла
    void getDataFromFile(QString path);
    bool valadateDescriptJSON(QJsonObject verifiedJSON);
    QJsonObject validateFileJSON(QJsonObject verifiedJSON );

    QString path;
    QJsonObject description;
    QHash<QString, QJsonObject> descriptionSensors;
    QHash<QString, QString> templates;
};

#endif // DESCRIPTIONOFSENSOR_H
