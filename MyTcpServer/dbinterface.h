#ifndef DBINTERFACE_H
#define DBINTERFACE_H

#include <qstring.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>


class dbInterface
{
public:
    virtual ~dbInterface() = default;

    virtual bool connect() = 0;
    virtual void disconnect() = 0;

    virtual bool regUser(const QString& login, const QString& password) = 0;
    virtual bool authUser(const QString& login, const QString& password) = 0;
    virtual bool existsUser(const QString& login) = 0;
    virtual bool deleteUser(const QString& login) = 0;
};


#endif // DBINTERFACE_H
