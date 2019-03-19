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
