#include "mappserver.h"
#include "mtcpserver.h"
#include "./mydebug.h"
MAppServer::MAppServer( int portBase, int cnt, QObject *parent) : QObject( parent)
{
    mPorts.clear();
    for ( int i = 0; i < cnt; i++ )
    {
        mPorts<<(portBase+i);
    }

    //! eop
    mSendEOP = "\r\n";
    mRecvEOP = "#";
}

MAppServer::~MAppServer()
{
    qDeleteAll( mTcpServers );
}

int MAppServer::start()
{
    if ( assureHomePath() )
    { return 0; }

    return -1;
}

int MAppServer::open()
{ return 0; }
void MAppServer::close()
{  }

MAppServer::ServerStatus MAppServer::status()
{ return state_idle; }

MAppServer::ServerStatus MAppServer::controllerStatus()
{logDbg()<<mConsoleServices.size();
    if ( mConsoleServices.size() > 0 )
    { return state_working; }
    else
    { return state_idle; }
}

void MAppServer::on_dislink()
{}

bool MAppServer::isLinked()
{ return false; }

void MAppServer::setSendEOP( const QString &eop )
{ mSendEOP = eop; }
QString MAppServer::sendEOP()
{ return mSendEOP; }

void MAppServer::setRecvEOP( const QString &eop )
{ mRecvEOP = eop; }
QString MAppServer::recvEOP()
{ return mRecvEOP; }

int MAppServer::services()
{
    return mServices.size();
}

void MAppServer::registerService( QThread *pService )
{
    mServiceMutex.lock();
        if ( mServices.contains( pService ))
        {}
        else
        { mServices.append( pService ); }
        logDbg()<<mServices.size();
    mServiceMutex.unlock();
}

int MAppServer::workings()
{
    return mWorkings.size();
}

void MAppServer::interrupt( const QByteArray &ary )
{}

void MAppServer::setServerName( const QString &name )
{ mServerName = name; }
QString MAppServer::serverName()
{ return mServerName; }

bool MAppServer::assureHomePath()
{
    QString path = homePath();

    //! create path
    QDir dir( path );
    if ( dir.exists() )
    { return true; }
    else
    {
        if ( dir.mkpath( path) )
        { return true; }
        else
        { return false; }
    }
}

QString MAppServer::homePath()
{
    QString homePath = qApp->applicationDirPath() + "/" + "file/" + mServerName;
    return homePath;
}

void MAppServer::connectWorking( WorkingThread *pWorking )
{
    Q_ASSERT( NULL != pWorking );
    mWorkingMutex.lock();
        mWorkings.append( pWorking );
    mWorkingMutex.unlock();
}
void MAppServer::disconnectWorking( WorkingThread *pWorking )
{
    Q_ASSERT( NULL != pWorking );
    mWorkingMutex.lock();
        mWorkings.removeAll( pWorking );
    mWorkingMutex.unlock();
}

void MAppServer::connectConsole( ConsoleThread *pThread )
{
    Q_ASSERT( NULL != pThread );

    mConsoleMutex.lock();
        mConsoleServices.append( pThread );
    mConsoleMutex.unlock();
logDbg()<<mConsoleServices.size();
    //! console
    connect( pThread, SIGNAL(signal_clean(ConsoleThread*)),
             this, SLOT(slot_console_clean(ConsoleThread*)) );

    connect( pThread, SIGNAL(signal_start(ConsoleThread*)),
             this, SLOT(slot_console_start(ConsoleThread*))) ;
}
void MAppServer::disconnectConsole( ConsoleThread *pThread )
{
    Q_ASSERT( NULL != pThread );

    mConsoleMutex.lock();
        mConsoleServices.removeAll( pThread );
        logDbg()<<mConsoleServices.size();
    mConsoleMutex.unlock();
}

void MAppServer::stopWorkings()
{
    mConsoleMutex.lock();
        logDbg()<<mConsoleServices.size();
        foreach( ConsoleThread *pThread, mConsoleServices )
        { pThread->requestInterruption(); }

        foreach( ConsoleThread *pThread, mConsoleServices )
        { pThread->wait(); }
    mConsoleMutex.unlock();

    mWorkingMutex.lock();
        foreach( WorkingThread *pThread, mWorkings )
        { pThread->requestInterruption(); }

        foreach( WorkingThread *pThread, mWorkings )
        { pThread->wait(); }
    mWorkingMutex.unlock();
}

void MAppServer::slot_clean( QThread * pThread )
{
    mServiceMutex.lock();
        mServices.removeAll( pThread );
        logDbg()<<mServices.size();

        if ( mServices.size() < 1 )
        { on_dislink(); }
        else
        {}

    mServiceMutex.unlock();
}

void MAppServer::slot_console_start( ConsoleThread *pThread )
{}

void MAppServer::slot_console_clean( ConsoleThread *pThread )
{
    disconnectConsole( pThread );
}
