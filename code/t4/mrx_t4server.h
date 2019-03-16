#ifndef MRX_T4SERVER_H
#define MRX_T4SERVER_H

#include "./mappserver.h"
#include "t4para.h"

class MRX_T4Server : public MAppServer, T4Para
{
public:
    MRX_T4Server( QObject *parent = nullptr );

protected:
    virtual void incomingConnection(qintptr socketDescriptor);

public:
    virtual int open();
    virtual void close();
};

#endif // MRX_T4SERVER_H
