#include "ConnectionHandler.h"

#include <qdebug.h>
#include <QNetworkInterface>

ConnectionHandler::ConnectionHandler(quint16 port)
    : _port(port)
{
    _receiver.bind(QHostAddress::AnyIPv4, _port);
    connect(&_receiver, &QUdpSocket::readyRead, this, &ConnectionHandler::establishConnection);
    qDebug() << "Waiting for connections on IP: " << getIpAddress().toString() << " port: " << _port;
}

void ConnectionHandler::establishConnection()
{
    QHostAddress address;
    quint16 port;
    QByteArray bytes(_receiver.pendingDatagramSize(), '\0');
    _receiver.readDatagram(bytes.data(), _receiver.pendingDatagramSize(), &address, &port);
    auto client = ClientAddress(address.toIPv4Address(), port);
    if (_connectedClients.contains(client))
        return;
    qDebug() << "Established connection with IP: " << address.toString() << " Port: " << port;

    // TODO изменить алгоритм выдачи порта (_nextDataSenderPort++), потому что, когда порты закончатся
    //  (переполнится quint16) порты начнут выдаваться с 0, что не есть хорошо. При этом если начать выдавать
    //  порты опять с того, с которого начинали изначально (30000) может получиться такое, что порт 30000 всё ещё занят
    _connectedClients[client] = new DataSender(_nextDataSenderPort++, address, port, this);
    connect(_connectedClients[client], &DataSender::connectionAborted,
            this, &ConnectionHandler::abortConnection);
    _connectedClients[client]->sendConfiguration();
    _connectedClients[client]->startSending();
}

QHostAddress ConnectionHandler::getIpAddress()
{
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address : QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            return address;
    }
    return QHostAddress::LocalHost;
}

void ConnectionHandler::abortConnection(const ClientAddress &address)
{
    delete _connectedClients[address];
    _connectedClients.remove(address);
    qDebug() << "Aborted connection with " << QHostAddress(address.first).toString() << " " << address.second;
}
