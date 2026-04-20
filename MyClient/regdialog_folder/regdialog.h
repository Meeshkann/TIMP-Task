#ifndef REGDIALOG_H
#define REGDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include "ui_regdialog.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

    void setStatus(const QString &status, bool isError = true);

private slots:
    void onRegisterClicked();

private:
    Ui::RegisterDialog *ui;
    QTcpSocket *socket;

    QString sendCommand(const QString& command);
    bool validateInput();
};

#endif // REGDIALOG_H