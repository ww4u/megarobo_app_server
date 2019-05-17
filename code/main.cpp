#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QThread>

#include "mydebug.h"
#include "./t4/mrx_t4server.h"
#include "./lte/let_server.h"

#include "syspara.h"

//! \note in raspberry VERSION is invalid
#define _version_ "0.0.0.1"

//! -logoutput
//! -loginput
//! -showin
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    sysSetArg( argc, argv );

    //! \note for version
    qInfo()<<"Verison:"<<_version_;
    qInfo()<<"Build:"<<__DATE__<<__TIME__;
    int ret;

    //! servers
    MRX_T4Server server( 50000 );

//    Let_Server server( 2345 );
    ret = server.start();
    logDbg()<<ret;
    do
    {
        ret = server.open();
        QThread::msleep( 100 );
    }while( ret != 0 );

    ret = a.exec();
    logDbg()<<ret;

    return ret;
}
