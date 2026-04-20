#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "regdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QGraphicsTextItem>

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
    ui->a_argument_label->setText(QString::number(value));
}

void MainWindow::onParameter2Changed(int value)
{
    updateGraph();
    ui->b_argument_label->setText(QString::number(value));
}

void MainWindow::onParameter3Changed(int value)
{
    updateGraph();
    ui->c_argument_label->setText(QString::number(value));
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
    drawGraph(response);   // <-- добавляем вызов
}

void MainWindow::updateTable(const QString &data)
{
    ui->tableWidget->setRowCount(10);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels({"X", "Y"});


    QString trimmed = data;
    if (trimmed.startsWith("SUCCESS:"))
        trimmed = trimmed.mid(8);

    QStringList points = trimmed.split(';', Qt::SkipEmptyParts);
    ui->tableWidget->setRowCount(points.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels({"X", "Y"});

    for (int i = 0; i < points.size(); ++i) {
        QStringList xy = points[i].split(',');
        if (xy.size() == 2) {
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(xy[0]));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(xy[1]));
        }
    }
}
void MainWindow::drawGraph(const QString &data)
{
    // Очищаем предыдущую сцену
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    // Фиксируем границы сцены от -10 до 10
    const double MIN_X = -10.0;
    const double MAX_X = 10.0;
    const double MIN_Y = -10.0;
    const double MAX_Y = 10.0;

    // Настраиваем сцену с фиксированными границами
    QRectF sceneRect(MIN_X, MIN_Y, MAX_X - MIN_X, MAX_Y - MIN_Y);
    scene->setSceneRect(sceneRect);

    // Настраиваем вид для центрирования
    ui->graphicsView->setSceneRect(sceneRect);
    ui->graphicsView->fitInView(sceneRect, Qt::KeepAspectRatio);

    // Рисуем оси
    QPen axisPen(Qt::black, 0.1);

    // Ось X (y=0)
    QLineF xAxis(MIN_X, 0, MAX_X, 0);
    scene->addLine(xAxis, axisPen);

    // Ось Y (x=0)
    QLineF yAxis(0, MIN_Y, 0, MAX_Y);
    scene->addLine(yAxis, axisPen);

    // Стрелки для осей (необязательно, для красоты)
    QPolygonF arrowX;
    arrowX << QPointF(MAX_X, 0) << QPointF(MAX_X - 0.3, -0.15) << QPointF(MAX_X - 0.3, 0.15);
    scene->addPolygon(arrowX, axisPen, QBrush(Qt::black));

    QPolygonF arrowY;
    arrowY << QPointF(0, MAX_Y) << QPointF(-0.15, MAX_Y - 0.3) << QPointF(0.15, MAX_Y - 0.3);
    QTransform transform;
    transform.scale(1, -1);  // Flip vertically if needed
    scene->addPolygon(transform.map(arrowY), axisPen, QBrush(Qt::black));

    // Рисуем сетку (пунктир) с шагом 1
    QPen gridPen(Qt::gray, 0.05);

    // Вертикальные линии (x = -9..9)
    for (int x = -9; x <= 9; ++x) {
        if (x == 0) continue; // пропускаем ось
        QLineF line(x, MIN_Y, x, MAX_Y);
        scene->addLine(line, gridPen);

        // Подпись значения на оси X
        /*QGraphicsTextItem *xLabel = scene->addText(QString::number(x));
        xLabel->setPos(x - 4.5, -2.5);
        xLabel->setDefaultTextColor(Qt::black);
        QFont smallFont = xLabel->font();
        smallFont.setPointSize(1);
        xLabel->setFont(smallFont);*/
    }

    // Горизонтальные линии (y = -9..9)
    for (int y = -9; y <= 9; ++y) {
        if (y == 0) continue; // пропускаем ось
        QLineF line(MIN_X, y, MAX_X, y);
        scene->addLine(line, gridPen);

        // Подпись значения на оси Y
        /*QGraphicsTextItem *yLabel = scene->addText(QString::number(y));
        yLabel->setPos(-0.5, y - 0.15);
        yLabel->setDefaultTextColor(Qt::gray);
        QFont smallFont = yLabel->font();
        smallFont.setPointSize(8);
        yLabel->setFont(smallFont);*/
    }

    // Подписи осей
    /*QFont axisFont("Arial", 3, QFont::Bold);
    QGraphicsTextItem *xAxisLabel = scene->addText("X", axisFont);
    xAxisLabel->setPos(MAX_X - 2, 0.2);
    xAxisLabel->setDefaultTextColor(Qt::black);

    QGraphicsTextItem *yAxisLabel = scene->addText("Y", axisFont);
    yAxisLabel->setPos(0.2, MAX_Y - 0.5);
    yAxisLabel->setDefaultTextColor(Qt::black);
*/
    // Центральная точка (0,0) - маленькая точка
    //scene->addEllipse(-0.05, -0.05, 0.1, 0.1, axisPen, QBrush(Qt::black));

    // Парсим данные: "SUCCESS:x1,y1;x2,y2;..."
    QString trimmed = data;
    if (trimmed.startsWith("SUCCESS:"))
        trimmed = trimmed.mid(8);

    QStringList pointsStr = trimmed.split(';', Qt::SkipEmptyParts);
    if (pointsStr.isEmpty()) return;

    QVector<QPointF> points;

    // Преобразуем строки в точки
    for (const QString &pt : pointsStr) {
        QStringList xy = pt.split(',');
        if (xy.size() == 2) {
            double x = xy[0].toDouble();
            double y = xy[1].toDouble();
            points.append(QPointF(x, y));
        }
    }

    if (points.isEmpty()) return;

    // Рисуем сам график (ломаная линия)
    QPen graphPen(Qt::black, 0.1);
    for (int i = 0; i < points.size() - 1; ++i) {
        // Обрезаем линии, выходящие за пределы видимой области
        QLineF line(points[i], points[i+1]);

        // Обрезка по X
        if (line.x1() < MIN_X && line.x2() < MIN_X) continue;
        if (line.x1() > MAX_X && line.x2() > MAX_X) continue;

        // Обрезка по Y
        if (line.y1() < MIN_Y && line.y2() < MIN_Y) continue;
        if (line.y1() > MAX_Y && line.y2() > MAX_Y) continue;

        scene->addLine(line, graphPen);
    }

    // Рисуем маркеры точек
    /*QPen pointPen(Qt::black, 3);
    for (const QPointF &p : points) {
        // Показываем только точки в пределах видимой области
        if (p.x() >= MIN_X && p.x() <= MAX_X &&
            p.y() >= MIN_Y && p.y() <= MAX_Y) {
            scene->addEllipse(p.x() - 0.07, p.y() - 0.07, 0.001, 0.001, pointPen, QBrush(Qt::black));
        }
    }*/
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
