#include "DataGenerator.h"

#include <QRandomGenerator>
#include <QVector>
#include "qdebug.h"

QByteArray DataGenerator::random(quint16 length)
{
    QVector<double> values(length);
    for (auto &value : values)
        value = QRandomGenerator::global()->generateDouble();

    qDebug() << values;
    return QByteArray(reinterpret_cast<char*>(values.data()), length * sizeof(double));
}
