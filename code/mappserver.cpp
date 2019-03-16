#include "mappserver.h"
#include <QTcpSocket>
#include "mydebug.h"

MAppServer::MAppServer(QObject *parent) : QTcpServer(parent)
{

}

//void MAppServer::incomingConnection(qintptr socketDescriptor)
//{
//    logDbg();
//    MAppService *thread = new MAppService(socketDescriptor, 0 );
//    thread->moveToThread( thread );
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
//    thread->start();
//}

bool MAppServer::start()
{
    bool bRet;
    bRet = listen( QHostAddress::Any, 2345 );
    if ( bRet )
    {}
    else
    { return false; }

    //! try open
    int ret = open();
    logDbg()<<ret;

    return true;
}


