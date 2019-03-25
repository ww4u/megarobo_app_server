#include "mappservice.h"
#include "mydebug.h"
#include <QtCore>
#include <QThread>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>

#include "mappexec.h"

//! create the type
MServiceEvent::MServiceEvent( int tpe ) : QEvent( QEvent::Type(tpe) )
{}

void MServiceEvent::setPara( QVariant v1, QVariant v2, QVariant v3 )
{
    mVar1 = v1;
    mVar2 = v2;
    mVar3 = v3;
}

MAppService::MAppService(qintptr ptr, QObject *parent) : QThread(parent)
{
    mPtr = ptr;
    m_pSocket = NULL;

    m_pServer = NULL;
    m_pExec = NULL;
}

MAppService::~MAppService()
{
    logDbg();

    //! gc
    if ( m_pExec != NULL )
    {
        m_pExec->requestInterruption();
        delete m_pExec;
    }
}

void MAppService::run()
{
    QTcpSocket tSocket;

    bool b = tSocket.setSocketDescriptor( mPtr );
    logDbg_Thread()<<b<<tSocket.thread();

    connect( &tSocket, SIGNAL(readyRead()),
             this, SLOT(slot_dataIn()) );
    m_pSocket = &tSocket;    

    connect( &tSocket, SIGNAL(disconnected()),
             this, SLOT( quit() ) );
    connect( &tSocket, SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT( quit() ) );

    //! create the exec thread
    m_pExec = new MAppExec();
    m_pExec->attachService( this );
    m_pExec->attachSocket( m_pSocket );

    //! connect
    connect( m_pExec, SIGNAL(signal_event_enter()),
             this, SLOT(slot_event_enter()) );

    connect( m_pExec, SIGNAL(signal_event_exit( QByteArray  )),
             this, SLOT(slot_event_exit( QByteArray )) );

    m_pExec->moveToThread( m_pExec );
    m_pExec->start();

    QThread::run();

    logDbg()<<QThread::currentThread()<<"end";

    //! move to app
    moveToThread( QCoreApplication::instance()->thread() );
}

bool MAppService::onUserEvent( QEvent *pEvent )
{
    return true;
}

void MAppService::attachServer( MAppServer *pServer )
{
    Q_ASSERT( NULL != pServer );

    m_pServer = pServer;
}

//! post event
void MAppService::postEvent( int tpe, QVariant v1, QVariant v2, QVariant v3 )
{
    if ( NULL == m_pExec )
    {
        logDbg()<<"invalid exec";
        return;
    }

    //! new event
    MServiceEvent *pEvent = new MServiceEvent( tpe );
    if ( NULL == pEvent )
    {
        logDbg()<<"new event fail";
        return;
    }

    //! set data
    pEvent->setPara( v1, v2, v3 );

    m_pExec->postEvent( pEvent );
}

//! proc
void MAppService::proc( QJsonDocument &doc )
{
    QJsonObject jsonObj = doc.object();

    QString strCmdName;
    if ( jsonObj.contains("command") )
    {
        strCmdName = jsonObj.value( "command" ).toString();
    }
    else
    {
        logDbg()<<"invalid command";
        return;
    }

    QMapIterator< QString, P_PROC > iter( mProcMap );
    int ret;
    while( iter.hasNext() )
    {
        iter.next();

        //! match
        if ( QString::compare( iter.key(), strCmdName ) == 0 )
        {
            ret = (this->*(iter.value()))( doc );
            if ( ret != 0 )
            { logDbg()<<ret; }
            return;
        }
    }

    logDbg()<<"no command match";
}

void MAppService::output( const QJsonDocument &doc )
{
    mOutput = doc.toJson();
}

void MAppService::slot_dataIn( )
{
    QByteArray ary = m_pSocket->readAll();

    logDbg_Thread();
    logDbg()<<ary;

    QJsonDocument doc = QJsonDocument::fromJson( ary );
    if ( doc.isObject() )
    {
        postEvent( 1, doc );
    }
    else if ( doc.isArray() )
    {
        postEvent( 2, doc );
    }
    else
    {
        logDbg()<<"Invalid cmd";
    }

    //! output
    //! \todo output the socket
//    if ( mOutput.size() > 0 )
//    {
//        m_pSocket->write( mOutput );
//        m_pSocket->flush();
//        mOutput.clear();
//    }
}

void MAppService::slot_disconnect()
{}

void MAppService::slot_timeout()
{
    logDbg_Thread();
}

void MAppService::slot_event_enter()
{
    logDbg_Thread();
}
void MAppService::slot_event_exit( QByteArray ary )
{
    logDbg_Thread();

    if ( ary.size() > 0 )
    {
        m_pSocket->write( ary );
        m_pSocket->flush();
        ary.clear();
    }
}

//! thread
WorkingThread::WorkingThread( QObject *parent ) : QThread( parent )
{
}

void WorkingThread::run()
{
    ProxyApi* pApi;
    int ret = -1;
    while( !mQueue.isEmpty() )
    {
        //! rst the ret
        ret = -1;

        if ( isInterruptionRequested() )
        {
            break;
        }

        pApi = mQueue.dequeue();
        if ( NULL == pApi || NULL == pApi->m_pObj )
        { continue; }

        logDbg()<<pApi->mApiName<<"enter";

        //! failed
        QJsonDocument doc = pApi->mVar.toJsonDocument();
        ret = (((pApi->m_pObj)->*(pApi->m_pProc)))( doc );
        if ( ret != 0 )
        {
            //! post the event
        }

        logDbg()<<pApi->mApiName<<"exit";

        delete pApi;
    }

}

int WorkingThread::attachProc( MAppService *pObj,
                                MAppService::P_PROC proc,
                                const QString &name,
                                QVariant var )
{
    ProxyApi *pApi;

    pApi = new ProxyApi();
    if ( NULL == pApi )
    { return -1; }

    pApi->m_pObj = pObj;
    pApi->m_pProc = proc;
    pApi->mVar = var;
    pApi->mApiName = name;

    mMutex.lock();
        mQueue.enqueue( pApi );
    mMutex.unlock();

    if ( isRunning() )
    {}
    else
    {
        start();
    }

    return 0;
}
