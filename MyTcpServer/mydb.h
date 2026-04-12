#ifndef DB_HANDLER_H
#define DB_HANDLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QCryptographicHash>
#include <QDebug>
#include <QUuid>

class MyDBHandler : public QObject
{
    Q_OBJECT

public:
    MyDBHandler(QObject *parent = nullptr) : QObject(parent) {
        connectionName = QUuid::createUuid().toString(QUuid::WithoutBraces);
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName("Test.db");
        open();
        CreateTable();
    }

    ~MyDBHandler() {
        disconnect();
        if (!connectionName.isEmpty()) {
            const QString name = connectionName;
            connectionName.clear();
            db = QSqlDatabase();
            QSqlDatabase::removeDatabase(name);
        }
    }

    operator bool() const {
        return db.isOpen();
    }

    bool open() {
        if (!db.isOpen()) {
            if (!db.open()) {
                qDebug() << "Failed to open database:" << db.lastError().text();
                return false;
            }
        }
        return true;
    }

    void disconnect() {
        if (db.isOpen()) {
            db.close();
        }
    }

    bool regUser(const QString& login, const QString& password, const QString& email) {
        if (!open()) return false;

        if (existsEmail(email) || existsUser(login)) return false;

        QSqlQuery query(db);
        query.prepare("INSERT INTO Users(login, password, email) "
                      "VALUES (:login, :password, :email)");

        QString hashedPassword = HashPassword(password);

        query.bindValue(":login", login);
        query.bindValue(":password", hashedPassword);
        query.bindValue(":email", email);

        return query.exec();
    }

    bool authUser(const QString& login, const QString& password) {
        if (!open()) return false;

        QSqlQuery query(db);
        query.prepare("SELECT password FROM Users WHERE login = (:login)");
        query.bindValue(":login", login);

        if (!query.exec()) return false;

        if (query.next()) {
            QString storedHash = query.value(0).toString();
            QString inputHash = HashPassword(password);
            return storedHash == inputHash;
        }

        return false;
    }

    bool resetPasswordByEmail(const QString& email, const QString& newPassword) {
        if (!open()) return false;

        if (!existsEmail(email)) return false;

        QSqlQuery query(db);
        query.prepare("UPDATE Users SET password = (:password) WHERE email = (:email)");

        QString hashedPassword = HashPassword(newPassword);
        query.bindValue(":password", hashedPassword);
        query.bindValue(":email", email);

        return query.exec();
    }

    bool existsEmail(const QString& email) {
        if (!open()) return false;

        QSqlQuery query(db);
        query.prepare("SELECT email FROM Users WHERE email = (:email)");
        query.bindValue(":email", email);

        if (!query.exec()) return false;

        return query.next();
    }

    bool existsUser(const QString& login) {
        if (!open()) return false;

        QSqlQuery query(db);
        query.prepare("SELECT login FROM Users WHERE login = (:login)");
        query.bindValue(":login", login);

        if (!query.exec()) return false;

        return query.next();
    }

    bool deleteUser(const QString& login) {
        if (!open()) return false;

        QSqlQuery query(db);
        query.prepare("DELETE FROM Users WHERE login = (:login)");
        query.bindValue(":login", login);

        return query.exec();
    }

private:
    QSqlDatabase db;
    QString connectionName;

    QString HashPassword(const QString& password) {
        QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
        return QString(hash.toHex());
    }

    bool CreateTable() {
        QSqlQuery query(db);
        return query.exec("CREATE TABLE IF NOT EXISTS Users("
                          "login TEXT PRIMARY KEY, "
                          "password TEXT NOT NULL, "
                          "email TEXT UNIQUE NOT NULL"
                          ")");
    }
};

#endif // DB_HANDLER_H
