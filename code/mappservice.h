#ifndef MAPPSERVICE_H
#define MAPPSERVICE_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QEvent>

#include "mappexec.h"

class MAppServer;
class WorkingThread;
class ProxyApi;

class MServiceEvent : public QEvent
{
public:
    MServiceEvent( int tpe );

    void setPara( QVariant v1, QVariant v2=0, QVariant v3=0 );

public:
    QVariant mVar1, mVar2, mVar3;
};

class MAppService : public QThread
{
    friend class MAppExec;
    Q_OBJECT

public:
    typedef int (MAppService::*P_PROC)( QJsonDocument &obj  );

public:
    explicit MAppService( qintptr ptr, QObject *parent = nullptr);
    ~MAppService();

protected:
    virtual void run();
public:
    virtual bool onUserEvent( QEvent *pEvent );

public:
    virtual void attachServer( MAppServer *pServer );

protected:
    void postEvent( int tpe, QVariant v1=0, QVariant v2=0, QVariant v3=0 );

protected:
    void dataProc( );

    void proc( QJsonDocument &jsonObj );
    void output( const QJsonDocument &obj );

protected:
    qintptr mPtr;
    QTcpSocket *m_pSocket;

    QByteArray mRecvCache;
    QByteArray mOutput;

protected:
    QMap< QString, P_PROC > mProcMap;

    MAppServer *m_pServer;
    MAppExec   *m_pExec;            //! executor service

    WorkingThread *m_pWorkingThread;
signals:

public slots:
    void slot_dataIn( );
    void slot_disconnect();

    void slot_timeout();

    void slot_event_enter();
    void slot_event_exit( QByteArray ary );
};

class WorkingThread : public QThread
{
public:
    WorkingThread( QObject *parent = nullptr );

protected:
    virtual void run();

public:
    int attachProc( MAppService *pObj,
                     MAppService::P_PROC proc,
                     const QString &name,
                     QVariant var );

protected:
    QQueue<ProxyApi*> mQueue;
    QMutex mMutex;
};

class ProxyApi
{
public:
    MAppService *m_pObj;
    MAppService::P_PROC m_pProc;
    QVariant mVar;
    QString mApiName;

public:
    ProxyApi()
    {
        m_pObj = NULL;
        m_pProc = NULL;
    }

};

#endif // MAPPSERVICE_H
