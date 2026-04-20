#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QGraphicsScene>      // добавляем
#include <QGraphicsView>       // добавляем

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString sendCommand(const QString& command);

private slots:
    void onParameter1Changed(int value);
    void onParameter2Changed(int value);
    void onParameter3Changed(int value);
    void onLogout();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QString currentUser;
    bool isAuthenticated;

    void showLoginDialog();
    void showRegisterDialog();
    void setupFunctionUi();
    void connectToServer();
    void updateGraph();
    void updateTable(const QString &data);
    void drawGraph(const QString &data);   // новый метод
};

#endif // MAINWINDOW_H
