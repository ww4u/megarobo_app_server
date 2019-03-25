#ifndef MAPPSERVER_H
#define MAPPSERVER_H

#include <QObject>
#include "serverintf.h"

class MTcpServer;
class WorkingThread;

class MAppServer : public QObject, public ServerIntf
{
    Q_OBJECT
public:
    enum ServerStatus
    {
        state_idle,
        state_working,
    };

public:
    explicit MAppServer(QObject *parent = nullptr);
    virtual ~MAppServer();

public:
    virtual int start();

    virtual int open();
    virtual void close();

    virtual ServerStatus status();

public:
    void connectWorking( WorkingThread *pWorking );
    void disconnectWorking( WorkingThread *pWorking );

protected:
    QList< MTcpServer *> mTcpServers;
    QList< quint16 > mPorts;

    QList<WorkingThread *> mWorkings;
signals:

public slots:
};

#endif // MAPPSERVER_H
