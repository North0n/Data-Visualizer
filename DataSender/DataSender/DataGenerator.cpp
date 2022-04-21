#include "DataGenerator.h"

#include <QRandomGenerator>
#include <QVector>
#include <cmath>
#include "qdebug.h"

QByteArray DataGenerator::random(quint16 length) const
{
    QVector<double> values(length);
    for (auto &value : values)
        value = QRandomGenerator::global()->generateDouble();

    return QByteArray(reinterpret_cast<char*>(values.data()), length * sizeof(double));
}

QByteArray DataGenerator::sin(quint16 length) const
{
    static double x = 0.0;
    static const double period = 2 * M_PI;
    QVector<double> values(length);
    for (auto &value : values) {
        value = std::sin(x);
        x += _step;
        x -= static_cast<int>(x / period) * period;
    }
    return QByteArray(reinterpret_cast<char*>(values.data()), length * sizeof(double));
}

QByteArray DataGenerator::cos(quint16 length) const
{
    static double x = 0.0;
    static const double period = 2 * M_PI;
    QVector<double> values(length);
    for (auto &value : values) {
        value = std::cos(x);
        x += _step;
        x -= static_cast<int>(x / period) * period;
    }
    return QByteArray(reinterpret_cast<char*>(values.data()), length * sizeof(double));
}

QByteArray DataGenerator::getData(quint16 length) const
{
    return _functions[_currentFunc](length);
}

void DataGenerator::setFuncIndex(int index)
{
    if (index < 0 || index >= _functions.size()) {
        qDebug() << "DataGenerator: invalid index = " << index;
        return;
    }
    _currentFunc = index;
}

DataGenerator::DataGenerator(int funcIndex, double step)
    : _currentFunc(funcIndex), _step(step)
{}

void DataGenerator::setStep(double step)
{
    _step = step;
}
