#ifndef INTFOBJ_H
#define INTFOBJ_H

#include <QtCore>

class IntfObj
{
public:
    IntfObj( const QString cmd="" );

public:
    QString command;
    QMap<QString, bool> bmMap;
};

class IntfNotify : public IntfObj
{
public:
    enum eNotifyCode
    {
        e_notify_unk,
        e_notify_socket_idle_timeout,
        e_notify_socket_idle_quit,
    };

public:
    IntfNotify( const QString cmd="notify" ) : IntfObj( cmd )
    {}

public:
    QString message;
    int code;
};

#endif // INTFOBJ_H
