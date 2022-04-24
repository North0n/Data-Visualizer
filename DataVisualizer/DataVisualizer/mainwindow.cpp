#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "qdebug.h"
#include "stylesheet.h"
#include <algorithm>
#include <QDataStream>

quint16 MainWindow::_port = 20001;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sbRange->setValue(_displayingPointsCount);
    fillComboBoxFunctions();
    fillComboBoxDistributions();

    ui->dataDisplay->addGraph();
    _graphData = ui->dataDisplay->graph(0)->data();
    _graphData->set(QVector<QCPGraphData>(_displayingPointsCount, {0.0, 0.0}));
    int i = 0;
    std::for_each(_graphData->begin(), _graphData->end(),
                  [&](QCPGraphData &point){point.key += _keyStep * i++; });

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
    _formConnection = std::make_unique<FormConnection>(this);
    connect(_formConnection.get(), &FormConnection::onConnectPressed, this, &MainWindow::connectToServer);
    _formConnection->show();
}

void MainWindow::connectToServer(const QHostAddress &host, quint16 port)
{
    if (&host != _serverAddress.get()) {
        _serverAddress = std::make_unique<QHostAddress>(host);
    }
    _serverPort = port;

    _dataReceiver = std::make_unique<DataReceiver>(_port, *_serverAddress,
                                                   _serverPort, this);

    // Send echo request to server to get his configuration
    _dataReceiver->refreshConnection();

    // Receive configuration information about server
    auto * const connection = new QMetaObject::Connection;
    *connection = connect(_dataReceiver.get(), &DataReceiver::dataReceived,
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

            // Setting up minimum range, so it couldn't be less than size of one packet,
            // which arrives from server
            if (ui->sbRange->value() < _displayingPointsCount) {
                QVector<QCPGraphData> addedData(_displayingPointsCount - ui->sbRange->value(),
                                                {_graphData->at(0)->key, 0});
                int i = 1;
                std::for_each(addedData.rbegin(), addedData.rend(),
                              [&](QCPGraphData &point){point.key -= _keyStep * i++; });
                _graphData->add(addedData);
                ui->sbRange->setMinimum(_displayingPointsCount + 1);
            } else {
                ui->sbRange->setMinimum(_displayingPointsCount + 1);
                _displayingPointsCount = ui->sbRange->value();
            }

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
            connect(_dataReceiver.get(), &DataReceiver::dataReceived, this, &MainWindow::receiveData);

            // Timer for telling server that this client is still alive and server should refreshConnection data
            _refreshConnectionTimer = std::make_unique<QTimer>(this);
            connect(_refreshConnectionTimer.get(), &QTimer::timeout,
                    _dataReceiver.get(), &DataReceiver::refreshConnection);
            _refreshConnectionTimer->start(_serverMaxDowntime / 2);
        }
    );
}

void MainWindow::receiveData(const QByteArray &bytes)
{
    int datagramSize = bytes.size() / sizeof(double);
    if (datagramSize <= 0)
        return;

    _graphData->removeBefore((_graphData->begin() + datagramSize)->key);

    QVector<double> receivedValues(reinterpret_cast<const double*>(bytes.begin()),
                                   reinterpret_cast<const double*>(bytes.end()));
    QVector<QCPGraphData> tempData(datagramSize);
    double startKey = (_graphData->end() - 1)->key + _keyStep;
    int i = 0;
    for (auto it = tempData.begin(); it != tempData.end(); ++it, ++i) {
        it->key = startKey + i * _keyStep;
        it->value = receivedValues[i];
    }

    _graphData->add(tempData, true);
    _axisScaler();
    ui->dataDisplay->replot();
}

void MainWindow::on_actChangePort_triggered()
{
    _formChangePort = std::make_unique<FormChangePort>(_port, this);
    connect(_formChangePort.get(), &FormChangePort::onSavePressed, this, &MainWindow::changePort);
    _formChangePort->show();
}

void MainWindow::changePort(quint16 port)
{
    if (port == _port)
        return;
    _port = port;
    _dataReceiver.reset(nullptr);
}


void MainWindow::on_actDisconnect_triggered()
{
    _dataReceiver.reset(nullptr);
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
    if (value > _displayingPointsCount) {
        int count = value - _displayingPointsCount;
        QVector<QCPGraphData> addedData(count,{_graphData->at(0)->key, 0});
        int i = 1;
        std::for_each(addedData.rbegin(), addedData.rend(),
                      [&](QCPGraphData &point){point.key -= _keyStep * i++; });
        _graphData->add(addedData);
    } else if (value < _displayingPointsCount) {
        int count = _displayingPointsCount - value;
        _graphData->removeBefore((_graphData->begin() + count)->key);
    }
    _displayingPointsCount = value;
}


void MainWindow::on_actScaling_triggered()
{
    _formScaling = std::make_unique<FormScaling>(_scalingType,
                   _displayingPointsCount, ui->dataDisplay->yAxis->range().lower,
                   ui->dataDisplay->yAxis->range().upper, this);
    connect(_formScaling.get(), &FormScaling::scalingTypeChanged, this, &MainWindow::changeScalingType);
    connect(_formScaling.get(), &FormScaling::yMinChanged,
        [this](double value)
        {
            ui->dataDisplay->yAxis->setRange(value, ui->dataDisplay->yAxis->range().upper);
        }
    );
    connect(_formScaling.get(), &FormScaling::yMaxChanged,
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
    _formProbability = std::make_unique<FormNoiseProbability>(_noiseProbability, this);
    connect(_formProbability.get(), &FormNoiseProbability::probabilityReady,
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


void MainWindow::on_cbOpenGl_toggled(bool checked)
{
    ui->dataDisplay->setOpenGl(checked);
}

