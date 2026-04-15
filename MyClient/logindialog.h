#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <ui_logindialog.h>

namespace Ui{
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
        ~LoginDialog();

    QString getLogin() const;
    QString getPassword() const;

    void setStatus(const QString &status, bool is_error=true);

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    Ui::LoginDialog *ui;
    QTcpSocket *socket;

    QString sendcommand(const QString command);

};

#endif // LOGINDIALOG_H
