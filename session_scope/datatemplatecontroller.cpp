#include "datatemplatecontroller.h"
#include "template.h"
#include "requestmapper.h"

DataTemplateController::DataTemplateController(QObject* parent):HttpRequestHandler (parent)
{
}
void DataTemplateController::service(HttpRequest &request, HttpResponse &response) {

    HttpSession session=RequestMapper::sessionStore->getSession(request,response,true);
    QString username=session.get("username").toString();
    QByteArray language=request.getHeader("Accept-Language");

    qDebug("language=%s",qPrintable(language));

    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    QString path = request.getPath ();
    QStringList pathList = path.split ("/");
    QString namePage = pathList[pathList.size ()-1];

    Template t=RequestMapper::templateCache->getTemplate("menu",language);
    Template page = RequestMapper::templateCache->getTemplate (namePage,language);
    Template head = RequestMapper::templateCache->getTemplate (namePage + "Head",language);
    t.setVariable("page",page);// Добовляет в шаблон переменную
    t.setVariable("head",head);// Добовляет в шаблон переменную

//    t.setCondition("logged-in",!username.isEmpty()); // Включает часть кода которая соответствует условию

//    t.loop("row",list.size()); // Создает в памяти код из шаблона в количестве указанном во втором параметре
//        // В цикле заполняем (заменяем) созданный код
//        for(int i=0; i<list.size(); i++) {
//            QString number=QString::number(i);
//            QString name=list.at(i);
//            t.setVariable("row"+number+".number",number); // формируем строку в зависимости от номера и заменяем ее в шаблоне на содержание переменной
//            t.setVariable("row"+number+".name",name); // формируем строку в зависимости от номера и заменяем ее в шаблоне на содержание переменной
//        }
    response.write(t.toUtf8(),true);
}
