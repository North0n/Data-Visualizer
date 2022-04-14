#ifndef DATASENDER_CONNECTIONHANDLER_H
#define DATASENDER_CONNECTIONHANDLER_H

#include <QObject>
#include <QUdpSocket>
#include "DataSender.h"

class ConnectionHandler : public QObject
{
    Q_OBJECT
public:
    ConnectionHandler(quint16 port);
    static QHostAddress getIpAddress();

private slots:
    void establishConnection();

private:
    quint16 _port;
    QUdpSocket _receiver;
    DataSender *_dataSender = nullptr;
    quint16 _nextDataSenderPort = 20000;
};


#endif //DATASENDER_CONNECTIONHANDLER_H
