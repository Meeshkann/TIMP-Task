#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../logindialog_folder/logindialog.h"
#include "../regdialog_folder/regdialog.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this))
    , isAuthenticated(false)
{
    ui->setupUi(this);

    // Скрываем основное окно до авторизации
    hide();

    // Показываем диалог логина
    showLoginDialog();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showLoginDialog()
{
    LoginDialog loginDialog(this);

    int result = loginDialog.exec();

    if (result == QDialog::Accepted) {
        // Успешный вход
        isAuthenticated = true;
        currentUser = loginDialog.getLogin();

        // Показываем основное окно
        show();

        // Настраиваем интерфейс для работы с функцией
        setupFunctionUi();

        // Подключаемся к серверу для основной работы
        connectToServer();

        statusBar()->showMessage("Добро пожаловать, " + currentUser);
    }
    else if (result == 2) {
        // Открываем регистрацию
        showRegisterDialog();
    }
    else {
        // Закрываем приложение
        close();
    }
}

void MainWindow::showRegisterDialog()
{
    RegisterDialog regDialog(this);

    if (regDialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Успех",
                                 "Регистрация завершена! Теперь войдите.");
        showLoginDialog();
    } else {
        showLoginDialog();
    }
}

void MainWindow::setupFunctionUi()
{
    // настройка графического представления
    // настройка слайдеров
    // настройка таблицы
    connect(ui->horizontalSlider, &QSlider::valueChanged,
            this, &MainWindow::onParameter1Changed);
    connect(ui->horizontalSlider_2, &QSlider::valueChanged,
            this, &MainWindow::onParameter2Changed);
    connect(ui->horizontalSlider_3, &QSlider::valueChanged,
            this, &MainWindow::onParameter3Changed);
    connect(ui->logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogout);

    // инициализация графика
    updateGraph();
}

void MainWindow::connectToServer()
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        socket->connectToHost("127.0.0.1", 54678);
        if (socket->waitForConnected(2000)) {
            socket->waitForReadyRead(500);
            socket->readAll();
            statusBar()->showMessage("Подключено к серверу", 2000);
        }
    }
}



void MainWindow::onParameter1Changed(int value)
{
    updateGraph();
}

void MainWindow::onParameter2Changed(int value)
{
    updateGraph();
}

void MainWindow::onParameter3Changed(int value)
{
    updateGraph();
}

void MainWindow::updateGraph()
{

    qDebug() << "Updating graph with parameters:"
             << ui->horizontalSlider->value()
             << ui->horizontalSlider_2->value()
             << ui->horizontalSlider_3->value();

    QString cmd = QString("calculate||%1||%2||%3")
                      .arg(ui->horizontalSlider->value())
                      .arg(ui->horizontalSlider_2->value())
                      .arg(ui->horizontalSlider_3->value());

    QString response = sendCommand(cmd);
    qDebug() << "Server response:" << response;

    updateTable(response);
}

void MainWindow::updateTable(const QString &data)
{
    ui->tableWidget->setRowCount(10);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels({"X", "Y"});

    // заполнение данными дальше...
}



QString MainWindow::sendCommand(const QString& command)
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        connectToServer();
    }

    socket->write((command + "\n").toUtf8());
    if (!socket->waitForBytesWritten(2000)) {
        return QString("ERROR: write failed");
    }
    if (!socket->waitForReadyRead(2000)) {
        return QString("ERROR: no response");
    }
    return QString::fromUtf8(socket->readAll()).trimmed();
}

void MainWindow::onLogout()
{
    isAuthenticated = false;
    currentUser = "";

    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }

    hide();
    showLoginDialog();
}
