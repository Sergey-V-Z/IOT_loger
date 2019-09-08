#include "datacontroller.h"

DataController::DataController(QObject* parent): HttpRequestHandler (parent)
{

}

void DataController::service(HttpRequest &request, HttpResponse &response, DescriptionOfSensor *descriptSensors)
{
    QString path = request.getPath ();
    if (path == "/data/deskript") {
        QJsonDocument docJSON(descriptSensors->getDescriptionsFileJSON ()) ;
        response.write (docJSON.toJson (),true);
    }
}
