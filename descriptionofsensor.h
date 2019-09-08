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
    QString getTemplates(QString id);
    QStringList keys();
    QJsonObject getDescriptionJSON(QString id);
    QJsonObject getDescriptionsFileJSON();
    bool setDescription(QByteArray rawJSON);
    void refresh();
private:
    void getDataFromFile(QString path);
    bool valadateDescriptJSON(QJsonObject verifiedJSON);
    QJsonObject validateFileJSON(QJsonObject verifiedJSON );

    QString path;
    QJsonObject description;
    QHash<QString, QJsonObject> descriptionSensors;
    QHash<QString, QString> templates;
};

#endif // DESCRIPTIONOFSENSOR_H
