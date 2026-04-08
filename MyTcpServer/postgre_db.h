#ifndef POSTGRE_DB_H
#define POSTGRE_DB_H


#include "dbinterface.h"

class PostGreDB : public dbInterface
{
public:
    PostGreDB();
    ~PostGreDB();

    bool connect() override;
    void disconnect() override;
    bool regUser(const QString& login, const QString& password) override;
    bool authUser(const QString& login, const QString& password) override;
    bool existsUser(const QString& login) override;
    bool deleteUser(const QString& login) override;

private:
    QSqlDatabase db;

    // вспомогательные методы
    bool createTable();
    QString hashPassword(const QString& password);
};


#endif // POSTGRE_DB_H
