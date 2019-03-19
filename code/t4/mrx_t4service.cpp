#include "mrx_t4service.h"
#include "../mydebug.h"
#include "../myjson.h"
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
                         QJsonObject obj = doc.object();\
                         type var;\
                         int localRet = -1111;\
                            \
                        Q_ASSERT( NULL != m_pServer );\
                        MRX_T4Server *pLocalServer = (MRX_T4Server*)m_pServer;

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

//    mProcMap.insert( QString("device_status"), (api_type)&api_class::on_device_status_proc);
//    mProcMap.insert( QString("exception"), (api_type)&api_class::on_exception_proc);
//    mProcMap.insert( QString("pose"), (api_type)&api_class::on_pose_proc);
//    mProcMap.insert( QString("parameter"), (api_type)&api_class::on_parameter_proc);

//    mProcMap.insert( QString("dataset"), (api_type)&api_class::on_dataset_proc);
//    mProcMap.insert( QString("meta"), (api_type)&api_class::on_meta_proc);
    mProcMap.insert( QString("config"), (api_type)&api_class::on_config_proc);
}

bool MRX_T4Service::onUserEvent(QEvent *pEvent)
{
    logDbg_Thread();

    MServiceEvent *pLocalEvent = (MServiceEvent*)pEvent;
    QJsonDocument localDoc = pLocalEvent->mVar1.toJsonDocument();

    //! proc the json obj
    proc( localDoc );

    return true;
}

int MRX_T4Service::on_ack_proc(  QJsonDocument &doc )
{
    return 0;
}

int MRX_T4Service::on_step_proc(  QJsonDocument &doc )
{
    pre_def( Intfstep );

    deload_double( angle );
    deload_double( z );
    deload_bool( continous );

    //! \todo

    return 0;
}
int MRX_T4Service::on_joint_step_proc(  QJsonDocument &doc )
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
int MRX_T4Service::on_action_proc( QJsonDocument &doc )
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

int MRX_T4Service::on_indicator_proc(  QJsonDocument &doc )
{
    pre_def( Intfindicator );

    deload_bool( status );

    localRet = mrgIdentify( local_vi(), var.status );

    return localRet;
}

int MRX_T4Service::on_add_proc(  QJsonDocument &doc )
{
    pre_def( Intfadd );

    deload_string( name );

    //! object
    QJsonObject poseObj;
    _deload_json_obj( poseObj, obj, pose );

    __deload_double( poseObj, var.pose, x );
    __deload_double( poseObj, var.pose, y );
    __deload_double( poseObj, var.pose, z );
    __deload_double( poseObj, var.pose, w );
    __deload_double( poseObj, var.pose, h );

    pLocalServer->addPoint( var.name, var.pose );

    pLocalServer->saveDataSet();

    return 0;
}

#define query_( proc )  localRet = proc( localDoc );\
                        if ( localRet != 0 )\
                        { return localRet; }\
                        \
                        output( localDoc );
int MRX_T4Service::on_query_proc(  QJsonDocument &doc )
{
    pre_def( Intfquery );

    deload_string( item );

    QJsonDocument localDoc;
    if ( var.item == "link_status" )
    {
        query_( on_link_status_proc_q );
    }
    else if ( var.item == "device_status" )
    {
        query_( on_device_status_proc );
    }
    else if ( var.item == "exception" )
    {
        query_( on_exception_proc );
    }
    else if ( var.item == "pose" )
    {
        query_( on_pose_proc );
    }
    else if ( var.item == "parameter" )
    {
        query_( on_parameter_proc );
    }
    else if ( var.item == "dataset" )
    {
        query_( on_dataset_proc );
    }
    else if ( var.item == "meta" )
    {
        query_( on_meta_proc );
    }
    else if ( var.item == "config" )
    {
        query_( on_config_proc_q );
    }
    else
    { }

    return localRet;
}

//! set
int MRX_T4Service::on_link_status_proc(  QJsonDocument &doc )
{
    pre_def( Intflink_status );

    deload_string( status );

    pLocalServer->mLinkStatus = var.status;

    doc.setObject( obj );

    return 0;
}

//! query
int MRX_T4Service::on_link_status_proc_q(  QJsonDocument &doc )
{
    pre_def( Intflink_status );

    var.status = pLocalServer->mLinkStatus;

    json_obj( command );
    json_obj( status );

    doc.setObject( obj );

    return 0;
}

int MRX_T4Service::on_device_status_proc( QJsonDocument &doc )
{
    pre_def( Intfdevice_status );

    //! \todo
    var.status = "stoped";

    //! if the bg thread is running or the device is running
    Q_ASSERT( NULL != m_pExec );
    if ( m_pExec->isRunning() )
    { var.status = "running"; }
    else
    { var.status = "stoped"; }

    json_obj( command );
    json_obj( status );

    doc.setObject( obj );

    return 0;
}
//! query
int MRX_T4Service::on_exception_proc(  QJsonDocument &doc )
{
    pre_def( Intfexception );

    var.cause = "unknown";

    json_obj( command );
    json_obj( cause );

    doc.setObject( obj );

    return 0;
}

