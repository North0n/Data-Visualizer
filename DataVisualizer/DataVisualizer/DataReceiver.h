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
    DataReceiver(quint16 port, QObject *parent = nullptr);

    void send(const QByteArray &bytes, const QHostAddress &host, quint16 port);

signals:
    void dataReceived(const QByteArray &data);

private:

    void getData();

    // Port which receiver "listens" to
    quint16 _port;
    QPointer<QUdpSocket> _receiver;
};


#endif //DATAVISUALIZER_DATARECEIVER_H
