#ifndef DATAVISUALIZER_FORMCHANGEPORT_H
#define DATAVISUALIZER_FORMCHANGEPORT_H

#include <QDialog>
#include <QValidator>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class FormChangePort;
}
QT_END_NAMESPACE

class FormChangePort : public QDialog
{
Q_OBJECT

public:
    explicit FormChangePort(quint16 currentPort, QWidget *parent = nullptr);

    ~FormChangePort() override;

signals:
    void onSavePressed(quint16 port);

private slots:
    void on_pbSave_clicked();

    void on_pbCancel_clicked();

private:
    Ui::FormChangePort *ui;
    QValidator *_portValidator;
};


#endif //DATAVISUALIZER_FORMCHANGEPORT_H
