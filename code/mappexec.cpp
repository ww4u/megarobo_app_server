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
//    while( !mEventQueue.isEmpty() )
    while( true )
    {
        //! break
        if ( isInterruptionRequested() )
        { logDbg();break; }

        if ( mEventSema.tryAcquire( 1, 100 ) )
        {}
        else
        { continue; }

        mMutex.lock();
            pEvent = mEventQueue.dequeue();
        mMutex.unlock();

        Q_ASSERT( NULL != pEvent );

        //! enter
        emit signal_event_enter();
//        m_pService->mOutput.clear();

        onUserEvent( pEvent );

        //! exit
//        emit signal_event_exit( m_pService->mOutput );

        delete pEvent;
    }
}

void MAppExec::postEvent( QEvent *pEvent )
{
    mMutex.lock();
        mEventQueue.enqueue( pEvent );
//        logDbg()<<mEventQueue.size();
        mEventSema.release();
    mMutex.unlock();

    //! not running
    if ( isRunning() )
    {}
    else
    { start();}
}

void MAppExec::signal_output( QByteArray ary )
{
    emit signal_event_exit( ary );
}

bool MAppExec::onUserEvent( QEvent *pEvent )
{
    //! self event;
    if ( NULL != m_pService )
    {//! proc by the executor

        //! service exec
        m_pService->onUserEvent( pEvent );

    }

    return true;
}


