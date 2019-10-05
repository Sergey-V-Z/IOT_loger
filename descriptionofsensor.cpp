#include "descriptionofsensor.h"

DescriptionOfSensor::DescriptionOfSensor(QString pathFile)
{
    QString absolutPath;
    QFileInfo configFile (pathFile);
    absolutPath = QFileInfo(configFile.absolutePath(),pathFile).absoluteFilePath();
    getDataFromFile (absolutPath);
    this->path = absolutPath;
}

QString DescriptionOfSensor::getTemplates(QString id)
{
    if(templates.contains (id)){
        return templates.value (id, "non");
    }else {
        return "id not found";
    }
}

QStringList DescriptionOfSensor::keys()
{
    return description.keys ();
}

QJsonObject DescriptionOfSensor::getDescriptionJSON(QString id)
{
    if(descriptionSensors.contains (id)){
        return descriptionSensors.value (id);
    }else {
        QJsonObject emty;
        return emty;
    }
}

QJsonObject DescriptionOfSensor::getDescriptionsFileJSON()
{
    return description;
}

bool DescriptionOfSensor::setDescription(QByteArray rawJSON)
{
    QJsonObject newValidJSON = validateFileJSON(QJsonDocument::fromJson (rawJSON).object ());
    if(newValidJSON.isEmpty ()){
        qWarning() << "in function (setDescription): settableObj is empty ";
        return false;
    }
    QStringList newKeys = newValidJSON.keys ();
    for (int i = 0; i < newKeys.length (); ++i) {
        description.insert (newKeys[i], newValidJSON[newKeys[i]]);
    }

    //записываем проверенный объект в descriptionSensors
    QStringList keysObj = newValidJSON.keys ();
    for (int i = 0; i < keysObj.size (); ++i) {
        descriptionSensors.insert (keysObj[i], newValidJSON[keysObj[i]].toObject ());
    }

    //записать описание в файл
    QFile descriptFile(path);
    descriptFile.remove ();
    descriptFile.open (QFile::WriteOnly);
    QJsonDocument docJSON(description);
    descriptFile.write (docJSON.toJson ());
    descriptFile.close ();
    refresh ();
    return true;
}

void DescriptionOfSensor::refresh()
{
    getDataFromFile(this->path);
}

void DescriptionOfSensor::getDataFromFile(QString path)
{
    // чистим
    descriptionSensors.clear ();
    templates.clear ();

    // чтение из файла
    QFile descriptFile(path);
    descriptFile.open (QFile::ReadOnly);
    QJsonObject fileJSON = QJsonDocument::fromJson (descriptFile.readAll ()).object ();
    descriptFile.close ();

    //проверка данных
    fileJSON = validateFileJSON(fileJSON);
    if(fileJSON.isEmpty ()){
        qCritical() << "in function (getDataFromFile): read JSON file contains errors ";
        return;
    }
    description = fileJSON;

    // разбор данных
    QStringList keys = fileJSON.keys ();
    for (int i = 0; i < keys.length (); ++i) {
        QJsonObject sensorDescript = fileJSON[keys[i]].toObject ();
        descriptionSensors.insert (keys[i],sensorDescript);

        // создаем строковй шаблон из JSON объекта
        QString templateSens = sensorDescript["name"].toString ();
        QJsonArray keysOneSensor =  sensorDescript["keys"].toArray ();
        for (int a = 0; a < keysOneSensor.count (); ++a) {
            templateSens += ","+ keysOneSensor[a].toObject ()["name"].toString ();
        }
        templates.insert (keys[i],templateSens );
    }
}

// проверка на наличие обязательных ключей
bool DescriptionOfSensor::valadateDescriptJSON(QJsonObject verifiedJSON)
{
    if(verifiedJSON.contains ("name") and verifiedJSON.contains ("keys")and verifiedJSON.contains ("alias") ){
        if (!verifiedJSON["name"].isString ()) {
            return false;
        }
        if (!verifiedJSON["keys"].isArray ()) {
            return false;
        }
        if (!verifiedJSON["alias"].isObject ()) {
            return false;
        }

        QJsonObject alias = verifiedJSON["alias"].toObject ();
        QStringList keysAlias = alias.keys ();
        QJsonArray tempArrJSON = verifiedJSON["keys"].toArray ();
        //проверяем в цикле содержание ключей в обектах
        for (int i = 0; i < tempArrJSON.count (); ++i) {
            // если в элемент не объект
            if(!tempArrJSON[i].isObject ()){
                qCritical("in function (valadateDescriptJSON),val in arrey(keys:i=%i. is not object",i) ;
                return  false;
            }
            QJsonObject tempVal = tempArrJSON[i].toObject ();
            QStringList keysVal = tempVal.keys ();
            if(keysVal != keysAlias){
                qCritical() << "in function (valadateDescriptJSON), keys valye != keys alias";
                return  false;
            }
            // если в элементе нет ключа
            if (!tempVal.contains ("name")){
                qCritical() << "in function (valadateDescriptJSON), Obj in arrey 'keys' not contains necessary key 'name'";
                return  false;
            }
            if (!tempVal.contains ("display")){
                qCritical() << "in function (valadateDescriptJSON), Obj in arrey 'keys' not contains necessary key 'display'";
                return  false;
            }

        }
    }
    else {
        qCritical() << "in function (valadateDescriptJSON), tempObj not contains necessary keys 'name'or  'keys'";
        return false;
    }
    return  true;
}

// проверка всех настроек
QJsonObject DescriptionOfSensor::validateFileJSON(QJsonObject verifiedJSON)
{
    if(verifiedJSON.isEmpty ()){
        qCritical() << "in function (validateFileJSON)verifiedJSON is empty, bad parametr ";
        return verifiedJSON;
    }

    QStringList keysObj = verifiedJSON.keys ();
    QRegExp reg("id=\\d{1,3}");
    //отсеиваем обекты по id  и содержанию
    for (int a = 0; a < keysObj.size (); ++a) {
        // если ключ не валиден удаляем его
        if(!reg.exactMatch (keysObj[a])){
            verifiedJSON.remove (keysObj[a]);
            qWarning() << "in function (setDescription)bad key: " + keysObj[a];
        }else {
            //если данные в обекте не валидны удаляем
            if(!valadateDescriptJSON(verifiedJSON[keysObj[a]].toObject ())){
                verifiedJSON.remove (keysObj[a]);
            }
        }
    }
    return verifiedJSON;
}
