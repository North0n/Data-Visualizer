#ifndef DATASENDER_CONNECTIONHANDLER_H
#define DATASENDER_CONNECTIONHANDLER_H

#include <QObject>
#include <QUdpSocket>
#include "DataSender.h"
#include <QMap>
#include <QPair>
#include <QList>
#include <queue>
#include <vector>

// QPair<quint32 , quint16> stands for a clients IPv4 address and port
using ClientAddress = QPair<quint32, quint16>;

class ConnectionHandler : public QObject
{
    Q_OBJECT
public:
    ConnectionHandler(quint16 port);
    static QHostAddress getIpAddress();

private slots:
    void establishConnection();
    void abortConnection(const ClientAddress &address);

private:
    quint16 _port;
    QUdpSocket _receiver;
    quint16 _nextDataSenderPort = 30000;
    std::priority_queue<quint16, std::vector<quint16>, std::greater<>> _availablePorts;
    QMap<ClientAddress, DataSender*> _connectedClients;
};


#endif //DATASENDER_CONNECTIONHANDLER_H
