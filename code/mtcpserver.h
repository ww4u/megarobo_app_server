#ifndef MTCPSERVER_H
#define MTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QThread>

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

protected Q_SLOTS:

protected:
    MAppServer *m_pServer;

private:
    quint16 mPort;

signals:

public slots:

};

#endif // MAPPSERVER_H
