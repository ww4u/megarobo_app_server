#ifndef MAPPSERVICE_H
#define MAPPSERVICE_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>

class MAppServer;
class MAppService : public QThread
{
    Q_OBJECT

public:
    typedef void (MAppService::*P_PROC)( const QJsonObject &obj  );

public:
    explicit MAppService( qintptr ptr, QObject *parent = nullptr);
    ~MAppService();

protected:
    virtual void run();

public:
    void attachServer( MAppServer *pServer );

protected:
    void proc( const QJsonObject &jsonObj );

private:
    qintptr mPtr;
    QTcpSocket *m_pSocket;

    QByteArray mOutput;

protected:
    QMap< QString, P_PROC > mProcMap;

    MAppServer *m_pServer;

signals:

public slots:
    void slot_dataIn( );
    void slot_disconnect();

    void slot_timeout();

};

#endif // MAPPSERVICE_H
