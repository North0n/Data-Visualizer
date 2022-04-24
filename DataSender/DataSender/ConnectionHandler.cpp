#include "ConnectionHandler.h"

#include <qdebug.h>
#include <QNetworkInterface>

ConnectionHandler::ConnectionHandler(quint16 port)
    : _port(port)
{
    for (int i = 30000; i < 40000; ++i) {
        _availablePorts.push(i);
    }
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

    if (_availablePorts.empty())
        return;
    _connectedClients[client] = new DataSender(_availablePorts.top(), address, port, this);
    qDebug() << "Established connection with IP:" << address.toString() << "Port:" << port
             << "on port" << _availablePorts.top();
    _availablePorts.pop();
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
    quint16 port = _connectedClients[address]->port();
    _availablePorts.push(port);
    delete _connectedClients[address];
    _connectedClients.remove(address);
    qDebug() << "Aborted connection with" << QHostAddress(address.first).toString()
             << address.second << "on port" << port;
}
