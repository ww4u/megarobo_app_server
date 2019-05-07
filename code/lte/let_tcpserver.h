#ifndef LET_TCPSERVER_H
#define LET_TCPSERVER_H

#include "../mtcpserver.h"

class Let_TcpServer : public MTcpServer
{
    Q_OBJECT
public:
    Let_TcpServer( QObject *parent = nullptr );

protected:
    virtual void incomingConnection(qintptr socketDescriptor);

protected:
};

#endif // LET_TCPSERVER_H
