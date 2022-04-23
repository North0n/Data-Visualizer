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
    fillComboBoxFunctions();
    fillComboBoxDistributions();

    ui->dataDisplay->addGraph();
    fillRangeWithStep(_keys.begin(), _keys.end(), 0.0, _keyStep);
    ui->dataDisplay->graph(0)->setData(_keys, _values);
    ui->dataDisplay->xAxis->setTickLabels(false);
    _axisScaler = [this](){ui->dataDisplay->graph(0)->rescaleAxes(); };
    _axisScaler();
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
    if (&host != _serverAddress) {
        delete _serverAddress;
        _serverAddress = new QHostAddress(host);
    }
    _serverPort = port;

    delete _dataReceiver;
    _dataReceiver = new DataReceiver(_port, *_serverAddress, _serverPort, this);

    // Send echo request to server to get his configuration
    _dataReceiver->refreshConnection();

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
               >> _displayingPointsCount; // Amount of points generated in one datagram

            // DataVisualizer setup
            if (ui->sbRange->value() < _displayingPointsCount) {
                _keys.resize(_displayingPointsCount);
                fillRangeWithStep(_keys.begin(), _keys.end(), 0.0, _keyStep);
                _values.resize(_displayingPointsCount);
            } else {
                _displayingPointsCount = ui->sbRange->value();
            }
            ui->sbRange->setMinimum(_displayingPointsCount);

            // Server setup depending on current DataVisualizer setup
            _dataReceiver->setServerPort(_serverPort);
            _dataReceiver->setFunction(ui->cbFunction->currentData().toInt());
            _dataReceiver->setStep(ui->sbStep->value());
            _dataReceiver->setDistribution(ui->cbDistribution->currentData().toInt());
            _dataReceiver->setNoiseProbability(_noiseProbability);

            // If data left, then it's already received data generated for displaying
            receiveData(in.device()->readAll());

            // Handle all the other received data using receiveData method
            QObject::disconnect(*connection);
            delete connection;
            connect(_dataReceiver, &DataReceiver::dataReceived, this, &MainWindow::receiveData);

            // Timer for telling server that this client is still alive and server should refreshConnection data
            delete _refreshConnectionTimer;
            _refreshConnectionTimer = new QTimer(this);
            connect(_refreshConnectionTimer, &QTimer::timeout,
                    _dataReceiver, &DataReceiver::refreshConnection);
            _refreshConnectionTimer->start(_serverMaxDowntime / 2);
        }
    );
}

void MainWindow::receiveData(const QByteArray &bytes)
{
    int datagramSize = bytes.size() / sizeof(double);
    _values.remove(0, datagramSize);
    _values.append(QVector<double>(reinterpret_cast<const double*>(bytes.begin()),
                                        reinterpret_cast<const double*>(bytes.end())));
//    qDebug() << QVector<double>(reinterpret_cast<const double*>(bytes.begin()),
//                                reinterpret_cast<const double*>(bytes.end()));

    // TODO по-хорошему подобные вычисления вынести из этого метода в поле и его изменять при изменении значений спинбоксов
//    double difference = _displayingPointsCount * _keyStep;
//    std::rotate(_keys.begin(), _keys.begin() + datagramSize, _keys.end());
//    std::for_each(_keys.begin() + (_keys.size() - datagramSize), _keys.end(),
//                     [step = difference](auto &value){value += step;});

    ui->dataDisplay->graph(0)->setData(_keys, _values);

    _axisScaler();
    ui->dataDisplay->replot();
}

void MainWindow::on_actChangePort_triggered()
{
    delete _formChangePort;
    _formChangePort = new FormChangePort(_port, this);
    connect(_formChangePort, &FormChangePort::onSavePressed, this, &MainWindow::changePort);
    _formChangePort->show();
}

void MainWindow::changePort(quint16 port)
{
    if (port == _port)
        return;
    _port = port;
    delete _dataReceiver;
    _dataReceiver = nullptr;
}


void MainWindow::on_actDisconnect_triggered()
{
    delete _dataReceiver;
    _dataReceiver = nullptr;
}


void MainWindow::on_cbFunction_currentIndexChanged(int index)
{
    if (_dataReceiver == nullptr)
        return;
    _dataReceiver->setFunction(ui->cbFunction->currentData().toInt());
}

void MainWindow::on_cbDistribution_currentIndexChanged(int index)
{
    if (_dataReceiver == nullptr)
        return;
    _dataReceiver->setDistribution(ui->cbDistribution->currentData().toInt());
}


void MainWindow::fillComboBoxFunctions()
{
    ui->cbFunction->addItem("Случайные данные", static_cast<int>(DataReceiver::Random));
    ui->cbFunction->addItem("Прямая", static_cast<int>(DataReceiver::Line));
    ui->cbFunction->addItem("Синус", static_cast<int>(DataReceiver::Sin));
    ui->cbFunction->addItem("Косинус", static_cast<int>(DataReceiver::Cos));
}

void MainWindow::fillComboBoxDistributions()
{
    ui->cbDistribution->addItem("Без шума", static_cast<int>(DataReceiver::NoDistribution));
    ui->cbDistribution->addItem("Равномерное", static_cast<int>(DataReceiver::Uniform));
    ui->cbDistribution->addItem("Нормальное", static_cast<int>(DataReceiver::Normal));
}

void MainWindow::on_sbStep_valueChanged(double value)
{
    if (_dataReceiver == nullptr)
        return;
    _dataReceiver->setStep(value);
}

void MainWindow::on_sbRange_valueChanged(int value)
{
    // TODO если я не подписываю ось X, тогда зачем тут _keyStep и вообще можно упростить
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


void MainWindow::on_actScaling_triggered()
{
    delete _formScaling;
    _formScaling = new FormScaling(_scalingType, _displayingPointsCount,
                                   ui->dataDisplay->yAxis->range().lower, ui->dataDisplay->yAxis->range().upper,
                                   this);
    connect(_formScaling, &FormScaling::scalingTypeChanged, this, &MainWindow::changeScalingType);
    connect(_formScaling, &FormScaling::yMinChanged,
        [this](double value)
        {
            ui->dataDisplay->yAxis->setRange(value, ui->dataDisplay->yAxis->range().upper);
        }
    );
    connect(_formScaling, &FormScaling::yMaxChanged,
        [this](double value)
        {
            ui->dataDisplay->yAxis->setRange(ui->dataDisplay->yAxis->range().lower, value);
        }
    );
    _formScaling->show();
}

void MainWindow::changeScalingType(FormScaling::Scaling type)
{
    _scalingType = type;
    if (type == FormScaling::Auto)
        _axisScaler = [this](){ui->dataDisplay->graph(0)->rescaleAxes(); };
    else
        _axisScaler = [this](){ui->dataDisplay->xAxis->rescale(); };
}


void MainWindow::on_actNoiseProbability_triggered()
{
    delete _formProbability;
    _formProbability = new FormNoiseProbability(_noiseProbability, this);
    connect(_formProbability, &FormNoiseProbability::probabilityReady,
        [this](double value)
        {
            _noiseProbability = value;
            if (_dataReceiver == nullptr)
                return;
            _dataReceiver->setNoiseProbability(_noiseProbability);
        }
    );
    _formProbability->show();
}

