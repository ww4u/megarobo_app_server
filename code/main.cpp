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

    logDbg_Thread();

    int ret;

    //! servers
    MRX_T4Server server( 4567 );
    ret = server.start();

    ret = a.exec();

    logDbg();

    return ret;
}
