#include "let_tcpserver.h"
#include "let_service.h"

#include "../mydebug.h"
#include "MegaGateway.h"
#include "let_server.h"

Let_TcpServer::Let_TcpServer( QObject *parent ) : MTcpServer( parent )
{

}

void Let_TcpServer::incomingConnection(qintptr socketDescriptor)
{
    //! self thread
    MAppService *thread = new Let_Service(socketDescriptor, mPort, 0 );
    if ( NULL == thread )
    { return; }
    thread->moveToThread( thread );

    Q_ASSERT( NULL != m_pServer );
    thread->attachServer( m_pServer );

    Let_Server *pLocalServer = (Let_Server*)m_pServer;

    //! server connection
    connect( thread, SIGNAL(signal_clean(QThread*)),
             pLocalServer, SLOT(slot_clean(QThread*)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