//! query
int MRX_T4Service::on_pose_proc(  QJsonDocument &doc )
{
    pre_def( Intfpose );

    json_obj( command );

    //! local pose
    QJsonObject pose;

    //! \todo
    var.pose.x = 0;
    var.pose.y = 1;
    var.pose.z = 2;
    var.pose.w = 3;
    var.pose.h = 4;

    pose.insert( "x", var.pose.x );
    pose.insert( "y", var.pose.x );
    pose.insert( "z", var.pose.x );
    pose.insert( "w", var.pose.x );
    pose.insert( "h", var.pose.x );

    obj.insert( "pose", pose );

    doc.setObject( obj );

    return 0;
}

//! query
//double currents[5];
//double idleCurrents[5];
//double slowRatios[5];
//double microSteps[5];

//bool handIos[2];
//bool distanceSensors[4];
//bool collide;
//bool tunnings[5];

#define export_array( array )   QJsonArray local##array;\
for ( int i = 0; i < sizeof_array( var.array); i++ )\
{ local##array<<var.array[i]; }\
obj.insert( #array, local##array );
int MRX_T4Service::on_parameter_proc(  QJsonDocument &doc )
{
    pre_def( Intfparameter );

    //! \todo
    var.currents[0] = 0;
    var.currents[1] = 1;
    var.currents[2] = 2;
    var.currents[3] = 3;
    var.currents[4] = 4;

    var.idleCurrents[0] = 0;
    var.idleCurrents[1] = 1;
    var.idleCurrents[2] = 2;
    var.idleCurrents[3] = 3;
    var.idleCurrents[4] = 4;

    var.slowRatios[0] = 50;
    var.slowRatios[1] = 50;
    var.slowRatios[2] = 50;
    var.slowRatios[3] = 38;
    var.slowRatios[4] = 25;

    var.microSteps[0] = 64;
    var.microSteps[1] = 64;
    var.microSteps[2] = 64;
    var.microSteps[3] = 64;
    var.microSteps[4] = 64;

    var.handIos[0] = true;
    var.handIos[1] = false;

    var.distanceSensors[0] = false;
    var.distanceSensors[1] = false;
    var.distanceSensors[2] = false;
    var.distanceSensors[3] = false;

    var.collide = true;

    var.tunnings[0] = true;
    var.tunnings[1] = true;
    var.tunnings[2] = true;
    var.tunnings[3] = true;
    var.tunnings[4] = true;

    //! output the json
    json_obj( command );

    export_array( currents );
    export_array( idleCurrents );
    export_array( slowRatios );
    export_array( microSteps );

    export_array( handIos );
    export_array( distanceSensors );
    export_array( tunnings );

    json_obj( collide );

    doc.setObject( obj );

    return 0;
}

//! query
int MRX_T4Service::on_dataset_proc( QJsonDocument &doc )
{
    pre_def( IntfdataSet );

    QJsonArray ary;
    pLocalServer->_dataDataToArray( ary );

    json_obj( command );
    obj.insert( "points", ary );

    doc.setObject( obj );

    return 0;
}

//! query
int MRX_T4Service::on_meta_proc( QJsonDocument &doc )
{
    pre_def( Intfmeta );

    Q_ASSERT( NULL != m_pServer );
    MRX_T4Server *pServer = (MRX_T4Server*)m_pServer;
    var.sn = pServer->mSn;
    var.model = pServer->mModel;
    var.alias = pServer->mAlias;
    var.has_hand = pServer->mbHasHand;

    json_obj( command );
    json_obj( sn );
    json_obj( model );
    json_obj( alias );

    json_obj( has_hand );

    //! return obj
    doc.setObject( obj );

    return 0;
}
int MRX_T4Service::on_config_proc(  QJsonDocument &doc )
{
    pre_def( Intfconfig );

    deload_double( step );
    deload_double( joint_step );
    deload_double( speed );

    //! config
    pLocalServer->mStep = var.step;
    pLocalServer->mJointStep = var.joint_step;
    pLocalServer->mSpeed = var.speed;

    //! post save
    //! \todo
    return pLocalServer->saveConfig();
}

int MRX_T4Service::on_config_proc_q(  QJsonDocument &doc )
{
    pre_def( Intfconfig );

    //! config q
    var.step = pLocalServer->mStep;
    var.joint_step = pLocalServer->mJointStep;
    var.speed = pLocalServer->mSpeed;

    //! export
    json_obj( command );
    json_obj( step );
    json_obj( joint_step );
    json_obj( speed );

    doc.setObject( obj );

    return 0;
}
