#include "mtcpserver.h"
#include <QTcpSocket>
#include "mydebug.h"

MTcpServer::MTcpServer(QObject *parent) : QTcpServer(parent)
{
    mPort = 0;
    m_pServer = NULL;
}

void MTcpServer::attachServer( MAppServer *pServer )
{
    Q_ASSERT( NULL != pServer );

    m_pServer = pServer;
}

bool MTcpServer::start( quint16 port )
{
    mPort = port;

    bool bRet;
    bRet = listen( QHostAddress::Any, mPort );
    if ( bRet )
    {}
    else
    { return false; }

    return true;
}






