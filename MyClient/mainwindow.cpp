#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    setupAuthUi();  // Создаём интерфейс
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::sendCommand(const QString& command)
{
    qDebug() << "Sending command:" << command;

    // Подключаемся, если ещё не подключены
    if (socket->state() != QAbstractSocket::ConnectedState) {
        socket->connectToHost("127.0.0.1", 54678);
        if (!socket->waitForConnected(2000)) {
            QString error = QString("ERROR: cannot connect to server (%1)").arg(socket->errorString());
            qDebug() << error;
            return error;
        }
        qDebug() << "Connected to server";

        // Читаем приветствие сервера
        socket->waitForReadyRead(500);
        QString greeting = socket->readAll();
        qDebug() << "Server greeting:" << greeting;
    }

    // Отправляем команду
    socket->write(command.toUtf8());
    if (!socket->waitForBytesWritten(2000)) {
        QString error = QString("ERROR: write failed (%1)").arg(socket->errorString());
        qDebug() << error;
        return error;
    }

    // Ждём ответ
    if (!socket->waitForReadyRead(3000)) {
        QString error = "ERROR: no response from server (timeout)";
        qDebug() << error;
        return error;
    }

    QString response = QString::fromUtf8(socket->readAll()).trimmed();
    qDebug() << "Response:" << response;
    return response;
}

void MainWindow::setupAuthUi()
{
    // Создаём виджеты
    auto *hostLabel = new QLabel("IP:");
    auto *portLabel = new QLabel("Port:");
    auto *loginLabel = new QLabel("Login:");
    auto *passwordLabel = new QLabel("Password:");
    auto *emailLabel = new QLabel("Email (for registration):");      // ✅ Добавлено
    auto *resetEmailLabel = new QLabel("Email (for reset):");        // ✅ Добавлено
    auto *newPasswordLabel = new QLabel("New password:");

    auto *hostEdit = new QLineEdit("127.0.0.1");
    auto *portEdit = new QLineEdit("54678");
    auto *loginEdit = new QLineEdit;
    auto *passwordEdit = new QLineEdit;
    auto *emailEdit = new QLineEdit;           // ✅ Поле для email при регистрации
    auto *resetEmailEdit = new QLineEdit;      // ✅ Поле для email при сбросе пароля
    auto *newPasswordEdit = new QLineEdit;

    passwordEdit->setEchoMode(QLineEdit::Password);
    newPasswordEdit->setEchoMode(QLineEdit::Password);

    // Добавляем подсказки (placeholder)
    loginEdit->setPlaceholderText("Enter login (min 3 chars)");
    passwordEdit->setPlaceholderText("Enter password (min 4 chars)");
    emailEdit->setPlaceholderText("user@example.com");
    resetEmailEdit->setPlaceholderText("user@example.com");
    newPasswordEdit->setPlaceholderText("New password (min 4 chars)");

    auto *registerBtn = new QPushButton("Register");
    auto *loginBtn = new QPushButton("Login");
    auto *helpBtn = new QPushButton("Help");          // ✅ Вместо Authorize
    auto *forgotBtn = new QPushButton("Forgot password");
    auto *responseBox = new QTextEdit;
    responseBox->setReadOnly(true);

    // Добавляем кнопку очистки лога
    auto *clearBtn = new QPushButton("Clear log");

    // Функция для отправки и отображения
    auto sendAndShow = [this, responseBox](const QString &command) {
        responseBox->append(">> " + command);
        const QString response = sendCommand(command + "\n");
        responseBox->append("<< " + response);
        responseBox->append("");  // Пустая строка для разделения
    };

    // ✅ Исправленная регистрация (с email)
    connect(registerBtn, &QPushButton::clicked, this, [=]() {
        // Валидация
        if (loginEdit->text().length() < 3) {
            responseBox->append("ERROR: Login must be at least 3 characters");
            return;
        }
        if (passwordEdit->text().length() < 4) {
            responseBox->append("ERROR: Password must be at least 4 characters");
            return;
        }
        if (!emailEdit->text().contains("@")) {
            responseBox->append("ERROR: Valid email is required");
            return;
        }

        QString cmd = QString("register||%1||%2||%3")
                          .arg(loginEdit->text())
                          .arg(passwordEdit->text())
                          .arg(emailEdit->text());
        sendAndShow(cmd);
    });

    // ✅ Логин (без изменений, но с валидацией)
    connect(loginBtn, &QPushButton::clicked, this, [=]() {
        if (loginEdit->text().isEmpty()) {
            responseBox->append("ERROR: Login is required");
            return;
        }
        if (passwordEdit->text().isEmpty()) {
            responseBox->append("ERROR: Password is required");
            return;
        }

        QString cmd = QString("login||%1||%2")
                          .arg(loginEdit->text())
                          .arg(passwordEdit->text());
        sendAndShow(cmd);
    });

    // ✅ Help команда (вместо authorize)
    connect(helpBtn, &QPushButton::clicked, this, [=]() {
        sendAndShow("help");
    });

    // ✅ Исправленный сброс пароля (с email)
    connect(forgotBtn, &QPushButton::clicked, this, [=]() {
        if (!resetEmailEdit->text().contains("@")) {
            responseBox->append("ERROR: Valid email is required for password reset");
            return;
        }
        if (newPasswordEdit->text().length() < 4) {
            responseBox->append("ERROR: New password must be at least 4 characters");
            return;
        }

        QString cmd = QString("forgot||%1||%2")
                          .arg(resetEmailEdit->text())
                          .arg(newPasswordEdit->text());
        sendAndShow(cmd);
    });

    // Очистка лога
    connect(clearBtn, &QPushButton::clicked, responseBox, &QTextEdit::clear);

    // Компоновка интерфейса
    auto *layout = new QGridLayout;
    int row = 0;

    // Строка 0: IP и Port
    layout->addWidget(hostLabel, row, 0);
    layout->addWidget(hostEdit, row, 1);
    layout->addWidget(portLabel, row, 2);
    layout->addWidget(portEdit, row, 3);

    // Строка 1: Login и Password
    row++;
    layout->addWidget(loginLabel, row, 0);
    layout->addWidget(loginEdit, row, 1);
    layout->addWidget(passwordLabel, row, 2);
    layout->addWidget(passwordEdit, row, 3);

    // Строка 2: Email для регистрации
    row++;
    layout->addWidget(emailLabel, row, 0);
    layout->addWidget(emailEdit, row, 1, 1, 3);

    // Строка 3: Email для сброса и новый пароль
    row++;
    layout->addWidget(resetEmailLabel, row, 0);
    layout->addWidget(resetEmailEdit, row, 1);
    layout->addWidget(newPasswordLabel, row, 2);
    layout->addWidget(newPasswordEdit, row, 3);

    // Строка 4: Кнопки
    row++;
    layout->addWidget(registerBtn, row, 0);
    layout->addWidget(loginBtn, row, 1);
    layout->addWidget(helpBtn, row, 2);
    layout->addWidget(forgotBtn, row, 3);

    // Строка 5: Кнопка очистки
    row++;
    layout->addWidget(clearBtn, row, 0, 1, 2);

    // Строка 6: Область вывода ответов
    row++;
    layout->addWidget(responseBox, row, 0, 1, 4);

    // Устанавливаем layout
    auto *container = new QWidget;
    container->setLayout(layout);
    setCentralWidget(container);

    // Устанавливаем заголовок окна
    setWindowTitle("TCP Client - Auth Demo");
    resize(800, 600);
}