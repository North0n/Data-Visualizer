#ifndef DATAVISUALIZER_DATARECEIVER_H
#define DATAVISUALIZER_DATARECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QPointer>

class DataReceiver : public QObject
{
    Q_OBJECT

public:
    DataReceiver(quint16 port, const QHostAddress &serverAddress, quint16 serverPort,
                 QObject *parent = nullptr);

    ~DataReceiver() override;

    void setServerPort(quint16 port) {_serverPort = port; }

    void refreshConnection();

    void setFunction(quint8 funcIndex);

    void setStep(double step);

    void setDistribution(quint8 distrIndex);

    void quit();

    enum class Commands
    {
        SetFunc,
        Quit,
        SetStep,
        NoOperation,
        SetDistribution,
    };

    enum Functions
    {
        Random = 0,
        Line,
        Sin,
        Cos,
    };

    enum Distributions
    {
        NoDistribution,
        Uniform,
        Normal,
    };

signals:
    void dataReceived(const QByteArray &data);

private:

    void getData();

    QHostAddress _serverAddress;
    quint16 _serverPort;

    // Port which receiver "listens" to
    quint16 _port;
    QPointer<QUdpSocket> _receiver;
};


#endif //DATAVISUALIZER_DATARECEIVER_H
