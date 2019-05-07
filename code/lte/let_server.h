#ifndef LET_SERVER_H
#define LET_SERVER_H

#include "../mappserver.h"
#include "letpara.h"

#include "axes.h"

class Let_Server : public MAppServer, public LetPara
{
public:
    Let_Server( int portBase=2345, int cnt = 1,
                QObject *pParent = nullptr );

public:
    virtual int start();

    virtual int open();
    virtual void close();
    virtual void rst();

    virtual MAppServer::ServerStatus status();
    virtual void on_dislink();
    virtual bool isLinked();

public:
    int load();

public:
    Axes mZAxes;

};

#endif // LET_SERVER_H
