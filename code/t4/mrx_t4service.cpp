#include "mrx_t4service.h"
#include "../mydebug.h"
#include "../myjson.h"
#include "t4para.h"

#include "mrx_t4server.h"
#include "MegaGateway.h"

#include "intfseries.h"
#include <math.h>
#include <float.h>

#define deg_to_rad( deg )   ( (deg)* M_PI /180)



#define get_bit( d, n )     ( ( d >> n ) & 1 )

#define api_class   MRX_T4Service
#define api_server  MRX_T4Server

/*
#define check_connect() \
Q_ASSERT ( m_pServer != NULL ); \
api_server *_pLocalServer;\
_pLocalServer = dynamic_cast<api_server*>(m_pServer); \
if ( NULL == _pLocalServer )\
{ return -1; }

#define pre_def( type )  check_connect();\
                         QJsonObject obj = doc.object();\
                         type var;\
                         int localRet = -1111;\
                            \
                        Q_ASSERT( NULL != m_pServer );\
                        api_server *pLocalServer = (api_server*)m_pServer;
*/
/*
#define api_type    MAppService::P_PROC

#define local_vi()          _pLocalServer->deviceVi()
#define device_handle()     _pLocalServer->deviceHandle()
#define robot_handle()      _pLocalServer->robotHandle()

#define wave_table          0

#define post_call( api )    Q_ASSERT( NULL != m_pWorkingThread );\
                            localRet = m_pWorkingThread->attachProc( this, \
                                                                     (MAppService::P_PROC)(&MRX_T4Service::post_##api), \
                                                                     (MAppService::P_PROC)(&MRX_T4Service::_on_preProc), \
                                                                     (MAppService::P_PROC)(&MRX_T4Service::_on_postProc), \
                                                                     QString("post_"#api), QVariant(doc) );\
                            return localRet;
*/

/*
#define query_( proc )  { QJsonDocument localDoc;\
                        localRet = proc( localDoc );\
                        if ( localRet != 0 )\
                        { return localRet; }\
                        \
                        output( localDoc ); }
*/

#define ack_status( )   query_( on_pose_proc );query_( on_device_status_proc );

#define ack_raw_status()    query_( on_pose_proc );query_( on_device_status_raw_proc );

MRX_T4Service::MRX_T4Service( qintptr ptr, QObject *parent ) : MAppService( ptr, parent )
{
    //! fill map
//    mProcMap.insert( QString("step"), (MAppService::P_PROC)&MRX_T4Service::on_step_proc);

    attachProc( QString("ack"), (api_type)&api_class::on_ack_proc );
    attachProc( QString("step"), (api_type)&api_class::on_step_proc, 500 );
    attachProc( QString("joint_step"), (api_type)&api_class::on_joint_step_proc, 500 );
    attachProc( QString("action"), (api_type)&api_class::on_action_proc );

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

}

