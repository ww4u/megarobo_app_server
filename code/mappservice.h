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
    quint64 mTs;
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

public:
    void setTimeout( int tmo );
    int timeout();

protected:
    void postEvent( int tpe, QVariant v1=0, QVariant v2=0, QVariant v3=0 );

protected:
    int attachProc( const QString &name, MAppService::P_PROC proc, quint64 tmo = 0 );

    void dataProc( );

    void proc( QJsonDocument &jsonObj, quint64 &ts );
    void output( const QJsonDocument &obj );

    void resetTimeout();

protected:
    qintptr mPtr;
    QTcpSocket *m_pSocket;

    QByteArray mRecvCache;
    QByteArray mOutput;

protected:
    QMap< QString, ProxyApi * > mProcMap;

    MAppServer *m_pServer;
    MAppExec   *m_pExec;            //! executor service

    WorkingThread *m_pWorkingThread;

    QTimer *m_pTimer;
    bool mbTmo;
    int mTimeout;
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
    int attachProc(  MAppService *pObj,
                     MAppService::P_PROC proc,
                     MAppService::P_PROC preProc,
                     MAppService::P_PROC postProc,
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

    MAppService::P_PROC m_pPreProc;

    MAppService::P_PROC m_pProc;

    MAppService::P_PROC m_pPostProc;

    QVariant mVar;
    QString mApiName;

    quint64 mTmo;       //! 0 no timeout needed
    quint64 mLastTs;    //! latest timestamp

public:
    ProxyApi()
    {
        m_pObj = NULL;
        m_pProc = NULL;

        m_pPreProc = NULL;
        m_pPostProc = NULL;

        mTmo = 0;
        mLastTs = 0;
    }

};

#endif // MAPPSERVICE_H
