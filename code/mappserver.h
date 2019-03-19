#ifndef MAPPSERVER_H
#define MAPPSERVER_H

#include <QObject>
#include "serverintf.h"

class MTcpServer;

class MAppServer : public QObject, public ServerIntf
{
    Q_OBJECT
public:
    explicit MAppServer(QObject *parent = nullptr);
    virtual ~MAppServer();

public:
    virtual int start();

    virtual int open();
    virtual void close();

protected:
    QList< MTcpServer *> mTcpServers;
    QList< quint16 > mPorts;

signals:

public slots:
};

#endif // MAPPSERVER_H
