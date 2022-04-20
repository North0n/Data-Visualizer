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

class DataSender : public QObject
{
    Q_OBJECT

public:
    DataSender(quint16 port, const QHostAddress &host, quint16 clientPort, QObject *parent = nullptr);

    void sendConfiguration();

    void startSending();

    static void setMaxDowntimeTime(quint16 maxDowntimeTime);

    static int getMaxDowntimeTime();

    enum class Commands
    {
        ChangeRandom,
        Quit
    };

signals:
    void connectionAborted();

private slots:
    void read();

private:
    void checkConnection();
    void abortConnection();
    void send();

    // TODO should delete this if user switch
    QVector<std::function<void(const QDataStream&)>> _commandHandlers =
        {
                [this](const QDataStream &datagram) { changeRandom(datagram); },
                [this](const QDataStream &datagram) { quit(datagram); }
        };
    void changeRandom(const QDataStream &datagram);
    void quit(const QDataStream &datagram);

    quint16 _port;
    QHostAddress _clientAddress;
    quint16 _clientPort;
    QUdpSocket _socket;

    /// Datagram size in amount of values of type double
    static quint16 _sequenceLength;
    std::function<QByteArray(quint16 length)> _currentGenerator = DataGenerator::random;

    QTimer _timer;
    /// Maximum time between client signals, that won't lead to disconnect
    static int _maxDowntimeTime;
    bool _hadClientSignal = true;

    bool _isConnected = true;
};


#endif //DATASENDER_DATASENDER_H
