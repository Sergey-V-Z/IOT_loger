#ifndef DATACONTROLLER_H
#define DATACONTROLLER_H
#include<QList>
#include<QString>
#include "httprequesthandler.h"
#include "descriptionofsensor.h"

using namespace stefanfrings;

class DataController : public HttpRequestHandler
{
    Q_OBJECT
public:
    DataController(QObject* parent=nullptr);
    void service(HttpRequest& request, HttpResponse& response,DescriptionOfSensor *descriptSensors );
private:
    QList<QString> list;
};
#endif // DATACONTROLLER_H
