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
            if(descriptSensors->setDescription (request.getBody ())){
                response.setStatus (202, "OK");
                response.write ("");
            }else {
                response.setStatus (406, "Not Acceptable");
                response.write ("");
            }

        }
    }
}
