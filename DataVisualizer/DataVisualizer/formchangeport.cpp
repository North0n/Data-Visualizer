#include "formchangeport.h"
#include "ui_formchangeport.h"
#include <QIntValidator>


FormChangePort::FormChangePort(quint16 currentPort, QWidget *parent) :
        QDialog(parent), ui(new Ui::FormChangePort)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setModal(true);

    ui->setupUi(this);

    _portValidator = new QIntValidator(0, UINT16_MAX, this);
    ui->lePort->setValidator(_portValidator);
    ui->lePort->setText(QString::number(currentPort));
}

FormChangePort::~FormChangePort()
{
    delete ui;
}

void FormChangePort::on_pbSave_clicked()
{
    if (ui->lePort->hasAcceptableInput())
        emit onSavePressed(ui->lePort->text().toInt());
    close();
}


void FormChangePort::on_pbCancel_clicked()
{
    close();
}

