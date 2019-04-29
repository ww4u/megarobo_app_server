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

#include "mappserver.h"

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

    connect( this, SIGNAL(finished()),this, SLOT(slot_finished()) );
}

MAppService::~MAppService()
{
    logDbg();

    //! gc working
    pre_quit();

    qDeleteAll( mProcMap );
}

void MAppService::run()
{
    QTcpSocket tSocket;

    bool b = tSocket.setSocketDescriptor( mPtr );
//    logDbg_Thread()<<b<<tSocket.thread();

    connect( &tSocket, SIGNAL(readyRead()),
             this, SLOT(slot_dataIn()) );
    m_pSocket = &tSocket;    

    //! quit the service
    connect( &tSocket, SIGNAL(disconnected()),
             this, SLOT( slot_on_socket_disconnect() ) );
    connect( &tSocket, SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT( slot_on_socket_error(QAbstractSocket::SocketError ) ) );

    //! create the exec thread
    m_pExec = new MAppExec();
    m_pExec->attachService( this );
    m_pExec->attachSocket( m_pSocket );

    //! move to self thread
    m_pExec->moveToThread( m_pExec );

    //! connect
    connect( m_pExec, SIGNAL(signal_event_enter()),
             this, SLOT(slot_event_enter()) );

    connect( m_pExec, SIGNAL(signal_event_exit( QByteArray  )),
             this, SLOT(slot_event_exit( QByteArray )) );

    m_pExec->start();

    QThread::run();

    logDbg()<<QThread::currentThread()<<"end";

    //! move to main thread
    moveToThread( QCoreApplication::instance()->thread() );
    logDbg();
}

bool MAppService::event(QEvent *event)
{
    //! output
    if ( event->type() == MServiceEvent::e_serv_event_output )
    {
        on_event_output( event );
        return true;
    }
    else if ( event->type() == MServiceEvent::e_serv_event_quit )
    {
        on_event_quit( event );
        return true;
    }
    else
    {}

    return QThread::event( event );
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


            //! pre
            if ( pApi->m_pPreProc!= NULL )
            { ret = (this->*(pApi->m_pPreProc))( doc ); }

            if ( pApi->m_pProc != NULL )
            {
                ret = (this->*(pApi->m_pProc))( doc );
                if ( ret != 0 )
                { logDbg()<<ret; }
            }

            //! post
            if ( pApi->m_pPostProc!= NULL )
            { ret = (this->*(pApi->m_pPostProc))( doc ); }

            return;
        }
    }

    logDbg()<<"no command match";
}

