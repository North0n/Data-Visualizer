#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "qdebug.h"
#include "stylesheet.h"
#include <algorithm>
#include <QDataStream>
#include "my_algorithm.h"

quint16 MainWindow::_port = 20001;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->sbRange, &QSpinBox::valueChanged, this, &MainWindow::changeRange);

    ui->dataDisplay->addGraph();
    fillRangeWithStep(_keys.begin(), _keys.end(), 0.0, _keyStep);
    ui->dataDisplay->graph()->setData(_keys, _values);
    ui->dataDisplay->graph(0)->rescaleAxes();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actConnect_triggered()
{
    delete _formConnection;
    _formConnection = new FormConnection(this);
    connect(_formConnection, &FormConnection::onConnectPressed, this, &MainWindow::connectToServer);
    _formConnection->show();
}

void MainWindow::connectToServer(const QHostAddress &host, quint16 port)
{
    delete _serverAddress;
    _serverAddress = new QHostAddress(host);
    _serverPort = port;

    delete _dataReceiver;
    _dataReceiver = new DataReceiver(_port, this);

    // Send echo request to server to get his configuration
    refreshConnection();

    // Receive configuration information about server
    auto * const connection = new QMetaObject::Connection;
    *connection = connect(_dataReceiver, &DataReceiver::dataReceived,
        [this, connection](const QByteArray &bytes)
        {
            // Setting server configuration
            // Port is changed, because first port is a port of a whole server,
            // which gives for this particular client a single thread to carry out its queries.
            // So new port refers to this particular thread
            QDataStream in(bytes);
            in >> _serverPort
               >> _serverMaxDowntime
               >> _displayingPointsCount;
            ui->sbRange->setMinimum(_displayingPointsCount);
            _keys.resize(_displayingPointsCount);
            _values.resize(_displayingPointsCount);

            // If data left, then it's already received data generated for displaying
            receiveData(in.device()->readAll());

            // Handle all the other received data using receiveData method
            QObject::disconnect(*connection);
            delete connection;
            connect(_dataReceiver, &DataReceiver::dataReceived, this, &MainWindow::receiveData);

            // Timer for telling server that this client is still alive and server should send data
            delete _refreshConnectionTimer;
            _refreshConnectionTimer = new QTimer(this);
            connect(_refreshConnectionTimer, &QTimer::timeout, this, &MainWindow::refreshConnection);
            _refreshConnectionTimer->start(_serverMaxDowntime / 2);
        });
}

void MainWindow::receiveData(const QByteArray &bytes)
{
    int datagramSize = bytes.size() / sizeof(double);
    // TODO обработать случай, когда размер датаграммы больше чем размер массива
    _values.remove(0, datagramSize);
    _values.append(QVector<double>(reinterpret_cast<const double*>(bytes.begin()),
                                        reinterpret_cast<const double*>(bytes.end())));

    // TODO по-хорошему подобные вычисления вынести из этого метода в поле и его изменять при изменении значений спинбоксов
    double difference = _displayingPointsCount * _keyStep;
    std::rotate(_keys.begin(), _keys.begin() + datagramSize, _keys.end());
    std::for_each(_keys.begin() + (_keys.size() - datagramSize), _keys.end(),
                     [step = difference](auto &value){value += step;});

    ui->dataDisplay->graph(0)->setData(_keys, _values);
    ui->dataDisplay->graph(0)->rescaleAxes();
    ui->dataDisplay->replot();
}

void MainWindow::refreshConnection()
{
    _dataReceiver->send(QByteArray("q"), *_serverAddress, _serverPort);
}

void MainWindow::changeRange(int value)
{
    if (value > _displayingPointsCount) {
        int count = value - _displayingPointsCount;
        _keys.insert(0, count, 0);
        fillRangeWithStep(_keys.rend() - count, _keys.rend(),
                          *(_keys.rend() - count - 1) - _keyStep, -_keyStep);
        _values.insert(0, count, 0);
    } else if (value < _displayingPointsCount) {
        int count = _displayingPointsCount - value;
        _keys.remove(0, count);
        _values.remove(0, count);
    }
    _displayingPointsCount = value;
}