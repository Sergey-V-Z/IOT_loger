#include "datacontroller.h"

DataController::DataController(QObject* parent): HttpRequestHandler (parent)
{

}

void DataController::service(HttpRequest &request, HttpResponse &response, DescriptionOfSensor *descriptSensors)
{
    QString path = request.getPath ();
    QByteArray method = request.getMethod ();
    if (path == "/data/deskript") {
        if (method  == "GET") {
            QJsonDocument docJSON(descriptSensors->getDescriptionsFileJSON ()) ;
            response.write (docJSON.toJson (),true);

        }else if (method  == "POST") {
            descriptSensors->setDescription (request.getBody ());
        }
    }
}