void MAppService::output( const QJsonDocument &doc )
{
    //! data length
    QByteArray outAry = doc.toJson();
    outAry = outAry.simplified();
    outAry.append('\n');

    //! use the \n for seperator
    outAry = outAry.simplified();
    if ( outAry.length() > 0 )
    { outAry.append('\n'); }
    else
    { return; }

    mExecMutex.lock();
    if ( m_pExec != NULL && !m_pExec->isInterruptionRequested() )
    {
//        m_pExec->signal_output( outAry );

        mExecMutex.unlock();

        do
        {
            MServiceEvent *pEvent = new MServiceEvent( MServiceEvent::e_serv_event_output );
            if ( NULL == pEvent )
            { break; }
            pEvent->setPara( outAry );

            qApp->postEvent( this, pEvent );

        }while( false );

        sysLogOut( outAry );
    }
    else
    { mExecMutex.unlock(); }

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

#define WAIT_TIME_OUT   60000
void MAppService::pre_quit()
{
    //! request
    if ( m_pExec != NULL )
    {
        m_pExec->requestInterruption();
    }

    //! clean
    if ( m_pExec != NULL )
    {
        do
        {logDbg();
            if ( m_pExec->wait( WAIT_TIME_OUT ) )
            {logDbg();
            }
            else
            {logDbg();
                m_pExec->terminate();
                if ( m_pExec->wait( WAIT_TIME_OUT ) )
                {
                }
                else
                {
                    logWarning()<<"exec gc fail";
                    break;
                }
            }

            mExecMutex.lock();
                delete m_pExec;
                m_pExec = NULL;
            mExecMutex.unlock();

        }while( 0 );

        mExecMutex.lock();
            m_pExec = NULL;
        mExecMutex.unlock();
    }

    //! working
    if ( NULL != m_pWorkingThread )
    {
        m_pWorkingThread->requestInterruption();
    }

    if ( NULL != m_pWorkingThread )
    {
        m_pServer->disconnectWorking( m_pWorkingThread );

        do
        {logDbg();
            if ( m_pWorkingThread->wait( WAIT_TIME_OUT ) )
            {logDbg();
            }
            else
            {logDbg();
                m_pWorkingThread->terminate();
                if ( m_pWorkingThread->wait( WAIT_TIME_OUT ) )
                { }
                else
                {
                    logWarning()<<"exec gc fail";
                    break;
                }
            }

            delete m_pWorkingThread;
        }while( 0 );

        m_pWorkingThread = NULL;
    }
}

void MAppService::on_event_output( QEvent *pEvent )
{
    MServiceEvent *pServEvent = (MServiceEvent*)pEvent;
    if ( NULL == pServEvent )
    { return;  }

    //! ary
    QByteArray ary = pServEvent->mVar1.toByteArray();

    if ( ary.size() > 0 && m_pSocket->isOpen() && m_pSocket->isWritable() )
    {//logDbg_Thread()<<ary;
        mSendMutex.lock();
            m_pSocket->write( ary );
            m_pSocket->flush();
//            QThread::msleep( 10 );
        mSendMutex.unlock();
//        ary.clear();
    }
}

void MAppService::on_event_quit( QEvent *pEvent )
{ quit(); }

void MAppService::slot_finished()
{
    emit signal_clean( this );
}

void MAppService::slot_dataIn( )
{
    mSendMutex.lock();
        QByteArray ary = m_pSocket->readAll();
    mSendMutex.unlock();

    if ( sysHasArg("-showin") )
    { logDbg()<<ary; }

    //! receive cache
    mRecvCache.append( ary );

    //! register this
    m_pServer->registerService( this );

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

    //! close the timer
    if ( mbTmo )
    {
        MServiceEvent *pEvent = new MServiceEvent( MServiceEvent::e_serv_event_quit );
        if ( NULL == pEvent )
        { return; }

        qApp->postEvent( this, pEvent );
    }
    else
    {
        //! 10s leaving
        m_pTimer->setInterval( 10000 );
    }

    logDbg_Thread();
    mbTmo = true;
}

void MAppService::slot_event_enter()
{
//    logDbg_Thread();
}
void MAppService::slot_event_exit( QByteArray ary )
{

    if ( ary.size() > 0 )
    {//logDbg_Thread()<<ary;
        mSendMutex.lock();
            m_pSocket->write( ary );
            m_pSocket->flush();
        mSendMutex.unlock();
//        ary.clear();
    }
}

void MAppService::slot_on_socket_error( QAbstractSocket::SocketError err )
{
    logDbg()<<err;

    pre_quit();

    quit();
}

void MAppService::slot_on_socket_disconnect()
{
    logDbg();

    pre_quit();

    quit();
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
//            logDbg()<<pApi->mApiName<<"enter";
            QJsonDocument doc = pApi->mVar.toJsonDocument();

            //! pre
            if ( pApi->m_pPreProc!= NULL )
            { ret = (((pApi->m_pObj)->*(pApi->m_pPreProc)))( doc ); }

            //! post do
            ret = (((pApi->m_pObj)->*(pApi->m_pProc)))( doc );
            if ( ret != 0 )
            {
                //! post the event
            }

            //! post
            if ( pApi->m_pPostProc!= NULL )
            { ret = (((pApi->m_pObj)->*(pApi->m_pPostProc)))( doc ); }

//            logDbg()<<pApi->mApiName<<"exit";
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
                                MAppService::P_PROC preProc,
                                MAppService::P_PROC postProc,
                                const QString &name,
                                QVariant var )
{
    ProxyApi *pApi;

    pApi = new ProxyApi();
    if ( NULL == pApi )
    { return -1; }

    pApi->m_pObj = pObj;
    pApi->m_pProc = proc;

    pApi->m_pPreProc = preProc;
    pApi->m_pPostProc = postProc;

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
