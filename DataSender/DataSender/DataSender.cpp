#include "DataSender.h"

#include <QtConcurrent>
#include <QDataStream>

int DataSender::_maxDowntimeTime = 30000;
quint16 DataSender::_sequenceLength = 64;

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
    in >> commandIndex;
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
    QtConcurrent::run([this] { send(); });
}

void DataSender::send()
{
    QByteArray bytes;
    while (_isConnected) {
        bytes = _generator.getData(_sequenceLength);
        _socket.writeDatagram(bytes, _clientAddress, _clientPort);
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
    emit connectionAborted(ClientAddress(_clientAddress.toIPv4Address(), _clientPort));
    disconnect(&_timer, &QTimer::timeout, this, &DataSender::checkConnection);
    qDebug() << "Client disconnected";
}
