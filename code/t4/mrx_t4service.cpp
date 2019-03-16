#include "mrx_t4service.h"
#include "../mydebug.h"
#include "t4para.h"

#include "mrx_t4server.h"
#include "MegaGateway.h"

#include "intfseries.h"

#define check_connect() \
Q_ASSERT ( m_pServer != NULL ); \
MRX_T4Server *_pLocalServer;\
_pLocalServer = dynamic_cast<MRX_T4Server*>(m_pServer); \
if ( NULL == _pLocalServer )\
{ return -1; }


#define pre_def( type )  check_connect();\
                         type var;\
                         int localRet = -1111;

#define api_type    MAppService::P_PROC
#define api_class   MRX_T4Service

#define local_vi()          _pLocalServer->deviceVi()
#define device_handle()     _pLocalServer->deviceHandle()
#define robot_handle()      _pLocalServer->robotHandle()

#define wave_table          0

MRX_T4Service::MRX_T4Service( qintptr ptr, QObject *parent ) : MAppService( ptr, parent )
{   
    //! fill map
//    mProcMap.insert( QString("step"), (MAppService::P_PROC)&MRX_T4Service::on_step_proc);

    mProcMap.insert( QString("ack"), (api_type)&api_class::on_ack_proc);
    mProcMap.insert( QString("step"), (api_type)&api_class::on_step_proc);
    mProcMap.insert( QString("joint_step"), (api_type)&api_class::on_joint_step_proc);
    mProcMap.insert( QString("action"), (api_type)&api_class::on_action_proc);

    mProcMap.insert( QString("indicator"), (api_type)&api_class::on_indicator_proc);
    mProcMap.insert( QString("add"), (api_type)&api_class::on_add_proc);
    mProcMap.insert( QString("query"), (api_type)&api_class::on_query_proc);
    mProcMap.insert( QString("link_status"), (api_type)&api_class::on_link_status_proc);

    mProcMap.insert( QString("device_status"), (api_type)&api_class::on_device_status_proc);
    mProcMap.insert( QString("exception"), (api_type)&api_class::on_exception_proc);
    mProcMap.insert( QString("pose"), (api_type)&api_class::on_pose_proc);
    mProcMap.insert( QString("parameter"), (api_type)&api_class::on_parameter_proc);

    mProcMap.insert( QString("dataset"), (api_type)&api_class::on_dataset_proc);
    mProcMap.insert( QString("meta"), (api_type)&api_class::on_meta_proc);
    mProcMap.insert( QString("config"), (api_type)&api_class::on_config_proc);
}

int MRX_T4Service::on_ack_proc(  QJsonObject &obj )
{
    return 0;
}

