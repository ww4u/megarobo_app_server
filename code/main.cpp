#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QThread>

#include "mydebug.h"
#include "./t4/mrx_t4server.h"

#include "syspara.h"


//! -logoutput
//! -loginput
//! -showin
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    sysSetArg( argc, argv );

    qInfo( qApp->applicationVersion().toLatin1().data() );

    logDbg()<<QDir::homePath();

    int ret;

    //! servers
//    MRX_T4Server server( 2345 );
    MRX_T4Server server( 50000 );
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
