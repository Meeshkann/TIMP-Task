#include "regdialog.h"
#include <QTimer>

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog),
    socket(new QTcpSocket(this))
{
    ui->setupUi(this);

    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::setStatus(const QString &status, bool isError)
{
    ui->statusLabel->setText(status);
    if (isError) {
        ui->statusLabel->setStyleSheet("color: red;");
    } else {
        ui->statusLabel->setStyleSheet("color: green;");
    }
}

bool RegisterDialog::validateInput()
{
    if (ui->loginEdit->text().length() < 3) {
        setStatus("Логин должен быть не менее 3 символов");
        return false;
    }
    if (ui->passwordEdit->text().length() < 4) {
        setStatus("Пароль должен быть не менее 4 символов");
        return false;
    }
    if (ui->passwordEdit->text() != ui->confirmPasswordEdit->text()) {
        setStatus("Пароли не совпадают");
        return false;
    }
    if (!ui->emailEdit->text().contains("@")) {
        setStatus("Введите корректный email");
        return false;
    }
    return true;
}

QString RegisterDialog::sendCommand(const QString& command)
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        socket->connectToHost("127.0.0.1", 54678);
        if (!socket->waitForConnected(2000)) {
            return QString("ERROR: cannot connect to server (%1)").arg(socket->errorString());
        }
        socket->waitForReadyRead(500);
        socket->readAll();
    }

    socket->write(command.toUtf8());
    if (!socket->waitForBytesWritten(2000)) {
        return QString("ERROR: write failed (%1)").arg(socket->errorString());
    }
    if (!socket->waitForReadyRead(2000)) {
        return QString("ERROR: no response (%1)").arg(socket->errorString());
    }
    return QString::fromUtf8(socket->readAll()).trimmed();
}

void RegisterDialog::onRegisterClicked()
{
    if (!validateInput()) return;

    setStatus("Регистрация...", false);

    QString cmd = QString("register||%1||%2||%3")
                      .arg(ui->loginEdit->text())
                      .arg(ui->passwordEdit->text())
                      .arg(ui->emailEdit->text());

    QString response = sendCommand(cmd + "\n");

    if (response.startsWith("SUCCESS")) {
        setStatus("Регистрация успешна! Теперь можно войти.", false);
        // Ждём 2 секунды и закрываем окно
        QTimer::singleShot(2000, this, &QDialog::accept);
    } else {
        setStatus("Ошибка: " + response);
    }
}