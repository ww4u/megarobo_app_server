#include "mappexec.h"
#include "mappservice.h"

#include "mydebug.h"

MAppExec::MAppExec( QObject *pParent ) : QThread( pParent )
{
    m_pService = NULL;
    m_pOutSocket = NULL;
}

void MAppExec::attachService( MAppService *pService )
{
    Q_ASSERT( NULL != pService );

    m_pService = pService;
}

void MAppExec::attachSocket( QTcpSocket *pSocket )
{
    Q_ASSERT( NULL != pSocket );
    m_pOutSocket = pSocket;
}

void MAppExec::run()
{
    QEvent *pEvent;

    //! proc the queue
    while( !mEventQueue.isEmpty() )
    {
        //! break
        if ( isInterruptionRequested() )
        { break; }

        pEvent = mEventQueue.dequeue();

        Q_ASSERT( NULL != pEvent );

        //! enter
        emit signal_event_enter();
        m_pService->mOutput.clear();

        onUserEvent( pEvent );

        //! exit
        emit signal_event_exit( m_pService->mOutput );

        delete pEvent;
    }
}

void MAppExec::postEvent( QEvent *pEvent )
{
    mMutex.lock();
    mEventQueue.enqueue( pEvent );
    mMutex.unlock();

    //! not running
    if ( isRunning() )
    {}
    else
    { start(); }
}

bool MAppExec::onUserEvent( QEvent *pEvent )
{
    //! self event;
    if ( NULL != m_pService )
    {//! proc by the executor

        //! service exec
        m_pService->onUserEvent( pEvent );

//        //! output the socket
//        if ( m_pService->mOutput.size() > 0 )
//        {
//            if ( m_pOutSocket != NULL )
//            {
//                m_pOutSocket->write( m_pService->mOutput );
//                m_pOutSocket->flush();
//            }
//        }
    }

    return true;
}


