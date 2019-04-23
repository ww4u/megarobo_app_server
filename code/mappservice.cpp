#include "mappservice.h"
#include "mydebug.h"
#include <QtCore>
#include <QThread>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>

#include "mappexec.h"
#include "syspara.h"
#include "intfobj.h"
#include "myjson.h"

//! create the type
MServiceEvent::MServiceEvent( int tpe ) : QEvent( QEvent::Type(tpe) )
{
    mTs = QDateTime::currentMSecsSinceEpoch();
}

void MServiceEvent::setPara( QVariant v1, QVariant v2, QVariant v3 )
{
    mTs = QDateTime::currentMSecsSinceEpoch();

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

    m_pTimer = NULL;
    mbTmo = false;

    mTimeout = 0;
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

    qDeleteAll( mProcMap );
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

void MAppService::setTimeout( int tmo )
{ mTimeout = tmo; }
int MAppService::timeout()
{ return mTimeout; }

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

int MAppService::attachProc( const QString &name, MAppService::P_PROC proc, quint64 tmo )
{
    ProxyApi *pApi;

    pApi = new ProxyApi();
    if ( NULL == pApi )
    { return -1; }

    pApi->mApiName = name;
    pApi->mTmo = tmo;
    pApi->m_pProc = proc;

    pApi->m_pObj = this;
    pApi->mLastTs = 0;

    //! inser the map
    mProcMap.insert( name, pApi );

    return 0;
}

void MAppService::dataProc( )
{
    //! find the '#' in the array

    QByteArray packet;
    QJsonDocument doc;
    int index;
    do
    {
        index = mRecvCache.indexOf( '#' );
        if ( index >= 0 )
        {
            packet = mRecvCache.mid( 0, index + 1 );
            mRecvCache.remove( 0, index + 1 );

            //! remove the '#'
            packet.remove( index, 1 );

            //! parse the packet
            doc = QJsonDocument::fromJson( packet );
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
        }
        else
        {}

    }while( index != -1 );
}

//! proc
void MAppService::proc( QJsonDocument &doc, quint64 &ts )
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

    QMapIterator< QString, ProxyApi* > iter( mProcMap );
    ProxyApi *pApi;
    int ret;
    while( iter.hasNext() )
    {
        iter.next();

        //! match
        if ( QString::compare( iter.key(), strCmdName ) == 0 )
        {
            pApi = iter.value();
            Q_ASSERT( NULL != pApi );

            //! check time -- time filter?
            if ( pApi->mTmo > 0 )
            {
                //! timeout
                if ( ts > ( pApi->mLastTs + pApi->mTmo ) )
                {}
                //! filtered
                else
                {
                    logDbg()<<"filtered";
                    return;
                }
            }
            else
            {}

            //! save the last ts
            pApi->mLastTs = ts;

            ret = (this->*(pApi->m_pProc))( doc );
            if ( ret != 0 )
            { logDbg()<<ret; }

            return;
        }
    }

    logDbg()<<"no command match";
}

void MAppService::output( const QJsonDocument &doc )
{
//    mOutput = doc.toJson();
    //! \note append the terminator
//    mOutput.append( '#' );

    QByteArray outAry = doc.toJson();
    m_pExec->signal_output( outAry );

    sysLogOut( outAry );

//    sysLogOut( mOutput );
}

void MAppService::resetTimeout()
{
    if ( mTimeout > 0 )
    {}
    else
    { return; }

    //! timer
    if ( NULL == m_pTimer )
    {
        m_pTimer = new QTimer( this );
        if ( NULL == m_pTimer )
        { return; }

        connect( m_pTimer, SIGNAL(timeout()),
                 this, SLOT(slot_timeout()) );
    }

    mbTmo = false;
    m_pTimer->start( mTimeout );
    logDbg_Thread();
}

void MAppService::slot_dataIn( )
{
    QByteArray ary = m_pSocket->readAll();

    if ( sysHasArg("-showin") )
    { logDbg()<<ary; }

    //! receive cache
    mRecvCache.append( ary );

    resetTimeout();

    sysLogIn( ary );

    dataProc();
}

void MAppService::slot_disconnect()
{}

void MAppService::slot_timeout()
{
    IntfNotify var;

    //! have timeout
    if ( mbTmo )
    {
        var.code = IntfNotify::e_notify_socket_idle_quit;
        var.message = "socket idle close";
    }
    else
    {
        var.code = IntfNotify::e_notify_socket_idle_timeout;
        var.message = "socket idle timeout";
    }

    //! obj
    QJsonObject obj;
    json_obj( command );
    json_obj( message );
    json_obj( code );

    //! doc
    QJsonDocument doc;
    doc.setObject( obj );

    //! output
    output( doc );

    //! flush
    slot_event_exit( mOutput );

    if ( mbTmo )
    { quit(); }
    else
    {}

    logDbg_Thread();
    mbTmo = true;
}

void MAppService::slot_event_enter()
{
    logDbg_Thread();
}
void MAppService::slot_event_exit( QByteArray ary )
{

    if ( ary.size() > 0 )
    {//logDbg_Thread()<<ary;
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

        try
        {
            logDbg()<<pApi->mApiName<<"enter";

            //! failed
            QJsonDocument doc = pApi->mVar.toJsonDocument();
            ret = (((pApi->m_pObj)->*(pApi->m_pProc)))( doc );
            if ( ret != 0 )
            {
                //! post the event
            }

            logDbg()<<pApi->mApiName<<"exit";
        }
        catch( QException &e )
        {   
            delete pApi;

            qDeleteAll( mQueue );
            mQueue.clear();

            break;
        }
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
