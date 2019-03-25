#include "mappserver.h"
#include "mtcpserver.h"

MAppServer::MAppServer(QObject *parent) : QObject(parent)
{

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
