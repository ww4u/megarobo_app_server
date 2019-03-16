#include "mappservice.h"
#include "mydebug.h"
#include <QThread>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonObject>

MAppService::MAppService(qintptr ptr, QObject *parent) : QThread(parent)
{
    mPtr = ptr;
    m_pSocket = NULL;

    m_pServer = NULL;

}
MAppService::~MAppService()
{
    logDbg();
}

void MAppService::run()
{
    QTcpSocket tSocket;

    bool b = tSocket.setSocketDescriptor( mPtr );
    logDbg()<<b<<QThread::currentThreadId()<<tSocket.thread();

    connect( &tSocket, SIGNAL(readyRead()),
             this, SLOT(slot_dataIn()) );
    m_pSocket = &tSocket;

    QThread::run();
}

void MAppService::attachServer( MAppServer *pServer )
{
    Q_ASSERT( NULL != pServer );

    m_pServer = pServer;
}

//! proc
void MAppService::proc( const QJsonObject &jsonObj )
{
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
    while( iter.hasNext() )
    {
        iter.next();

        //! match
        if ( QString::compare( iter.key(), strCmdName ) == 0 )
        {
            (this->*(iter.value()))( jsonObj );
            return;
        }
    }

    logDbg()<<"no command match";
}

void MAppService::slot_dataIn( )
{
    QByteArray ary = m_pSocket->readAll();

    logDbg()<<QThread::currentThreadId();
    logDbg()<<ary;

    QJsonDocument doc = QJsonDocument::fromJson( ary );
    if ( doc.isObject() )
    {
//        logDbg()<<doc.object();
        proc( doc.object() );
    }
    else
    {
        logDbg()<<"Invalid cmd";
    }

    //! output
    if ( mOutput.size() > 0 )
    {
        m_pSocket->write( mOutput );
        m_pSocket->flush();
        mOutput.clear();
    }
}

void MAppService::slot_disconnect()
{}

void MAppService::slot_timeout()
{
    logDbg()<<QThread::currentThreadId();
}
