#ifndef MAPPSERVICE_H
#define MAPPSERVICE_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QEvent>

#include "mappexec.h"

class MAppServer;

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
    typedef int (MAppService::*P_PROC)( const QJsonDocument &obj  );

public:
    explicit MAppService( qintptr ptr, QObject *parent = nullptr);
    ~MAppService();

protected:
    virtual void run();
public:
    virtual bool onUserEvent( QEvent *pEvent );

public:
    void attachServer( MAppServer *pServer );

protected:
    void postEvent( int tpe, QVariant v1=0, QVariant v2=0, QVariant v3=0 );

protected:
    void proc( QJsonDocument &jsonObj );
    void output( const QJsonDocument &obj );

protected:
    qintptr mPtr;
    QTcpSocket *m_pSocket;

    QByteArray mOutput;

protected:
    QMap< QString, P_PROC > mProcMap;

    MAppServer *m_pServer;
    MAppExec   *m_pExec;            //! executor service

signals:

public slots:
    void slot_dataIn( );
    void slot_disconnect();

    void slot_timeout();

    void slot_event_enter();
    void slot_event_exit( QByteArray ary );
};

#endif // MAPPSERVICE_H
