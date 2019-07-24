#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QThread>
#include <QAbstractSocket>

#include "mydebug.h"
#include "./t4/mrx_t4server.h"
#include "./lte/let_server.h"

#include "syspara.h"

//! \note in raspberry VERSION is invalid
#define _version_ "0.0.0.1"

//! -logoutput
//! -loginput
//! -showin
//! -let
//! -mega
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    sysSetArg( argc, argv );

    qRegisterMetaType<QAbstractSocket::SocketError>( "QAbstractSocket::SocketError" );

    //! \note for version
    qInfo()<<"Verison:"<<_version_;
    qInfo()<<"Build:"<<__DATE__<<__TIME__;

    logDbg_Thread();

    int ret;

    //! servers
    MAppServer *pServer;
    if ( sysHasArg("-let") )
    {
        pServer = new Let_Server(2345);
    }
    else
    {
        pServer = new MRX_T4Server( 50000, 2 );
    }
    if ( NULL == pServer )
    {
        logDbg()<<"fail to new";
        return -1;
    }

    pServer->start();
    do
    {
        ret = pServer->open();
        QThread::msleep( 100 );
    }while( ret != 0 );

    ret = a.exec();

    delete pServer;

    return ret;
}
