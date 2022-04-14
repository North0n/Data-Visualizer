#include "ConnectionHandler.h"

#include <qdebug.h>
#include <QNetworkInterface>

ConnectionHandler::ConnectionHandler(quint16 port)
    : _port(port)
{
    _receiver.bind(QHostAddress::Any, _port);
    connect(&_receiver, &QUdpSocket::readyRead, this, &ConnectionHandler::establishConnection);
    qDebug() << "Waiting for connections on IP: " << getIpAddress().toString().constData() << " port: " << _port;
}

void ConnectionHandler::establishConnection()
{
    QHostAddress address;
    quint16 port;
    _receiver.readDatagram(nullptr, _receiver.pendingDatagramSize(), &address, &port);
    disconnect(&_receiver, &QUdpSocket::readyRead, this, &ConnectionHandler::establishConnection);
    qDebug() << "Established connection with IP: " << address.toString().constData() << " Port: " << port;

    _dataSender = new DataSender(_nextDataSenderPort++, address, port);
    _dataSender->startSending();
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
