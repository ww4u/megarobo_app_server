#ifndef MAPPSERVER_H
#define MAPPSERVER_H

#include <QObject>
#include <QTcpServer>

#include "mappservice.h"
#include "serverintf.h"

class MAppServer : public QTcpServer, public ServerIntf
{
    Q_OBJECT
public:
    explicit MAppServer(QObject *parent = nullptr);

protected:
//    virtual void incomingConnection(qintptr socketDescriptor);

public:
    bool start();

private:
    QList<MAppService *> mServices;

signals:

public slots:

};

#endif // MAPPSERVER_H
