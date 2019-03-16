#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QThread>

#include "mydebug.h"
#include "./t4/mrx_t4server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    logDbg()<<QThread::currentThreadId();

    MRX_T4Server server;

    server.start();

    return a.exec();
}
