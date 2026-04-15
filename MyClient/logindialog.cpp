#include <logindialog.h>
#include <QMessageBox>
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    socket(new QTcpSocket(this))
{
    ui->setupUi(this);

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::getLogin() const
{
    return ui->loginEdit->text();
}

QString LoginDialog::getPassword() const
{
    return ui->passwordEdit->text();
}

void LoginDialog::setStatus(const QString &status, bool isError)
{
    ui->statusLabel->setText(status);
    if (isError) {
        ui->statusLabel->setStyleSheet("color: red;");
    } else {
        ui->statusLabel->setStyleSheet("color: green;");
    }
}

QString LoginDialog::sendcommand(const QString command)
{
    if (socket->state() != QAbstractSocket::ConnectedState)
    {
        socket->connectToHost("127.0.0.1",54678);
        if (!socket->waitForConnected(2000))
        {
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

void LoginDialog::onLoginClicked()
{
    QString login = getLogin();
    QString password = getPassword();

    if (login.isEmpty() || password.isEmpty())
    {
        setStatus("Заполните все поля");
        return;
    }

    setStatus("Проверка...", false);

    QString cmd = QString("login||%1||%2").arg(login, password);
    QString response = sendcommand(cmd + "\n");


    if (response.startsWith("SUCCESS")) {
        setStatus("Успешный вход!", false);
        accept();  // Закрываем диалог с кодом Accepted
    } else {
        setStatus("Ошибка: " + response);
    }

}


void LoginDialog::onRegisterClicked()
{
    done(2);  // код для открытия окна регистрации
}






