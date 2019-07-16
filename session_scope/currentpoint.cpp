#include "currentpoint.h"
//#include "template.h"
//#include "requestmapper.h"

int CurrentPoint::countObj;
QJsonObject* CurrentPoint:: currentJSON;
QJsonObject* CurrentPoint:: currentSettings;
QJsonObject* CurrentPoint:: currentFileNames;
QJsonObject* CurrentPoint:: currentAllFiles;

CurrentPoint::CurrentPoint(QSettings *settings, QObject *parent):HttpRequestHandler (parent)
{
    settingSensors = settings;
    logFolder = settingSensors->value("logFolder").toString();

    //Если виртуальные "файлы" не созданны создаем их
    if(currentJSON  != nullptr){
    }else{currentJSON = new QJsonObject (); }

    if(currentSettings != nullptr){
    }else{ currentSettings= new QJsonObject ();}

    if(currentFileNames != nullptr){
    }else{ currentFileNames= new QJsonObject (); }

    if(currentAllFiles != nullptr){
    }else{ currentAllFiles= new QJsonObject (); }

    countObj++;
}

CurrentPoint::~CurrentPoint()
{
    delete currentJSON;
    delete currentSettings;

}

void CurrentPoint::service(HttpRequest &request, HttpResponse &response)
{
    QByteArray path =  request.getPath();
    QJsonDocument tempDoc;
    if(path == "/vf/settings"){
        tempDoc.setObject (*currentSettings);
        response.write(tempDoc.toJson() ,true);
    }
    else if (path == "/vf/curentpoint") {
        tempDoc.setObject (*currentJSON);
        response.write(tempDoc.toJson() ,true);
    }
    else if (path == "/vf/allFile") {
        // читаем все файлы в директории
        QStringList list = getFilesList ( "*.dat");
        for (int i = 0; i < list.count (); ++i) {
            currentAllFiles->insert (QString::number (i), "/" +  logFolder + "/" + list[i]);
        }
        tempDoc.setObject (*currentAllFiles);
        response.write(tempDoc.toJson() ,true);
    }
    else if (path == "/vf/listFile") {
        //  чтение файлов из папки и показ файлов с сегоднешней датой

        QDateTime date;
        QStringList list = getFilesList (date.currentDateTime().toString("dd.MM.yy")+ "*" + ".dat");
        QRegExp reg("id=\\d{0,3}");
        for (int i = 0; i < list.count (); ++i) {
            int pos = reg.indexIn (list[i]);
            if(pos != 0){
                QStringList find = reg.capturedTexts ();
                if(find.count ()> 1){continue;}
                currentFileNames->insert ( find[0], "/" + logFolder + "/" +list[i]);
            }else { continue;}
        }

        tempDoc.setObject (*currentFileNames);
        response.write(tempDoc.toJson() ,true);
    }
    else if (path == "/vf/updateSet") {
        emit updateSetSensor ();
    }
    else{
        response.setHeader("Content-Type", "text/html; charset=UTF-8");
        response.write("<html><body>");
        response.write("Vortual file list: <br> <a href=\"/vf/settings \">Settings<a>");
        response.write(" <br> <a href=\"/vf/curentpoint \">Curent point<a>");
        response.write("<br> <a href=\"/vf/listFile \">List log Files<a> ");
        response.write("<br> <a href=\"/vf/updateSet\">Update settings sensors<a> ");
        response.write("<br> <a href=\"/vf/allFile\">All Files in dir<a> ");

        //        response.write("<br> <a href=\"/vf/settings \">settings<a> ");
        response.write("</body></html>",true);

    }
    //    QByteArray str = "not point, ";
    //    str += QByteArray::number(countObj);


}

void CurrentPoint::setCurrenJSON(QJsonObject currentPoint, QJsonObject nameFile)
{
    // Добовляем в статические объекты данные если данные сушествуют они будут заменены новыми
    QJsonObject tempObj;

    QStringList listKey =  currentPoint.keys();
    int countKey = currentPoint.count ();
    for (int i = 0; i < countKey; ++i) {
        currentJSON->insert (listKey[i],currentPoint.value (listKey[i]));
    }

    QStringList list2Key =  nameFile.keys();
    int count2Key = nameFile.count ();
    for (int i = 0; i < count2Key; ++i) {
        currentFileNames->insert (list2Key[i],nameFile.value (list2Key[i]));
    }
    emit currentDataSignal (*currentJSON);

}

void CurrentPoint::setSettingsJSON(QJsonObject Settings)
{
    QStringList listKey =  Settings.keys();
    int countKey = Settings.count ();
    for (int i = 0; i < countKey; ++i) {
        currentSettings->insert (listKey[i],Settings.value (listKey[i]));
    }
}

// Ищит файлы по маске в заданной в настройках директории
QStringList CurrentPoint::getFilesList(QString mask)
{
    QDir mDir;
    QString path;
    QFileInfo configFile (settingSensors->value("logFile").toString());
    path = QFileInfo(configFile.absolutePath(),path).absoluteFilePath();
    mDir.setPath(path);
    return  mDir.entryList(QStringList()<< mask);
}

