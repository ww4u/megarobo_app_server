#include "mrx_t4service.h"
#include "../mydebug.h"
#include "../myjson.h"
#include "t4para.h"

#include "mrx_t4server.h"
#include "MegaGateway.h"

#include "intfseries.h"
#include <math.h>

#define deg_to_rad( deg )   ( (deg)* M_PI /180)

#define distance( x, y, z, x1, y1, z1 ) ( sqrt( pow(  x - x1, 2) + \
                   pow(  y - y1, 2) + \
                   pow(  z - z1, 2) \
                   ) )

#define get_bit( d, n )     ( ( d >> n ) & 1 )

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

#define post_call( api )    Q_ASSERT( NULL != m_pWorkingThread );\
                            localRet = m_pWorkingThread->attachProc( this, (MAppService::P_PROC)post_##api, QString("post_"#api), QVariant(doc) );\
                            return localRet;

MRX_T4Service::MRX_T4Service( qintptr ptr, QObject *parent ) : MAppService( ptr, parent )
{   
    //! fill map
//    mProcMap.insert( QString("step"), (MAppService::P_PROC)&MRX_T4Service::on_step_proc);

    attachProc( QString("ack"), (api_type)&api_class::on_ack_proc);
    attachProc( QString("step"), (api_type)&api_class::on_step_proc, 1000 );
    attachProc( QString("joint_step"), (api_type)&api_class::on_joint_step_proc, 1000 );
    attachProc( QString("action"), (api_type)&api_class::on_action_proc);

    attachProc( QString("indicator"), (api_type)&api_class::on_indicator_proc);
    attachProc( QString("add"), (api_type)&api_class::on_add_proc);
    attachProc( QString("query"), (api_type)&api_class::on_query_proc);
    attachProc( QString("link_status"), (api_type)&api_class::on_link_status_proc);

//    mProcMap.insert( QString("device_status"), (api_type)&api_class::on_device_status_proc);
//    mProcMap.insert( QString("exception"), (api_type)&api_class::on_exception_proc);
//    mProcMap.insert( QString("pose"), (api_type)&api_class::on_pose_proc);
//    mProcMap.insert( QString("parameter"), (api_type)&api_class::on_parameter_proc);

//    mProcMap.insert( QString("dataset"), (api_type)&api_class::on_dataset_proc);
//    mProcMap.insert( QString("meta"), (api_type)&api_class::on_meta_proc);
    attachProc( QString("config"), (api_type)&api_class::on_config_proc);

    m_pWorkingThread = new WorkingThread();
    Q_ASSERT( NULL != m_pWorkingThread );
}

MRX_T4Service::~MRX_T4Service()
{
    if ( NULL != m_pWorkingThread )
    {
        m_pServer->disconnectWorking( m_pWorkingThread );

        m_pWorkingThread->requestInterruption();
        m_pWorkingThread->wait();
        delete m_pWorkingThread;
    }
}

bool MRX_T4Service::onUserEvent(QEvent *pEvent)
{
    logDbg_Thread();

    MServiceEvent *pLocalEvent = (MServiceEvent*)pEvent;
    QJsonDocument localDoc = pLocalEvent->mVar1.toJsonDocument();

    //! proc the json obj
    proc( localDoc, pLocalEvent->mTs );

    return true;
}

void MRX_T4Service::attachServer( MAppServer *pServer )
{
    //! attach
    MAppService::attachServer( pServer );

    //! link the working
    pServer->connectWorking( m_pWorkingThread );
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

    post_call( on_step_proc );
}

int MRX_T4Service::post_on_step_proc(  QJsonDocument &doc )
{
    pre_def( Intfstep );

    deload_double( angle );
    deload_double( z );
    deload_bool( continous );

    //! deparse the x/y/z
    double lx, ly, lz;
    if ( var.z == 0 )
    {
        lx = pLocalServer->mStep * sin( deg_to_rad(var.angle) );
        ly = - pLocalServer->mStep * cos( deg_to_rad(var.angle) );
        lz = pLocalServer->mStep * var.z;
    }
    else
    {
        lx = 0;
        ly = 0;
        lz = pLocalServer->mStep * var.z;logDbg()<<lz<<pLocalServer->mStep;
    }

    //! guess t
    double dist = distance( lx, ly, lz, 0,0,0 );
    double t = dist / pLocalServer->mMaxBodySpeed / pLocalServer->mSpeed;

    localRet = mrgRobotRelMove( local_vi(),
                                robot_handle(),
                                wave_table,
                                lx,
                                ly,
                                lz,
                                t,
                                120000
                               );

    return localRet;
}

int MRX_T4Service::on_joint_step_proc(  QJsonDocument &doc )
{
    pre_def( Intfjoint_step );

    deload_double( value );
    deload_bool( continous );
    deload_int( joint );

    post_call( on_joint_step_proc );
}

int MRX_T4Service::post_on_joint_step_proc(  QJsonDocument &doc )
{
    pre_def( Intfjoint_step );

    deload_double( value );
    deload_bool( continous );
    deload_int( joint );

    if ( var.joint == 3 )
    {
        //! abs value
        //! \todo

        //! norminal angle
        localRet = mrgMRQAdjust( local_vi(),
                                 device_handle(),
                                 3,
                                 wave_table,
                                 var.value,
                                 10,
                                 120000 );
    }
    else if ( var.joint == 4 )
    {
        localRet = mrgMRQAdjust( local_vi(),
                                 device_handle(),
                                 4,
                                 wave_table,
                                 var.value * pLocalServer->mJointStep,
                                 pLocalServer->mJointStep / pLocalServer->mMaxJointSpeed / pLocalServer->mSpeed,
                                 120000 );
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
        //! \todo
        //! stop the mission thread
        Q_ASSERT( NULL != m_pWorkingThread );
        m_pWorkingThread->requestInterruption();

        //! \note force stop
        localRet = mrgSysSetEmergencyStop( local_vi(), 1 );

        post_call( on_action_proc );
    }
    else if ( var.item == "emergency_stop" )
    {
        //! \todo
        //! stop the mission thread
        Q_ASSERT( NULL != m_pWorkingThread );
        m_pWorkingThread->requestInterruption();

        //! \note force stop
        localRet = mrgSysSetEmergencyStop( local_vi(), 1 );
    }
    else
    {
        post_call( on_action_proc );
    }

}

int MRX_T4Service::post_on_action_proc(  QJsonDocument &doc )
{
    pre_def( Intfaction );

    deload_string( item );

    if (  var.item == "home" )
    {
        //! \todo stop and home
        localRet = mrgRobotGoHome( local_vi(),
                                   robot_handle(), 120000 );
    }
    else if ( var.item == "emergency_stop" )
    {
        //! \todo force stop
        localRet = mrgSysSetEmergencyStop( local_vi(), 1 );
    }
    else if ( var.item == "stop" )
    {
        //! \todo normal stop
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
    else if ( var.item == "test" )
    {
        logDbg()<<doc;
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

    //! if the bg thread is running or the device is running
    Q_ASSERT( NULL != m_pExec );
    if ( m_pServer->status() == MAppServer::state_working )
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

    //! x,y,z now
    float fx, fy, fz;
    localRet = mrgGetRobotCurrentPosition( local_vi(),
                                      robot_handle(),

                                      &fx, &fy, &fz );
    if ( localRet != 0 )
    {
        fx = 0;
        fy = 0;
        fz = 0;
    }

    float fHAngle;
    localRet = mrgGetRobotToolPosition( local_vi(), robot_handle(),
                                        &fHAngle );

    var.pose.x = fx;
    var.pose.y = fy;
    var.pose.z = fz;
    var.pose.w = 0;             //! \todo
    var.pose.h = fHAngle;

    pose.insert( "x", var.pose.x );
    pose.insert( "y", var.pose.y );
    pose.insert( "z", var.pose.z );
    pose.insert( "w", var.pose.w );
    pose.insert( "h", var.pose.h );

    obj.insert( "pose", pose );

    doc.setObject( obj );

    return localRet;
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

    int iVal;
    for ( int i = 0; i < 5; i++ )
    {
        //! current
        localRet = mrgMRQDriverCurrent_Query( local_vi(),
                                         device_handle(),
                                         i,
                                         var.currents + i );
        if ( localRet != 0 )
        { return localRet; }

        //! idle current
        localRet = mrgMRQDriverIdleCurrent_Query( local_vi(),
                                                  device_handle(),
                                             i,
                                             var.idle_currents + i );
        if ( localRet != 0 )
        { return localRet; }

        //! step
        localRet = mrgMRQDriverMicroStep_Query( local_vi(),
                                          device_handle(),
                                          i,
                                          var.micro_steps + i );

        //! tunning
        mrgMRQDriverTuningState_Query( local_vi(),
                                       device_handle(),
                                       i,
                                       &iVal );
        var.tunning[ i ] = iVal > 0;

        //!
        int a, b;
        mrgMRQMotorGearRatio_Query( local_vi(), device_handle(),
                                    i,
                                    &a, &b );
        var.slow_ratio[i] = a;
    }

    //! dio
    unsigned short ioState;
    mrgGetMRQDioState( local_vi(),
                       device_handle(),
                       &ioState );
    var.hand_io[0] = get_bit( ioState, 3 );
    var.hand_io[1] = get_bit( ioState, 2 );

//    //! \todo
//    var.currents[0] = 0;
//    var.currents[1] = 1;
//    var.currents[2] = 2;
//    var.currents[3] = 3;
//    var.currents[4] = 4;

//    var.idle_currents[0] = 0;
//    var.idle_currents[1] = 1;
//    var.idle_currents[2] = 2;
//    var.idle_currents[3] = 3;
//    var.idle_currents[4] = 4;

//    var.slow_ratio[0] = 50;
//    var.slow_ratio[1] = 50;
//    var.slow_ratio[2] = 50;
//    var.slow_ratio[3] = 38;
//    var.slow_ratio[4] = 25;

//    var.micro_steps[0] = 64;
//    var.micro_steps[1] = 64;
//    var.micro_steps[2] = 64;
//    var.micro_steps[3] = 64;
//    var.micro_steps[4] = 64;

//    var.hand_io[0] = true;
//    var.hand_io[1] = false;

//    var.distance_sensors[0] = false;
//    var.distance_sensors[1] = false;
//    var.distance_sensors[2] = false;
//    var.distance_sensors[3] = false;

    var.collide = true;

//    var.tunning[0] = true;
//    var.tunning[1] = true;
//    var.tunning[2] = true;
//    var.tunning[3] = true;
//    var.tunning[4] = true;

    //! output the json
    json_obj( command );

    export_array( currents );
    export_array( idle_currents );
    export_array( slow_ratio );
    export_array( micro_steps );

    export_array( hand_io );
    export_array( distance_sensors );
    export_array( tunning );

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
