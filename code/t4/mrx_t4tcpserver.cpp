#include "mrx_t4tcpserver.h"
#include "mrx_t4service.h"

#include "../mydebug.h"
#include "../device/MegaGateway.h"

MRX_T4TcpServer::MRX_T4TcpServer( QObject *parent ) : MTcpServer( parent )
{   
}

void MRX_T4TcpServer::incomingConnection(qintptr socketDescriptor)
{
    //! self thread
    MAppService *thread = new MRX_T4Service(socketDescriptor, 0 );
    thread->moveToThread( thread );
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    Q_ASSERT( NULL != m_pServer );
    thread->attachServer( m_pServer );

    thread->start();
}


