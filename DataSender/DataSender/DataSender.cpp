#include "DataSender.h"

#include <QtConcurrent>
#include <QDataStream>

DataSender::DataSender(quint16 port, const QHostAddress &host, quint16 clientPort, QObject *parent)
    : QObject(parent),
      _port(port),
      _clientAddress(host),
      _clientPort(clientPort),
      _timer(this)
{
    _socket.bind(QHostAddress::Any, _port);
    connect(&_socket, &QUdpSocket::readyRead, this, &DataSender::read);
    connect(&_timer, &QTimer::timeout, this, &DataSender::checkConnection);
    _timer.start(_maxDowntimeTime);
}

void DataSender::read()
{
    // Reads datagram and gets information about sender
    QByteArray datagram;
    datagram.resize(_socket.pendingDatagramSize());
    QHostAddress address;
    quint16 port;
    _socket.readDatagram(datagram.data(), datagram.size(), &address, &port);
    if (address.toIPv4Address() != _clientAddress.toIPv4Address() || port != _clientPort)
        return;
    _hadClientSignal = true;

    // Handles received command
    quint8 commandIndex;
    QDataStream in(&datagram, QIODevice::ReadOnly);

    // TODO Добавил эти 4 строчки с мыслью вдруг из-за них крашиться начало
//    if (in.device()->size() >= sizeof(quint8))
        in >> commandIndex;
//    else
//        return;

    switch (commandIndex) {
        case static_cast<quint8>(Commands::SetFunc):
            quint8 funcIndex;
            in >> funcIndex;
            _generator.setFuncIndex(funcIndex);
            break;
        case static_cast<quint8>(Commands::Quit):
            abortConnection();
            break;
        case static_cast<quint8>(Commands::SetStep):
            double step;
            in >> step;
            _generator.setStep(step);
            break;
        case static_cast<quint8>(Commands::SetDistribution):
            quint8 distrIndex;
            in >> distrIndex;
            _generator.setDistribution(distrIndex);
            break;
        case static_cast<quint8>(Commands::SetProbability):
            double probability;
            in >> probability;
            _generator.setNoiseProbability(probability);
            break;
        case static_cast<quint8>(Commands::NoOperation):
            break;
    }
}

void DataSender::sendConfiguration()
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << _port
        << _maxDowntimeTime
        << _sequenceLength;
    _socket.writeDatagram(data, _clientAddress, _clientPort);
}

void DataSender::startSending()
{
    _sending = QtConcurrent::run([this] { send(); });
}

void DataSender::send()
{
    QByteArray bytes;
    while (_isConnected) {
        bytes = _generator.getData(_sequenceLength);
        _socket.writeDatagram(bytes, _clientAddress, _clientPort);
        QThread::usleep(1);
    }
}

void DataSender::checkConnection()
{
    if (!_hadClientSignal) {
        abortConnection();
    }
    _hadClientSignal = false;
}

void DataSender::abortConnection()
{
    _isConnected = false;
    _sending.waitForFinished();
    emit connectionAborted(ClientAddress(_clientAddress.toIPv4Address(), _clientPort));
}

quint16 DataSender::port() const
{
    return _port;
}
