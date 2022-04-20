#include "DataReceiver.h"

DataReceiver::DataReceiver(quint16 port, const QHostAddress &serverAddress, quint16 serverPort,
                           QObject *parent)
                           : QObject(parent),
                           _port(port),
                           _serverAddress(serverAddress),
                           _serverPort(serverPort)
{
    _receiver = new QUdpSocket(this);
    _receiver->bind(QHostAddress::Any, _port);
    connect(_receiver, &QUdpSocket::readyRead, this, &DataReceiver::getData);
}

void DataReceiver::getData()
{
    QByteArray bytes(_receiver->pendingDatagramSize(), '\0');
    _receiver->readDatagram(bytes.data(), _receiver->pendingDatagramSize());
    emit dataReceived(bytes);
}

DataReceiver::~DataReceiver()
{
    quit();
}

void DataReceiver::refreshConnection()
{
    _receiver->writeDatagram(QByteArray("q"), _serverAddress, _serverPort);
}

void DataReceiver::quit()
{
    QByteArray data;
    QDataStream command(&data, QIODevice::WriteOnly);
    command << static_cast<quint8>(Commands::Quit);
    _receiver->writeDatagram(data, _serverAddress, _serverPort);
}
