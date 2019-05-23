#ifndef MAPPSERVER_H
#define MAPPSERVER_H

#include <QObject>
#include "serverintf.h"

class MTcpServer;
class WorkingThread;
class ConsoleThread;

class MAppServer : public QObject, public ServerIntf
{
    Q_OBJECT
public:
    enum ServerStatus
    {
        state_idle,
        state_working,
        state_pending,
    };

public:
    explicit MAppServer( int portBase, int cnt, QObject *parent = nullptr);
    virtual ~MAppServer();

public:
    virtual int start();

    virtual int open();
    virtual void close();

    virtual ServerStatus status();
    virtual ServerStatus controllerStatus();

    virtual void on_dislink();
    virtual bool isLinked();

    int services();
    void registerService( QThread *pService );

    //! interrupt
    virtual void interrupt( const QByteArray &ary );

public:
    void connectWorking( WorkingThread *pWorking );
    void disconnectWorking( WorkingThread *pWorking );

    void connectConsole( ConsoleThread *pThread );
    void disconnectConsole( ConsoleThread *pThread );

signals:

protected Q_SLOTS:
    void slot_clean( QThread * );
    void slot_console_clean( ConsoleThread *pThread );

protected:
    QList< MTcpServer *> mTcpServers;
    QList< quint16 > mPorts;

    QMutex mWorkingMutex;
    QList<WorkingThread *> mWorkings;

    QMutex mServiceMutex;
    QList<QThread*> mServices;

    QMutex mConsoleMutex;
    QList<ConsoleThread*> mConsoleServices;

};

#endif // MAPPSERVER_H
