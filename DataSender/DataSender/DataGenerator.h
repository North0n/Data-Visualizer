#ifndef DATASENDER_DATAGENERATOR_H
#define DATASENDER_DATAGENERATOR_H

#include <QByteArray>
#include <functional>
#include <QVector>
#include <random>

class DataGenerator
{
public:

    // length parameter in following functions refers to the length of generated sequence
    QByteArray line(quint16 length);

    QByteArray random(quint16 length);

    QByteArray sin(quint16 length);

    QByteArray cos(quint16 length);

    QByteArray getData(quint16 length);

    void setFuncIndex(int index);

    void setDistribution(int index);

    void setStep(double step);

    void setNoiseProbability(double probability);

private:

    QVector<std::function<QByteArray(quint16)>> _functions =
        {
            [this](quint16 length) {return random(length); },
            [this](quint16 length) {return line(length); },
            [this](quint16 length) {return sin(length); },
            [this](quint16 length) {return cos(length); },
        };
    int _currentFunc = 0;

    QVector<std::function<double()>> _distributions =
        {
            [](){return 0; },
            [this](){return uniformDistribution(generator); },
            [this](){return normalDistribution(generator); },
        };
    int _currentDistribution = 0;
    double _noiseProbability = 0.15;

    double _step = 0.1;

    std::mt19937 generator;
    std::uniform_real_distribution<> noiseDistribution{0, 1};
    std::uniform_real_distribution<> uniformDistribution{-0.1, 0.1};
    std::normal_distribution<> normalDistribution{0, 0.1};
};


#endif //DATASENDER_DATAGENERATOR_H
