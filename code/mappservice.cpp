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
void MAppService::proc( QJsonObject &jsonObj )
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
    int ret;
    while( iter.hasNext() )
    {
        iter.next();

        //! match
        if ( QString::compare( iter.key(), strCmdName ) == 0 )
        {
            ret = (this->*(iter.value()))( jsonObj );
            if ( ret != 0 )
            { logDbg()<<ret; }
            return;
        }
    }

    logDbg()<<"no command match";
}

void MAppService::output( const QJsonObject &obj )
{
    QJsonDocument doc( obj );

    mOutput = doc.toJson();
}

void MAppService::slot_dataIn( )
{
    QByteArray ary = m_pSocket->readAll();

    logDbg()<<QThread::currentThreadId();
    logDbg()<<ary;

    QJsonDocument doc = QJsonDocument::fromJson( ary );
    QJsonObject localObj;
    if ( doc.isObject() )
    {
//        logDbg()<<doc.object();
        localObj = doc.object();
        proc( localObj );
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
