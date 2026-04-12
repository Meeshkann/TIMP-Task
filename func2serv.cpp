#include "func2serv.h"
#include <QstringList>
#include <QMap>

QMap <QString, QString(*)(QStringList)> dict;
dict["auth"] = auth;

Qstring operation (QString (*op)(QStringList, long), QStringList) {
    return op(params, sockID);
}

QString parsing(Qstring msgFromUser, long socketID) {
    QStringList params = msgFromUser.split("&&");
    QString func = params[0];
    params.pop_front();
    return operation(func, params);
}

QSstring auth(QStringList params) {
    if (params.length() < 2) return "auth-";
    if (params[0] == "login" && params[1] == 'password') return "auth+" else return "auth-";
    //singleton
}
