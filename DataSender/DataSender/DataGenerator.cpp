#include "DataGenerator.h"

#include <QRandomGenerator>
#include <QVector>
#include <cmath>
#include "qdebug.h"

QByteArray DataGenerator::random(quint16 length)
{
    QVector<double> values(length);
    for (auto &value : values)
        value = QRandomGenerator::global()->generateDouble();

    return QByteArray(reinterpret_cast<char*>(values.data()), length * sizeof(double));
}

QByteArray DataGenerator::line(quint16 length)
{
    QVector<double> values(length, 0);
    for (auto &value : values)
        value = _distributions[_currentDistribution]();

    return QByteArray(reinterpret_cast<char*>(values.data()), length * sizeof(double));
}

QByteArray DataGenerator::sin(quint16 length)
{
    static double x = 0.0;
    static const double period = 2 * M_PI;
    QVector<double> values(length);
    for (auto &value : values) {
        value = std::sin(x);
        // TODO было бы всё-таки хорошо добавить возможность управлять вероятностью шума
        value += static_cast<int>(noiseDistribution(generator) + _noiseProbability)
                * _distributions[_currentDistribution]();
//        value += normalDistribution(generator);
        x += _step;
        x -= static_cast<int>(x / period) * period;
//        qDebug() << x << " " << value;
    }
    return QByteArray(reinterpret_cast<char*>(values.data()), length * sizeof(double));
}

QByteArray DataGenerator::cos(quint16 length)
{
    static double x = 0.0;
    static const double period = 2 * M_PI;
    QVector<double> values(length);
    for (auto &value : values) {
        value = std::cos(x);
        value += _distributions[_currentDistribution]();
        x += _step;
        x -= static_cast<int>(x / period) * period;
    }
    return QByteArray(reinterpret_cast<char*>(values.data()), length * sizeof(double));
}

QByteArray DataGenerator::getData(quint16 length)
{
    return _functions[_currentFunc](length);
}

void DataGenerator::setFuncIndex(int index)
{
    if (index < 0 || index >= _functions.size()) {
        qDebug() << "Invalid function index = " << index;
        return;
    }
    _currentFunc = index;
}

void DataGenerator::setDistribution(int index)
{
    if (index < 0 || index >= _distributions.size()) {
        qDebug() << "Invalid distribution index = " << index;
        return;
    }
    _currentDistribution = index;
}

DataGenerator::DataGenerator(int funcIndex, double step)
    : _currentFunc(funcIndex), _step(step)
{}

void DataGenerator::setStep(double step)
{
    _step = step;
}
