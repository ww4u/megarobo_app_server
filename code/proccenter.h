#ifndef PROCCENTER_H
#define PROCCENTER_H

#include <QObject>
#include <QTcpSocket>

class ProcCenter : public QObject
{
    Q_OBJECT
public:
    explicit ProcCenter( QTcpSocket *pSocket, QObject *parent = nullptr);

private:
    QTcpSocket *m_pSocket;

signals:

public slots:
    void slot_data_in();
};

#endif // PROCCENTER_H
