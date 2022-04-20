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
    disconnect(&_receiver, &QUdpSocket::readyRead, this, &ConnectionHandler::establishConnection);
    qDebug() << "Established connection with IP: " << address.toString() << " Port: " << port;

    _dataSender = new DataSender(_nextDataSenderPort++, address, port, this);
    _dataSender->sendConfiguration();
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