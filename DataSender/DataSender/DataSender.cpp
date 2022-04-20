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
    QByteArray datagram;
    datagram.resize(_socket.pendingDatagramSize());
    QHostAddress address;
    quint16 port;
    _socket.readDatagram(datagram.data(), datagram.size(), &address, &port);
    if (address.toIPv4Address() != _clientAddress.toIPv4Address() || port != _clientPort)
        return;
    _hadClientSignal = true;

    quint8 commandIndex;
    QDataStream in(&datagram, QIODevice::ReadOnly);
    if (in.device()->size() > sizeof(quint8))
        in >> commandIndex;
    else
        return;

    // By default, just ignore message (but _hadClientSignal variable)
    switch (commandIndex) {
        case static_cast<quint8>(Commands::ChangeRandom):
            break;
        case static_cast<quint8>(Commands::Quit):
            _isConnected = false;
            break;
    }
}

void DataSender::changeRandom(const QDataStream& datagram)
{

}

void DataSender::quit(const QDataStream &datagram)
{
    _isConnected = false;
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
    auto result = QtConcurrent::run([this] { send(); });
}

void DataSender::send()
{
    QByteArray bytes;
    while (_isConnected) {
        bytes = _currentGenerator(_sequenceLength);
        _socket.writeDatagram(bytes, _clientAddress, _clientPort);
    }
}

void DataSender::checkConnection()
{
    if (!_hadClientSignal) {
        _isConnected = false;
        emit connectionAborted();
        disconnect(&_timer, &QTimer::timeout, this, &DataSender::checkConnection);
        qDebug() << "Client disconnected";
    }
    _hadClientSignal = false;
}

int DataSender::getMaxDowntimeTime()
{
    return _maxDowntimeTime;
}

void DataSender::setMaxDowntimeTime(quint16 maxDowntimeTime)
{
    _maxDowntimeTime = maxDowntimeTime;
}