bool MRX_T4Service::onUserEvent(QEvent *pEvent)
{
//    logDbg_Thread();

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

int MRX_T4Service::_on_preProc( QJsonDocument &doc )
{
    int localRet;

    query_(on_device_status_proc );

    return localRet;
}
int MRX_T4Service::_on_postProc( QJsonDocument &doc )
{
    int localRet;

    ack_raw_status();

    return localRet;
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

    //! \note
    //! \todo
    var.continous = false;

    //! deparse the x/y/z
    double lx, ly, lz;
    if ( var.continous )
    {
        if ( var.z == 0 )
        {
            lx = 1 * sin( deg_to_rad(var.angle) );
            ly = - 1 * cos( deg_to_rad(var.angle) );
            lz = 1 * var.z;
        }
        else
        {
            lx = 0;
            ly = 0;
            lz = 1 * var.z;
        }
    }
    //! step
    else
    {
        if ( var.z == 0 )
        {
            lx = pLocalServer->localStep() * sin( deg_to_rad(var.angle) );
            ly = - pLocalServer->localStep() * cos( deg_to_rad(var.angle) );
            lz = pLocalServer->localStep() * var.z;
        }
        else
        {
            lx = 0;
            ly = 0;
            lz = pLocalServer->localStep() * var.z;
        }
    }

    //! guess t
    double dist = distance( lx, ly, lz, 0,0,0 );
    double t = dist / pLocalServer->mMaxBodySpeed / pLocalServer->localSpeedRatio();

    //! \note no move on
    if ( var.continous )
    {
        localRet = mrgRobotMoveOn( local_vi(),
                                   robot_handle(),
                                   wave_table,
                                   lx,
                                   ly,
                                   lz,
                                   pLocalServer->mMaxBodySpeed * pLocalServer->localSpeedRatio() );
    }
    else
    {
        localRet = mrgRobotRelMove( local_vi(),
                                    robot_handle(),
                                    wave_table,
                                    lx,
                                    ly,
                                    lz,
                                    t,
                                    (1000 * t + 2000)
                                   );
    }

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

    int tmoms;
    if ( var.joint == 3 )
    {
        //! norminal angle
        if ( var.continous )
        {
            //! \note var.value = dir
            localRet = mrgRobotJointMoveOn( local_vi(),
                                 robot_handle(),
                                 3,
                                 var.value * pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio() );
            logDbg()<<pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio();
        }
        else
        {
            //! step proc
            if ( qAbs(var.value) == 1 )
            {
                tmoms = guessTmo( 3, var.value, pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio() );
                localRet = mrgMRQAdjust( local_vi(),
                                         device_handle(),
                                         3,
                                         wave_table,
                                         var.value * pLocalServer->localJStep(),
                                         pLocalServer->localJStep() / pLocalServer->mMaxJointSpeed / pLocalServer->localSpeedRatio(),
                                         tmoms );
            }
            else
            {
                //! \note align the dst angle
                float normAngle = 270 - var.value;
                normAngle = alignP360( normAngle );

                //! \note config the speed not time
                tmoms = guessTmo( 3, 360, pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio() );
                localRet = mrgSetRobotWristPose( local_vi(),
                                                      robot_handle(),
                                                      wave_table,
                                                      normAngle,
                                                      pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio(),
                                                      tmoms );
            }
        }
    }
    else if ( var.joint == 4 )
    {
        //! continous
        if ( var.continous )
        {
            //! \note var.value = dir
            localRet = mrgRobotJointMoveOn( local_vi(),
                                 robot_handle(),
                                 4,
                                 var.value * pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio() );
        }
        else
        {
            tmoms = guessTmo( 4, var.value, pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio() );
            localRet = mrgMRQAdjust( local_vi(),
                                     device_handle(),
                                     4,
                                     wave_table,
                                     var.value * pLocalServer->localJStep(),
                                     pLocalServer->localJStep() / pLocalServer->mMaxJointSpeed / pLocalServer->localSpeedRatio(),
                                     tmoms );
        }
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
        if ( m_pWorkingThread->isRunning() )
        {
            m_pWorkingThread->requestInterruption();

            //! \note force stop
            localRet = mrgSysSetEmergencyStop( local_vi(), 1 );

            localRet = mrgSysSetEmergencyStop( local_vi(), 0 );
        }
        else
        {}
    }
    else if ( var.item == "emergency_stop" )
    {
        //! \todo
        //! stop the mission thread
        Q_ASSERT( NULL != m_pWorkingThread );
        m_pWorkingThread->requestInterruption();

        //! \note force stop
        localRet = mrgSysSetEmergencyStop( local_vi(), 1 );

        localRet = mrgSysSetEmergencyStop( local_vi(), 0 );

        ack_raw_status();

        return localRet;
    }
    else if ( var.item == "stop" )

    {
        localRet = mrgRobotStop( local_vi(), robot_handle(), wave_table );

        Q_ASSERT( NULL != m_pWorkingThread );
        m_pWorkingThread->requestInterruption();

        ack_raw_status();

        return localRet;
    }
    //! 1. stop
    //! 2. post package
    else if ( var.item == "package" )
    {
        localRet = mrgRobotStop( local_vi(), robot_handle(), wave_table );

        Q_ASSERT( NULL != m_pWorkingThread );
        m_pWorkingThread->requestInterruption();
    }
    else
    {}

    {
        post_call( on_action_proc );
    }

    return localRet;
}

int MRX_T4Service::post_on_action_proc(  QJsonDocument &doc )
{
    pre_def( Intfaction );

    deload_string( item );

    if (  var.item == "home" )
    {
        //! \todo stop and home
        localRet = mrgRobotGoHome( local_vi(),
                                   robot_handle(),
                                   120000 );

    }
    else if ( var.item == "emergency_stop" )
    {
        //! \note has stoped
    }
    else if ( var.item == "stop" )
    {
        //! \todo normal stop
        //! \note has stoped
//        localRet = mrgRobotStop( local_vi(), robot_handle(), wave_table );
    }
    else if ( var.item == "package" )
    {
        localRet = mrgSetRobotFold( local_vi(),robot_handle(),
                               0,
                               18.8,
                               -57.4,
                               -103,
                               120000
                               );

        query_( on_parameter_proc );

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

//    //! object
    QJsonObject poseObj;
    logDbg();
    _deload_json_obj( poseObj, obj, pose );
    logDbg();

    __deload_double( poseObj, var.pose, x );
    __deload_double( poseObj, var.pose, y );
    __deload_double( poseObj, var.pose, z );
    __deload_double( poseObj, var.pose, w );
    __deload_double( poseObj, var.pose, h );

    pLocalServer->addPoint( var.name, var.pose );

    pLocalServer->saveDataSet();

    return 0;
}

int MRX_T4Service::on_query_proc(  QJsonDocument &doc )
{
    pre_def( Intfquery );

    deload_string( item );


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

    deload_bool( status );

    pLocalServer->mbLink = var.status;

    doc.setObject( obj );

    return 0;
}

//! query
int MRX_T4Service::on_link_status_proc_q(  QJsonDocument &doc )
{
    pre_def( Intflink_status );

    var.status = pLocalServer->isLinked();

    json_obj( command );
    json_obj( status );

    doc.setObject( obj );

    return 0;
}

int MRX_T4Service::on_device_status_proc( QJsonDocument &doc )
{
    pre_def( Intfdevice_status );

    //! if the bg thread is running or the device is running
    if ( m_pServer->status() == MAppServer::state_working )
    { var.status = "running"; logDbg(); }
    else
    {
        return on_device_status_raw_proc( doc );
    }

    json_obj( command );
    json_obj( status );

    doc.setObject( obj );

    return 0;
}

//! \note only the device status
int MRX_T4Service::on_device_status_raw_proc( QJsonDocument &doc )
{
    pre_def( Intfdevice_status );

    var.status = "exception_stopd";

    char states[128];
    localRet = mrgGetRobotStates( local_vi(),
                      robot_handle(),
                      wave_table,
                      states );
    if ( localRet != 0 )
    {}
    else if ( QString( states ).toLower() == "idle" )
    {
        var.status = "stoped";
    }
    else
    {
        var.status = "running";
    }

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
        logWarning()<<"read pos fail";
        return localRet;
    }

    float fHAngle, fw;
    fHAngle = 0;
//    localRet = mrgGetRobotToolPosition( local_vi(), robot_handle(),
//                                        &fHAngle );
    if ( localRet != 0 )
    {
        logWarning()<<"read tool fail";
        return localRet;
    }

    localRet = mrgGetRobotWristPose( local_vi(), robot_handle(),
                          &fw );
    if ( localRet != 0 )
    {
        logWarning()<<"read wrist fail";
        return localRet;
    }

    //! align fw
    fw = 270 - fw;
    fw = alignP360( fw );

    var.pose.x = fx;
    var.pose.y = fy;
    var.pose.z = fz;
    var.pose.w = fw;
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
        localRet = mrgMRQDriverTuningState_Query( local_vi(),
                                       device_handle(),
                                       i,
                                       &iVal );
        if ( localRet != 0 )
        { return localRet; }

        var.tunning[ i ] = iVal > 0;

        //!
        int a, b;
        localRet = mrgMRQMotorGearRatio_Query( local_vi(), device_handle(),
                                    i,
                                    &a, &b );
        if ( localRet != 0 )
        { return localRet; }

        var.slow_ratio[i] = a;
    }

    //! dio
    unsigned short ioState;
    localRet = mrgGetMRQDioState( local_vi(),
                       device_handle(),
                       &ioState );
    if ( localRet != 0 )
    { return localRet; }

    var.hand_io[0] = get_bit( ioState, 3 );
    var.hand_io[1] = get_bit( ioState, 2 );

    var.mechanical_io = false;

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

    //! \note soft limit
    var.collide = true;
    for ( int i = 0; i < 4; i++ )
    {
        localRet = mrgMRQAbsEncoderAlarmState_Query( local_vi(), device_handle(),
                                                     i, &iVal );
        if ( localRet != 0 )
        { return localRet; }

        var.collide = var.collide && ( iVal > 0 );
    }

    var.max_joint_speed = _pLocalServer->mMaxJointSpeed;
    var.max_tcp_speed = _pLocalServer->mMaxBodySpeed;

    //! x,y,z,w,h
    {
        //! x,y,z now
        float fx, fy, fz;
        localRet = mrgGetRobotCurrentPosition( local_vi(),
                                          robot_handle(),

                                          &fx, &fy, &fz );
        if ( localRet != 0 )
        { return localRet; }

        float fHAngle;
        localRet = mrgGetRobotToolPosition( local_vi(), robot_handle(),
                                            &fHAngle );
        if ( localRet != 0 )
        { return localRet; }


        float fw;
        localRet = mrgGetRobotWristPose( local_vi(), robot_handle(),
                                         &fw );
        if ( localRet != 0 )
        { return localRet; }

        //! align fw
        fw = 270 - fw;
        fw = alignP360( fw );

        var.x = fx;
        var.y = fy;
        var.z = fz;

        var.w = fw;
        var.h = fHAngle;
    }

    //! distance sensor
    for ( int i = 0; i < 4; i++ )
    {
        localRet = mrgMRQDistanceAlarmState_Query(  local_vi(), device_handle(),
                                                    i, &iVal );
        if ( localRet != 0 )
        { return localRet; }

        var.distance_sensors[i] = iVal > 0;
    }

    //! output the json
    json_obj( command );

    export_array( currents );
    export_array( idle_currents );
    export_array( slow_ratio );
    export_array( micro_steps );

    //! \todo by the hand type
    export_array( hand_io );
    json_obj( mechanical_io );

    export_array( distance_sensors );
    export_array( tunning );

    json_obj( collide );

    json_obj( max_joint_speed );
    json_obj( max_tcp_speed );

    json_obj( x );
    json_obj( y );
    json_obj( z );
    json_obj( w );
    json_obj( h );

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
    var.link = pServer->isLinked();

    json_obj( command );
    json_obj( sn );
    json_obj( model );
    json_obj( alias );

    json_obj( has_hand );

    json_obj( link );

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
    pLocalServer->setStep( var.step );
    pLocalServer->setJStep( var.joint_step );
    pLocalServer->setSpeed( var.speed );

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

int MRX_T4Service::guessTmo( int joint, float dist, float speed )
{
    Q_ASSERT ( qAbs(speed) > FLT_EPSILON );

    float t = qAbs(dist/speed);

    if ( t < FLT_EPSILON )
    { return 1000; }

    return 60 * t *1000;
}

float MRX_T4Service::alignP360( float degree )
{
    while( degree > 360 )
    { degree -= 360; }
    while( degree < 0 )
    { degree += 360; }

    return degree;
}
float MRX_T4Service::alignN360( float degree )
{
    while( degree > 0 )
    { degree -= 360; }
    while( degree < -360 )
    { degree += 360; }

    return degree;
}
