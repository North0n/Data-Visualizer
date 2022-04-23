#include "formnoiseprobability.h"
#include "ui_formnoiseprobability.h"


FormNoiseProbability::FormNoiseProbability(double probability, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::FormNoiseProbability)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setModal(true);

    ui->setupUi(this);
    ui->sbProbability->setValue(probability);
}

FormNoiseProbability::~FormNoiseProbability()
{
    delete ui;
}

void FormNoiseProbability::on_pbOk_clicked()
{
    emit probabilityReady(ui->sbProbability->value());
    close();
}

void FormNoiseProbability::on_pbCancel_clicked()
{
    close();
}

