#include "DataGenerator.h"

#include <QRandomGenerator>
#include <QVector>

QByteArray DataGenerator::random(quint16 length)
{
    QVector<quint32> bytes(length / sizeof(quint32));
    QRandomGenerator::global()->fillRange(bytes.data(), bytes.size());
    QByteArray arr(reinterpret_cast<char*>(bytes.data()), length);
    return arr;
}
