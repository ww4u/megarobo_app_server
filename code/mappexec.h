#ifndef MAPPEXEC_H
#define MAPPEXEC_H

#include <QtCore>
#include <QThread>
#include <QTcpSocket>

class MAppService;

class MAppExec : public QThread
{
    Q_OBJECT
public:
    MAppExec( QObject *pParent = nullptr );

    void attachService( MAppService *pService );
    void attachSocket( QTcpSocket *pSocket );

protected:
    virtual void run();

public:
    void postEvent( QEvent *pEvent );

protected:
    virtual bool onUserEvent( QEvent *pEvent );

protected:
    MAppService *m_pService;
    QTcpSocket *m_pOutSocket;

    QMutex mMutex;
    QQueue<QEvent*> mEventQueue;

Q_SIGNALS:
    void signal_event_enter();
    void signal_event_exit( QByteArray ary );

};

#endif // MAPPEXEC_H
