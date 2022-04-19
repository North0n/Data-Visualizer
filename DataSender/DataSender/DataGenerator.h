#ifndef DATASENDER_DATAGENERATOR_H
#define DATASENDER_DATAGENERATOR_H

#include <QByteArray>

class DataGenerator
{
public:
    /**
     * Generates a sequence of random double values of length @b length
     */
    static QByteArray random(quint16 length);
};


#endif //DATASENDER_DATAGENERATOR_H
