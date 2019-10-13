#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QString>
#include "httplistener.h"
#include "session_scope/requestmapper.h"
#include "filelogger.h"
#include"sensors/server.h"
#include "connektor.h"
#include "descriptionofsensor.h"
using namespace stefanfrings;

QString searchConfigFile(QString nameFile);

static server* serverSensor;
static DescriptionOfSensor *descriptSensors;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QString configFileName=searchConfigFile("webapp.ini");
    QString descriptFileName=searchConfigFile("description.json");

    // Configure logging
    QSettings* logSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    logSettings->beginGroup("logging");
    RequestMapper::logger=new FileLogger(logSettings,10000,&app);
    RequestMapper::logger->installMsgHandler();

    // Log the library version
    qDebug("QtWebApp has version %s",getQtWebAppLibVersion());

    // Load the configuration file
    QSettings* listenerSettings=new QSettings(configFileName, QSettings::IniFormat, &app);
    listenerSettings->beginGroup("listener");

    // Session store
    QSettings* sessionSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    sessionSettings->beginGroup("sessions");
    RequestMapper::sessionStore=new HttpSessionStore(sessionSettings,&app);

    // Static file controller
    QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    fileSettings->beginGroup("files");
    RequestMapper::staticFileController=new StaticFileController(fileSettings,&app);

    // Configure template cache
    QSettings* templateSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    templateSettings->beginGroup("templates");
    RequestMapper::templateCache=new TemplateCache(templateSettings,&app);

    // Template Sensors
    //    QSettings* sensorTemplates=new QSettings(configFileName,QSettings::IniFormat,&app);
    //    sensorTemplates->beginGroup("templatSensors");

    // Configure server sensors
    QSettings* servSet=new QSettings(configFileName,QSettings::IniFormat,&app);
    servSet->beginGroup("servSensors");

    //Config loginBase
    QSettings* loginBase=new QSettings(configFileName,QSettings::IniFormat,&app);
    loginBase->beginGroup("loginBase");

    // Description Sensor
    descriptSensors = new DescriptionOfSensor(descriptFileName);

    // Start the HTTP server
    RequestMapper* mapper = new RequestMapper(descriptSensors, servSet, loginBase, &app);
    new HttpListener(listenerSettings, mapper , &app);

    //Server Sensors
    serverSensor = new server (descriptSensors, servSet, &app);
    new Connektor(serverSensor, mapper, &app); // объект для соеденения сигналов и слотов

    return app.exec();
}

/**
 * Поиск конфигурационного файла
 * @return The valid filename
 */
QString searchConfigFile(QString nameFile) {
    QString binDir=QCoreApplication::applicationDirPath();
    QString appName=QCoreApplication::applicationName();
    QFile file;
    file.setFileName(binDir+"/"+nameFile);
    if (!file.exists()) {
        file.setFileName(binDir+"/etc/"+nameFile);
        if (!file.exists()) {
            file.setFileName(binDir+"/../etc/"+nameFile);
            if (!file.exists()) {
                file.setFileName(binDir+"/../"+appName+"/etc/"+nameFile);
                if (!file.exists()) {
                    file.setFileName(binDir+"/../../"+appName+"/etc/"+nameFile);
                    if (!file.exists()) {
                        file.setFileName(binDir+"/../../../../../"+appName+"/etc/"+nameFile);
                        if (!file.exists()) {
                            file.setFileName(QDir::rootPath()+"etc/"+nameFile);
                        }
                    }
                }
            }
        }
    }
    if (file.exists()) {
        QString configFileName=QDir(file.fileName()).canonicalPath();
        qDebug("using config file %s", qPrintable(configFileName));
        return configFileName;
    }
    else {
        qFatal("config file not found");
    }
}
