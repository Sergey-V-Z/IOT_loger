#ifndef SETTINGSCONTROL_H
#define SETTINGSCONTROL_H

#include "httprequesthandler.h"

using namespace stefanfrings;

class settingsControl : public HttpRequestHandler{
    Q_OBJECT

public:
    settingsControl(QObject* parent=nullptr);
    void service(HttpRequest& request, HttpResponse& response);

public slots:

signals:
    void settingsSouse(QString set);
    void settingsServer(QString set);

};

#endif // SETTINGSCONTROL_H
