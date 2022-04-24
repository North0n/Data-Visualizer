#ifndef DATASENDER_DATASENDER_H
#define DATASENDER_DATASENDER_H

#include <functional>
#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QUdpSocket>
#include <QDataStream>
#include <QTimer>
#include <QByteArray>
#include "DataGenerator.h"
#include <QFuture>

// QPair<quint32 , quint16> stands for a clients IPv4 address and port
using ClientAddress = QPair<quint32, quint16>;

class DataSender : public QObject
{
    Q_OBJECT

public:
    DataSender(quint16 port, const QHostAddress &host, quint16 clientPort, QObject *parent = nullptr);

    void sendConfiguration();

    void startSending();

    quint16 port() const;

    enum class Commands
    {
        SetFunc,
        Quit,
        SetStep,
        NoOperation,
        SetDistribution,
        SetProbability,
    };

signals:
    void connectionAborted(const ClientAddress &address);

private slots:
    void read();

private:
    void checkConnection();
    void abortConnection();
    void send();

    quint16 _port;
    QHostAddress _clientAddress;
    quint16 _clientPort;
    QUdpSocket _socket;

    /// Datagram size in amount of values of type double
    static quint16 _sequenceLength;
    DataGenerator _generator;

    QTimer _timer;
    /// Maximum time between client signals, that won't lead to disconnect
    static int _maxDowntimeTime;
    bool _hadClientSignal = true;

    bool _isConnected = true;
    QFuture<void> _sending;
};


#endif //DATASENDER_DATASENDER_H
