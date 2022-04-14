#include "DataGenerator.h"

#include <QRandomGenerator>
#include <QVector>

QByteArray DataGenerator::random(quint16 length)
{
    // Divided by 2 because QRandomGenerator accepts only containers of quint32 and quint64
    QVector<quint32> bytes(length / sizeof(quint16) / 2);
    QRandomGenerator::global()->fillRange(bytes.data(), bytes.size());
    return reinterpret_cast<char*>(bytes.data());
}
