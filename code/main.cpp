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

    int ret;

    //! servers
//    MRX_T4Server server( 2345 );
    MRX_T4Server server( 50000 );
    ret = server.start();
    logDbg()<<ret;

    ret = server.open();
    logDbg()<<ret;

    ret = a.exec();
    logDbg()<<ret;

    return ret;
}
