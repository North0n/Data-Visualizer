#include "formscaling.h"
#include "ui_formscaling.h"


FormScaling::FormScaling(Scaling scaling, int range, double yMin, double yMax, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::FormScaling)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setModal(true);

    ui->setupUi(this);
    ui->rbManual->setChecked(scaling == Manual);
    setManual(scaling == Manual);

    _yValidator.setLocale(QLocale::c());
    ui->leYMin->setValidator(&_yValidator);
    ui->leYMax->setValidator(&_yValidator);

    ui->leYMin->setText(QString::number(yMin));
    ui->leYMax->setText(QString::number(yMax));
}

FormScaling::~FormScaling()
{
    delete ui;
}

void FormScaling::setManual(bool manual)
{
    ui->leYMin->setEnabled(manual);
    ui->leYMax->setEnabled(manual);
}
void FormScaling::on_rbAuto_toggled(bool checked)
{
    // Auto-scaling selected
    if (checked) {
        setManual(false);
        emit scalingTypeChanged(Auto);
    } // Manual scaling selected
    else {
        setManual(true);
        emit scalingTypeChanged(Manual);
    }
}


void FormScaling::on_pbOk_clicked()
{
    close();
}


void FormScaling::on_leYMin_textChanged(const QString &text)
{
    if (ui->leYMin->text().toDouble() > ui->leYMax->text().toDouble())
        return;
    emit yMinChanged(text.toDouble());
}


void FormScaling::on_leYMax_textChanged(const QString &text)
{
    if (ui->leYMin->text().toDouble() > ui->leYMax->text().toDouble())
        return;
    emit yMaxChanged(text.toDouble());
}