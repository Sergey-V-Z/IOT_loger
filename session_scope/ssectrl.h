#ifndef SSECTRL_H
#define SSECTRL_H

#include "httprequesthandler.h"
#include"QJsonDocument"
#include"QJsonObject"
#include"QJsonArray"
#include "QThread"

using namespace stefanfrings;

class sseCtrl : public HttpRequestHandler
{
        Q_OBJECT
public:
    sseCtrl(QObject* parent=nullptr);
    void service(HttpRequest& request, HttpResponse& response);
public slots:
    void currentDataSlot(QJsonObject data);
signals:

private:
    QJsonObject currentData;
    bool dataReady = false;
};

#endif // SSECTRL_H
