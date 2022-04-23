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
    QByteArray data;
    QDataStream command(&data, QIODevice::WriteOnly);
    command << static_cast<quint8>(Commands::NoOperation);
    _receiver->writeDatagram(data, _serverAddress, _serverPort);
}

void DataReceiver::quit()
{
    QByteArray data;
    QDataStream command(&data, QIODevice::WriteOnly);
    command << static_cast<quint8>(Commands::Quit);
    _receiver->writeDatagram(data, _serverAddress, _serverPort);
}

void DataReceiver::setFunction(quint8 funcIndex)
{
    QByteArray data;
    QDataStream command(&data, QIODevice::WriteOnly);
    command << static_cast<quint8>(Commands::SetFunc)
            << funcIndex;
    _receiver->writeDatagram(data, _serverAddress, _serverPort);
}

void DataReceiver::setStep(double step)
{
    QByteArray data;
    QDataStream command(&data, QIODevice::WriteOnly);
    command << static_cast<quint8>(Commands::SetStep)
            << step;
    _receiver->writeDatagram(data, _serverAddress, _serverPort);
}

void DataReceiver::setDistribution(quint8 distrIndex)
{
    // TODO неплохо бы как-то эти однотипные отправления функций вынести в одну
    QByteArray data;
    QDataStream command(&data, QIODevice::WriteOnly);
    command << static_cast<quint8>(Commands::SetDistribution)
            << distrIndex;
    _receiver->writeDatagram(data, _serverAddress, _serverPort);
}

void DataReceiver::setNoiseProbability(double probability)
{
    QByteArray data;
    QDataStream command(&data, QIODevice::WriteOnly);
    command << static_cast<quint8>(Commands::SetProbability)
            << probability;
    _receiver->writeDatagram(data, _serverAddress, _serverPort);
}
