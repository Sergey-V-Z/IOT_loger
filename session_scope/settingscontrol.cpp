#include "settingscontrol.h"

settingsControl::settingsControl(QObject *parent)
    : HttpRequestHandler(parent) {
    // empty
}

void settingsControl::service(HttpRequest &request, HttpResponse &response)
{
    QByteArray metod = request.getMethod ();

    if(metod == "POST" || metod == "post"){
       QString body(request.getBody ()) ;
       emit settingsSouse (body);
    }else {
        response.setStatus(405,"Method Not Allowed");
        response.write("only POST metod",true);
        qDebug() << "settingsControl Method Not Allowed";
    }

}
