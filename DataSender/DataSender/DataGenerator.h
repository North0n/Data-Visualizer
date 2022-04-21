#ifndef DATASENDER_DATAGENERATOR_H
#define DATASENDER_DATAGENERATOR_H

#include <QByteArray>
#include <functional>
#include <QVector>

class DataGenerator
{
public:

    DataGenerator(int funcIndex = 0, double step = 0.1);
    /**
     * Generates a sequence of random double values of length @b length
     */
    QByteArray random(quint16 length) const;

    QByteArray sin(quint16 length) const;

    QByteArray cos(quint16 length) const;

    QByteArray getData(quint16 length) const;

    void setFuncIndex(int index);

    void setStep(double step);

    enum Functions
    {
        Random,
        Sin,
        Cos
    };

private:

    QVector<std::function<QByteArray(quint16)>> _functions =
        {
            [this](quint16 length) {return random(length); },
            [this](quint16 length) {return sin(length); },
            [this](quint16 length) {return cos(length); },
        };
    int _currentFunc = 0;

    double _step = 0.1;
};


#endif //DATASENDER_DATAGENERATOR_H
