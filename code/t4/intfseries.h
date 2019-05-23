#ifndef INTFSERIES_H
#define INTFSERIES_H

#include "../intfobj.h"

//! base obj
class ObjPose
{
public:
    double x,y,z,w,h;

};
class ObjPoint
{
public:
    QString name;
    ObjPose pose;
};

//! intfs
class Intfack : public IntfObj
{
public:
    Intfack( const QString cmd="ack" ) : IntfObj( cmd )
    {}

public:
    QString item;
    QString message;
    QString source;
    int code;
};

class Intfstep : public IntfObj
{
public:
    Intfstep( const QString cmd="step" ) : IntfObj( cmd )
    {}
public:
    double angle;
    double z;
    bool continous;
};

class Intfjoint_step : public IntfObj
{
public:
    Intfjoint_step( const QString cmd="joint_step" ) : IntfObj( cmd )
    {}
public:
    double value;
    int joint;
    bool continous;
};

class Intfaction : public IntfObj
{
public:
    Intfaction( const QString cmd="action" ) : IntfObj( cmd )
    {}
public:
    QString item;
};

class Intfindicator : public IntfObj
{
public:
    Intfindicator( const QString cmd="indicator" ) : IntfObj( cmd )
    {}
public:
    bool status;
};

class Intfadd : public IntfObj
{
public:
    Intfadd( const QString cmd="add" ) : IntfObj( cmd )
    {}
public:
    QString name;
    ObjPose pose;
};

class Intfquery : public IntfObj
{
public:
    Intfquery( const QString cmd="query" ) : IntfObj( cmd )
    {}
public:
    QString item;
};

class Intflink_status : public IntfObj
{
public:
    Intflink_status( const QString cmd="link_status" ) : IntfObj( cmd )
    {}
public:
    bool status;
};

class Intfdevice_status : public IntfObj
{
public:
    Intfdevice_status( const QString cmd="device_status" ) : IntfObj( cmd )
    {}
public:
    QString status;
};

class Intfexception : public IntfObj
{
public:
    Intfexception( const QString cmd="exception" ) : IntfObj( cmd )
    {}
public:
    QString cause;
};

class Intfpose : public IntfObj
{
public:
    Intfpose( const QString cmd="pose" ) : IntfObj( cmd )
    {}
public:
    ObjPose pose;
};

class Intfparameter : public IntfObj
{
public:
    Intfparameter( const QString cmd="parameter" ) : IntfObj( cmd )
    {}
public:
    float currents[5];
    float idle_currents[5];
    float slow_ratio[5];
    int micro_steps[5];

    bool hand_io[2];
    bool mechanical_io;
    bool distance_sensors[4];
    bool collide;
    bool tunning[5];

    float max_tcp_speed, max_joint_speed;

    //! obj pos
    float x,y,z,w,h;
};

class IntfdataSet : public IntfObj
{
public:
    IntfdataSet( const QString cmd="dataset" ) : IntfObj( cmd )
    {}
public:
    ObjPoint mPoints;
};

class Intfmeta : public IntfObj
{
public:
    Intfmeta( const QString cmd="meta" ) : IntfObj( cmd )
    {}
public:
    QString sn, alias, model;
    bool has_hand;
    bool link;
};

class Intfconfig : public IntfObj
{
public:
    Intfconfig( const QString cmd="config" ) : IntfObj( cmd )
    {}
public:
    int timeout;
    double step;
    double joint_step;
    double speed;
    double max_joint_speed;
    double max_body_speed;
};

//! operators

//! motion operators
class IntfMotion : public IntfObj
{
public:
    IntfMotion( const QString cmd ) : IntfObj( cmd )
    {}
public:
    double a, v, t;

};

class IntfMovej : public IntfMotion
{
public:
    IntfMovej( const QString cmd="movej" ) : IntfMotion( cmd )
    {}
public:
    double j1, j2, j3, j4, j5;
};

class IntfMove : public IntfMotion
{
public:
    IntfMove( const QString cmd="move" ) : IntfMotion( cmd )
    {}
public:
    double x,y,z;
};

class IntfMovel : public IntfMove
{
public:
    IntfMovel( const QString cmd="movel" ) : IntfMove( cmd )
    {}
public:
    double step;
};

class IntfSleep : public IntfObj
{
public:
    IntfSleep( const QString cmd="sleep" ) : IntfObj( cmd )
    {}
public:
    double s;
};

class IntfWaituntil : public IntfObj
{
public:
    IntfWaituntil( const QString cmd="waituntil" ) : IntfObj( cmd )
    {}

public:
    //! some DIs
};

class IntfSyncdo : public IntfObj
{
public:
    IntfSyncdo( const QString cmd="syncdo" ) : IntfObj( cmd )
    {}

public:
    QByteArray ports, outs;
};

class IntfDIOs : public IntfObj
{
public:
    IntfDIOs( const QString cmd = "io" ) : IntfObj( cmd )
    {}
public:
    QList<int> value;
};

class IntfGetio : public IntfObj
{
public:
    IntfGetio( const QString cmd="getio" ) : IntfObj( cmd )
    {}

public:
    QString type;
    QList<int> ports;
};

class IntfSetio : public IntfObj
{
public:
    IntfSetio( const QString cmd="setio" ) : IntfObj( cmd )
    {}

public:
    QString type;
    QList<int> ports;
    int value;
};

class IntfExecute: public IntfObj
{
public:
    IntfExecute( const QString cmd="execute") : IntfObj( cmd )
    {}

public:
    QString script;
};


#endif // INTFSERIES_H
