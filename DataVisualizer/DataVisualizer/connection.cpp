#include "connection.h"
#include "ui_connection.h"

#include <QIntValidator>
#include <QRegularExpression>

FormConnection::FormConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connection)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setModal(true);

    ui->setupUi(this);

    _portValidator =  std::make_unique<QIntValidator>(0, UINT16_MAX, this);
    ui->lePort->setValidator(_portValidator.get());

    // Regular expression for valid IPv4 address
    QRegularExpression regExp(R"(^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$)");
    _ipValidator = std::make_unique<QRegularExpressionValidator>(regExp, this);
    ui->leIp->setValidator(_ipValidator.get());
}

FormConnection::~FormConnection()
{
    delete ui;
}

void FormConnection::on_pbConnect_clicked()
{
    if (ui->leIp->hasAcceptableInput() && ui->lePort->hasAcceptableInput())
        emit onConnectPressed(QHostAddress(ui->leIp->text()), ui->lePort->text().toInt());
    close();
}


void FormConnection::on_pbCancel_clicked()
{
    close();
}