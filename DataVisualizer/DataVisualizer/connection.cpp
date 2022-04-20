#include "connection.h"
#include "ui_connection.h"

#include <QIntValidator>
#include <QRegularExpression>
#include "stylesheet.h"

FormConnection::FormConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connection)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setModal(true);

    ui->setupUi(this);
//    setupStylesheet();

    _portValidator = new QIntValidator(0, UINT16_MAX, this);
    ui->lePort->setValidator(_portValidator);

    // Regular expression for valid IPv4 address
    QRegularExpression regExp(R"(^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$)");
    _ipValidator = new QRegularExpressionValidator(regExp, this);
    ui->leIp->setValidator(_ipValidator);
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

void FormConnection::setupStylesheet()
{
    ui->pbConnect->setStyleSheet(Styles::pushButton);
    ui->pbCancel->setStyleSheet(Styles::pushButton);
}