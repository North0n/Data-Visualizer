#include "DataSender.h"

#include <QtConcurrent>

int DataSender::_maxDowntimeTime = 30000;
quint16 DataSender::_sequenceLength = 8;

DataSender::DataSender(quint16 port, const QHostAddress &host, quint16 clientPort, QObject *parent)
    : QObject(parent),
    _port(port),
    _client(host),
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
    if (address != _client || port != _clientPort)
        return;
    _hadClientSignal = true;

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
    auto result = QtConcurrent::run([this] { send(); });
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
    if (!_hadClientSignal) {
        _isConnected = false;
        emit connectionAborted();
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

void DataSender::quit(const QDataStream &datagram)
{
    _isConnected = false;
    emit connectionAborted();
}
