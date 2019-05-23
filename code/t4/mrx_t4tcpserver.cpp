#include "mrx_t4tcpserver.h"
#include "mrx_t4service.h"

#include "../mydebug.h"
#include "MegaGateway.h"
#include "mrx_t4server.h"
MRX_T4TcpServer::MRX_T4TcpServer( QObject *parent ) : MTcpServer( parent )
{   
}

void MRX_T4TcpServer::incomingConnection(qintptr socketDescriptor)
{
    //! self thread
    MAppService *thread = new MRX_T4Service(socketDescriptor, mPort, 0 );
    if ( NULL == thread )
    { logDbg(); return; }

    connect( thread, SIGNAL(finished()), thread, SLOT(slot_deleteLater()));

    thread->moveToThread( thread );

    Q_ASSERT( NULL != m_pServer );
    thread->attachServer( m_pServer );

    MRX_T4Server *pLocalServer = (MRX_T4Server*)m_pServer;
    thread->setTimeout( pLocalServer->mTickTmo );

    //! server connection
    connect( thread, SIGNAL(signal_clean(QThread*)),
             pLocalServer, SLOT(slot_clean(QThread*)));

    logDbg_Thread()<<pLocalServer->mTickTmo<<socketDescriptor;

    thread->start();

}


