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

void MAppServer::connectWorking( WorkingThread *pWorking )
{
    Q_ASSERT( NULL != pWorking );
    mWorkings.append( pWorking );
}
void MAppServer::disconnectWorking( WorkingThread *pWorking )
{
    Q_ASSERT( NULL != pWorking );
    mWorkings.removeAll( pWorking );
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