#define deload_double( item )   if ( obj.contains( #item ) )\
                                { var.item = obj.value( #item ).toDouble(); logDbg()<<var.item; } \
                                else \
                                { return -1; }
#define deload_string( item )   if ( obj.contains( #item ) )\
                                { var.item = obj.value( #item ).toString(); logDbg()<<var.item; } \
                                else \
                                { return -1; }
#define deload_int( item )   if ( obj.contains( #item ) )\
                                { var.item = obj.value( #item ).toInt(); logDbg()<<var.item; } \
                                else \
                                { return -1; }
#define deload_bool( item )   if ( obj.contains( #item ) )\
                                { var.item = obj.value( #item ).toBool(); logDbg()<<var.item; } \
                                else \
                                { return -1; }

int MRX_T4Service::on_step_proc(  QJsonObject &obj )
{
    pre_def( Intfstep );

    deload_double( angle );
    deload_double( z );
    deload_bool( continous );

    return 0;
}
int MRX_T4Service::on_joint_step_proc(  QJsonObject &obj )
{
    pre_def( Intfjoint_step );

    deload_int( direction );
    deload_bool( continous );
    deload_int( joint );

    if ( var.joint == 3 )
    {
        localRet = mrgMRQAdjust( local_vi(),
                                 device_handle(),
                                 3,
                                 wave_table,
                                 var.direction * 10,
                                 1,
                                 1000 );
    }
    else if ( var.joint == 4 )
    {
        localRet = mrgMRQAdjust( local_vi(),
                                 device_handle(),
                                 4,
                                 wave_table,
                                 10,
                                 1,
                                 1000 );
    }
    else
    { return -1; }

    return localRet;
}
int MRX_T4Service::on_action_proc(  QJsonObject &obj )
{
    pre_def( Intfaction );

    deload_string( item );

    if (  var.item == "home" )
    {
        localRet = mrgRobotGoHome( local_vi(),
                              robot_handle(), 5000 );
    }
    else if ( var.item == "emergency_stop" )
    {
        localRet = mrgSysSetEmergencyStop( local_vi(), 1 );
    }
    else if ( var.item == "stop" )
    {
        //! \todo stop all
        localRet = mrgSysSetEmergencyStop( local_vi(), 1 );
//        ret = mrgRobotToolStop( local_vi(),
//                                robot_handle() );
//        ret = mrgRobotGoHomeStop( )
    }
    else if ( var.item == "package" )
    {

        localRet = mrgGetRobotFold( local_vi(),robot_handle(),
                               wave_table,
                               0,
                               -18.8,
                               -57.4,
                               -103
                               );

    }
    else
    {}

    return localRet;
}

int MRX_T4Service::on_indicator_proc(  QJsonObject &obj )
{
    pre_def( Intfindicator );

    deload_bool( status );

    localRet = mrgIdentify( local_vi(), var.status );

    return localRet;
}


int MRX_T4Service::on_add_proc(  QJsonObject &obj )
{
    pre_def( Intfadd );

    deload_string( name );

    //! \todo the pose

    return 0;
}
int MRX_T4Service::on_query_proc(  QJsonObject &obj )
{
    pre_def( Intfquery );

    deload_string( item );

    QJsonObject localObj;
    if ( var.item == "link_status" )
    {}
    else if ( var.item == "device_status" )
    {
        localRet = on_device_status_proc( localObj );
        if ( localRet != 0 )
        { return localRet; }

        output( localObj );
    }
    else if ( var.item == "exception" )
    {}
    else if ( var.item == "pose" )
    {}
    else if ( var.item == "parameter" )
    {}
    else if ( var.item == "dataset" )
    {}
    else if ( var.item == "meta" )
    {}
    else if ( var.item == "config" )
    {}
    else
    { }

    return localRet;
}

//! set
int MRX_T4Service::on_link_status_proc(  QJsonObject &obj )
{
    pre_def( Intflink_status );

    deload_string( status );

    return 0;
}

//! query
int MRX_T4Service::on_device_status_proc(  QJsonObject &obj )
{
    pre_def( Intfdevice_status );


    var.status = "stoped";

    obj.insert( "command", var.command );
    obj.insert( "status", var.status );

    return 0;
}
//! query
int MRX_T4Service::on_exception_proc(  QJsonObject &obj )
{
    pre_def( Intfexception );

    deload_string( cause );

    return 0;
}

//! query
int MRX_T4Service::on_pose_proc(  QJsonObject &obj )
{
    pre_def( Intfpose );

//    deload_string( status );

    return 0;
}

//! query
int MRX_T4Service::on_parameter_proc(  QJsonObject &obj )
{
    pre_def( Intfparameter );



    return 0;
}

//! query
int MRX_T4Service::on_dataset_proc(  QJsonObject &obj )
{
    pre_def( IntfdataSet );

//    deload_string( status );

    return 0;
}

//! query
int MRX_T4Service::on_meta_proc(  QJsonObject &obj )
{
    pre_def( Intfmeta );

//    deload_string( status );

    return 0;
}
int MRX_T4Service::on_config_proc(  QJsonObject &obj )
{
    pre_def( Intfconfig );

    deload_double( step );
    deload_double( joint_step );
    deload_double( speed );

    return 0;
}
