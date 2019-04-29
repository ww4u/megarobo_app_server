#ifndef MRX_T4TCPSERVER_H
#define MRX_T4TCPSERVER_H

#include "../mtcpserver.h"

class MRX_T4TcpServer : public MTcpServer
{
    Q_OBJECT
public:
    MRX_T4TcpServer( QObject *parent = nullptr );

protected:
    virtual void incomingConnection(qintptr socketDescriptor);

protected:
};

#endif // MRX_T4SERVER_H
