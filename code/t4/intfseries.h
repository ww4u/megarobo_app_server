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
    QString status;
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
    bool distance_sensors[4];
    bool collide;
    bool tunning[5];
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
};

class Intfconfig : public IntfObj
{
public:
    Intfconfig( const QString cmd="config" ) : IntfObj( cmd )
    {}
public:
    double step;
    double joint_step;
    double speed;
    double max_joint_speed;
    double max_body_speed;
};

#endif // INTFSERIES_H
