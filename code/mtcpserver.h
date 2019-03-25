#ifndef MTCPSERVER_H
#define MTCPSERVER_H

#include <QObject>
#include <QTcpServer>

#include "mappservice.h"

class MAppServer;
class WorkingThread;

class MTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MTcpServer(QObject *parent = nullptr);

    void attachServer( MAppServer *pServer );

protected:

public:
    bool start( quint16 port );

protected:
    MAppServer *m_pServer;

private:
    QList<MAppService *> mServices;
    quint16 mPort;

signals:

public slots:

};

#endif // MAPPSERVER_H
