#include "ssectrl.h"

sseCtrl::sseCtrl(QObject* parent): HttpRequestHandler (parent)
{
    dataReady = false;
}
void sseCtrl::service(HttpRequest &request, HttpResponse &response)
{
    qDebug()<< "SSE started" <<endl;
    QByteArray path = request.getPath();
    QHostAddress peer = request.getPeerAddress();

    if(path == "/sse" && request.getMethod() == "GET")
       {
           response.setHeader("Content-Type","text/event-stream;charset=utf-8");
           response.setHeader("Cache-Control","no-cache");
           response.setHeader("Connection","keep-alive");
           response.setHeader("Access-Control-Allow-Origin","*");   // also important , if not set , the html application wont run.

            while(response.isConnected())
            {
                QThread::msleep(2000);

                if(dataReady){
                    QJsonDocument temp;
                    temp.setObject (currentData);
                    QByteArray strDara = temp.toJson (QJsonDocument::Compact);
                    response.write("id: UndequenID\n");
                    response.write("event: add \n");
                    response.write("data: "+strDara+" \n\n");
                    response.flush(); //Очистить выходной буфер (основного сокета).
                    dataReady = false;
                }
            }
            qDebug()<< "End Transport"<< endl;
       }
       else
       {
           response.setStatus(404, "Not found");
           response.write("The URL is wrong, no such document",true);

    }
}

void sseCtrl::currentDataSlot(QJsonObject data)
{
    currentData=data;
    dataReady=true;

}
