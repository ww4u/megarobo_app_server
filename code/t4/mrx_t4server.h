#ifndef MRX_T4SERVER_H
#define MRX_T4SERVER_H

#include "../mappserver.h"
#include "t4para.h"

class MRX_T4Server : public MAppServer, public T4Para
{
public:
    MRX_T4Server( int portBase=2345, int cnt = 3,
                  QObject *pParent = nullptr );

public:
    virtual int start();

    virtual int open();
    virtual void close();

    virtual MAppServer::ServerStatus status();
public:
    int load();

};

#endif // MRX_T4SERVER_H
