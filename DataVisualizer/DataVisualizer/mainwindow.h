#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostAddress>
#include "DataReceiver.h"
#include "connection.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_actConnect_triggered();
    void connectToServer(const QHostAddress &host, quint16 port);
    void receiveData(const QByteArray &byteArray);
    void changeRange(int value);

private:

    static quint16 _port;
    quint16 _displayingPointsCount = 400;
    double _keyStep = 1;

    Ui::MainWindow *ui;

    qint32 _serverMaxDowntime;
    QHostAddress *_serverAddress = nullptr;
    quint16 _serverPort;
    QTimer *_refreshConnectionTimer = nullptr;

    FormConnection *_formConnection = nullptr;
    DataReceiver *_dataReceiver = nullptr;
    QVector<double> _values = QVector<double>(_displayingPointsCount, 0);
    QVector<double> _keys = QVector<double>(_displayingPointsCount);
};
#endif // MAINWINDOW_H
