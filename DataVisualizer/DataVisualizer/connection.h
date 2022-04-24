#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>
#include <QString>
#include <QValidator>
#include <QHostAddress>
#include <memory>

namespace Ui {
class Connection;
}

class FormConnection : public QDialog
{
    Q_OBJECT

public:
    explicit FormConnection(QWidget *parent = nullptr);
    ~FormConnection() override;

signals:
    void onConnectPressed(const QHostAddress &hostname, quint16 port);

private slots:
    void on_pbConnect_clicked();

    void on_pbCancel_clicked();

private:

    void setupStylesheet();

    Ui::Connection *ui;
    std::unique_ptr<QValidator> _ipValidator;
    std::unique_ptr<QValidator> _portValidator;
};

#endif // CONNECTION_H
