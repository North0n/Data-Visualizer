#include "DataSender.h"

#include <QtConcurrent>

quint16 DataSender::_maxDowntimeTime = 5 * 60;
quint16 DataSender::_sequenceLength = 256;

DataSender::DataSender(quint16 port, const QHostAddress &host, quint16 clientPort)
    : _port(port), _client(host), _clientPort(clientPort), _timer(this)
{
    _socket.bind(QHostAddress::Any, _port);
    connect(&_socket, &QUdpSocket::readyRead, this, &DataSender::read);

    connect(&_timer, &QTimer::timeout, this, &DataSender::checkConnection);
    _timer.start();
}

void DataSender::read()
{
    QByteArray datagram;
    datagram.resize(_socket.pendingDatagramSize());
    QHostAddress address;
    quint16 port;
    _socket.readDatagram(datagram.data(), datagram.size(), &address, &port);
    if (address != _client || port != _clientPort)
        return;
    _prevClientSignalTime = QDateTime::currentSecsSinceEpoch();

    quint8 commandIndex;
    QDataStream in(&datagram, QIODevice::ReadOnly);
    if (in.device()->size() > sizeof(quint8))
        in >> commandIndex;
    else
        return;

    _commandHandlers[commandIndex](in); // TODO probably isn't necessary
}

void DataSender::startSending()
{
    QtConcurrent::run([this] { send(); });
}

void DataSender::send()
{
    QByteArray bytes;
    while (_isConnected) {
        bytes = _currentGenerator(_sequenceLength);
        _socket.writeDatagram(bytes, _client, _clientPort);
    }
}

void DataSender::changeRandom(const QDataStream& datagram)
{

}

void DataSender::ping(const QDataStream &datagram)
{

}

void DataSender::checkConnection()
{
    quint64 currTime = QDateTime::currentSecsSinceEpoch();
    if (currTime - _prevClientSignalTime > _maxDowntimeTime) {
        _isConnected = false;
    }
}

int DataSender::getMaxDowntimeTime()
{
    return _maxDowntimeTime;
}

void DataSender::setMaxDowntimeTime(quint16 maxDowntimeTime)
{
    _maxDowntimeTime = maxDowntimeTime;
}
