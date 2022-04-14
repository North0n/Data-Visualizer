#include <QCoreApplication>

#include "ConnectionHandler.h"

constexpr quint16 port = 19999;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ConnectionHandler handler(port);

    return a.exec();
}
