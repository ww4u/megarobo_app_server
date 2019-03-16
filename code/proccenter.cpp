#include "proccenter.h"
#include <QThread>
#include "mydebug.h"

ProcCenter::ProcCenter( QTcpSocket *pSocket, QObject *parent) : QObject(parent)
{
    m_pSocket = pSocket;
}


void ProcCenter::slot_data_in()
{
    QByteArray ary = m_pSocket->readAll();

    logDbg()<<QThread::currentThreadId();
    logDbg()<<ary;
}


