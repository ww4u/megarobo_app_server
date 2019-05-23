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
}

MAppServer::~MAppServer()
{
    qDeleteAll( mTcpServers );
}

int MAppServer::start()
{ return -1; }

int MAppServer::open()
{ return 0; }
void MAppServer::close()
{  }

MAppServer::ServerStatus MAppServer::status()
{ return state_idle; }

MAppServer::ServerStatus MAppServer::controllerStatus()
{
    if ( mConsoleServices.size() > 0 )
    { return state_working; }
    else
    { return state_idle; }
}

void MAppServer::on_dislink()
{}

bool MAppServer::isLinked()
{ return false; }

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

void MAppServer::interrupt( const QByteArray &ary )
{}

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

    //! console
    connect( pThread, SIGNAL(signal_clean(ConsoleThread*)),
             this, SLOT(slot_console_clean(ConsoleThread*)) );
}
void MAppServer::disconnectConsole( ConsoleThread *pThread )
{
    Q_ASSERT( NULL != pThread );

    mConsoleMutex.lock();
        mConsoleServices.removeAll( pThread );
        logDbg()<<mConsoleServices.size();
    mConsoleMutex.unlock();
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

void MAppServer::slot_console_clean( ConsoleThread *pThread )
{
    disconnectConsole( pThread );
}
