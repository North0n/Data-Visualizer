#include "DataReceiver.h"

DataReceiver::DataReceiver(quint16 port, QObject *parent)
    : _port(port)
{
    _receiver = new QUdpSocket(this);
    _receiver->bind(QHostAddress::Any, _port);
    connect(_receiver, &QUdpSocket::readyRead, this, &DataReceiver::getData);
}

void DataReceiver::send(const QByteArray &bytes, const QHostAddress &host, quint16 port)
{
    _receiver->writeDatagram(bytes, host, port);
}

void DataReceiver::getData()
{
    QByteArray bytes(_receiver->pendingDatagramSize(), '\0');
    _receiver->readDatagram(bytes.data(), _receiver->pendingDatagramSize());
    emit dataReceived(bytes);
}
