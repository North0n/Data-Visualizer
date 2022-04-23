#ifndef DATAVISUALIZER_FORMNOISEPROBABILITY_H
#define DATAVISUALIZER_FORMNOISEPROBABILITY_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui
{
    class FormNoiseProbability;
}
QT_END_NAMESPACE

class FormNoiseProbability : public QDialog
{
Q_OBJECT

public:
    explicit FormNoiseProbability(double probability, QWidget *parent = nullptr);

    ~FormNoiseProbability() override;

signals:
    void probabilityReady(double value);

private slots:
    void on_pbOk_clicked();

    void on_pbCancel_clicked();

private:
    Ui::FormNoiseProbability *ui;
};


#endif //DATAVISUALIZER_FORMNOISEPROBABILITY_H
