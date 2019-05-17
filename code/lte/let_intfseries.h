#ifndef LET_INTFSERIES_H
#define LET_INTFSERIES_H

#include "../intfobj.h"


class ObjPose
{
public:
    double x,y,z,vx,vy,vz;
};

class IntfConfig : public IntfObj
{
public:
    IntfConfig( const QString cmd="config") : IntfObj( cmd )
    {}
public:
    QString item;

    double x, y, z;
    int dirx, diry;
    double w, h, d;
    double dw, dh,dd;
    double rv;
    double gap, gapspeed, zhomespeed;
};

class IntfPose : public IntfObj, public ObjPose
{
public:
    IntfPose( const QString cmd="pose") : IntfObj( cmd )
    {}
};

class IntfStatus : public IntfObj
{
public:
    IntfStatus( const QString cmd="status" ): IntfObj( cmd )
    {}
public:
    QString status;
};

class IntfRequest : public IntfObj
{
public:
    IntfRequest( const QString cmd="request" ) : IntfObj( cmd )
    {}

public:
    QString item;
    double velocity;
    double x, y, z;
    int n;
};

class IntfQuery: public IntfObj
{
public:
    IntfQuery( const QString cmd="query" ) : IntfObj( cmd )
    {}

public:
    QString item;
};

#endif // LET_INTFSERIES_H
