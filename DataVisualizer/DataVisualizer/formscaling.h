#ifndef DATAVISUALIZER_FORMSCALING_H
#define DATAVISUALIZER_FORMSCALING_H

#include <QDialog>
#include <QDoubleValidator>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class FormScaling;
}
QT_END_NAMESPACE

class FormScaling : public QDialog
{
Q_OBJECT

public:

    enum Scaling
    {
        Auto,
        Manual,
    };

    explicit FormScaling(Scaling scaling, int range, double yMin, double yMax, QWidget *parent = nullptr);

    ~FormScaling() override;

signals:
    void yMinChanged(double value);
    void yMaxChanged(double value);
    void scalingTypeChanged(Scaling type);

private slots:
    void on_rbAuto_toggled(bool checked);

    void on_pbOk_clicked();

    void on_leYMin_textChanged(const QString &text);

    void on_leYMax_textChanged(const QString &text);

private:

    void setManual(bool manual);

    Ui::FormScaling *ui;
    QDoubleValidator _yValidator;
};


#endif //DATAVISUALIZER_FORMSCALING_H
