#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostAddress>
#include "DataReceiver.h"
#include "connection.h"
#include "formchangeport.h"
#include <QTimer>
#include "formscaling.h"
#include "formnoiseprobability.h"

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

    void connectToServer(const QHostAddress &host, quint16 port);
    void changePort(quint16 port);
    void receiveData(const QByteArray &byteArray);
    void changeScalingType(FormScaling::Scaling type);

    void on_actConnect_triggered();

    void on_actDisconnect_triggered();

    void on_actChangePort_triggered();

    void on_cbFunction_currentIndexChanged(int index);

    void on_sbStep_valueChanged(double value);

    void on_sbRange_valueChanged(int value);

    void on_actScaling_triggered();

    void on_cbDistribution_currentIndexChanged(int index);

    void on_actNoiseProbability_triggered();

private:

    void fillComboBoxFunctions();
    void fillComboBoxDistributions();

    static quint16 _port;
    double _keyStep = 1;

    Ui::MainWindow *ui;

    qint32 _serverMaxDowntime;
    QHostAddress *_serverAddress = nullptr;
    quint16 _serverPort;
    QTimer *_refreshConnectionTimer = nullptr;

    FormConnection *_formConnection = nullptr;
    FormChangePort *_formChangePort = nullptr;
    FormScaling *_formScaling = nullptr;

    FormScaling::Scaling _scalingType = FormScaling::Auto;
    std::function<void()> _axisScaler;
    quint16 _displayingPointsCount = 400;

    FormNoiseProbability *_formProbability = nullptr;
    double _noiseProbability = 0.50;

    DataReceiver *_dataReceiver = nullptr;
    QVector<double> _values = QVector<double>(_displayingPointsCount, 0);
    QVector<double> _keys = QVector<double>(_displayingPointsCount);
};
#endif // MAINWINDOW_H
