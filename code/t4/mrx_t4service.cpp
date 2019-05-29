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

MRX_T4Service::MRX_T4Service( qintptr ptr, quint16 port, QObject *parent ) : MAppService( ptr, port, parent )
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

    attachProc( QString("config"), (api_type)&api_class::on_config_proc);

    attachProc( QString("alignj"), (api_type)&api_class::on_alignj_proc );
    attachProc( QString("movej"), (api_type)&api_class::on_movej_proc );
    attachProc( QString("move"), (api_type)&api_class::on_move_proc );
    attachProc( QString("movel"), (api_type)&api_class::on_movel_proc );

    attachProc( QString("setio"), (api_type)&api_class::on_setio );
    attachProc( QString("getio"), (api_type)&api_class::on_getio );

    attachProc( QString("execute"), (api_type)&api_class::on_execute );

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

//    QThread::msleep( 50 );
    query_(on_device_status_proc );

    return localRet;
}
int MRX_T4Service::_on_postProc( QJsonDocument &doc )
{
    int localRet;

    ack_raw_status();

    //! interrupt status

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

        //! \errant wait for end
        localRet = mrgRobotWaitEnd( local_vi(), robot_handle(), wave_table, 100 );

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
    else if ( var.item == "stopmove" )
    {
        //! \todo
    }
    else if ( var.item == "startmove" )
    {
        //! \todo
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

//        ack_raw_status();
    }
    else if ( var.item == "stop" )
    {
        //! \todo normal stop
        //! \note has stoped
//        localRet = mrgRobotStop( local_vi(), robot_handle(), wave_table );

//        ack_raw_status();
    }
    else if ( var.item == "package" )
    {
        localRet = mrgSetRobotFold( local_vi(),
                                    robot_handle(),
                               0,
                               18.8,
                               -57.4,
                               -103,
                               120000
                               );

        query_( on_parameter_proc );

    }
    else if ( var.item == "stopmove" )
    {
        //! \todo
    }
    else if ( var.item == "startmove" )
    {
        //! \todo
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
    else if ( var.item == "controller_status" )
    {
        query_( on_controller_status_proc );
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
    else if ( var.item == "cpose" )
    {
        query_( on_cpose_proc );
    }
    else if ( var.item == "cjoint" )
    {
        query_( on_cjoint_proc );
    }
    else if ( var.item == "ctorque" )
    {
        //! \todo
    }
    else if ( var.item == "ccurrent" )
    {
        //! \todo
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

    Q_ASSERT( NULL != m_pServer );
    //! if the bg thread is running or the device is running
    if ( m_pServer->status() == MAppServer::state_working )
    { var.status = "running"; logDbg(); }
    else
    {logDbg();
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

    localRet = rawStatus( var.status );

    json_obj( command );
    json_obj( status );

    doc.setObject( obj );

    return localRet;
}

int MRX_T4Service::on_controller_status_proc( QJsonDocument &doc )
{
    pre_def( Intfdevice_status );

    //! \note contorller status
    var.command = "controller_status";

    //! first check the local app engine
    if ( _pLocalServer->controllerStatus() == MAppServer::state_working )
    { var.status = "running"; logDbg(); }
    //! if the bg thread is running or the device is running
    else if ( _pLocalServer->status() == MAppServer::state_working )
    { var.status = "running"; logDbg(); }
    else
    {
        rawStatus( var.status );
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

//! \note only pose
int MRX_T4Service::on_cpose_proc( QJsonDocument &doc )
{
    pre_def( Intfpose );

    //! local pose
//    QJsonObject pose;

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

    obj.insert( "x", fx );
    obj.insert( "y", fy );
    obj.insert( "z", fz );

    doc.setObject( obj );

    return localRet;
}

int MRX_T4Service::on_cjoint_proc( QJsonDocument &doc )
{
    pre_def( Intfpose );

    //! local pose
//    QJsonObject pose;

    float angles[16] = {0};

//    for ( int i = 0; i < 4; i++ )
    {
        localRet = mrgGetRobotJointAngle( local_vi(),
                                      robot_handle(),
                                      -1,
                                      angles );
        if ( localRet != 5 )
        {
            logWarning()<<"read joint fail"<<localRet;
            return localRet;
        }
    }

    obj.insert( "j1", angles[0] );
    obj.insert( "j2", angles[1] );
    obj.insert( "j3", angles[2] );
    obj.insert( "j4", angles[3] );

    //! \todo the hand angle
    obj.insert( "j5", angles[5] );

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

#define export_list( array ) QJsonArray local##array;\
for ( int i = 0; i < var.array.size(); i++ )\
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

int MRX_T4Service::on_alignj_proc( QJsonDocument &doc )
{
    pre_def( IntfAlignj );

    post_call( on_alignj_proc );

    return 0;
}
int MRX_T4Service::post_on_alignj_proc( QJsonDocument &doc )
{
    pre_def( IntfMovej );

    try_deload_double( j1 );
    try_deload_double( j2 );
    try_deload_double( j3 );
    try_deload_double( j4 );
    try_deload_double( j5 );

    try_deload_double( a );
    deload_double( v );
    try_deload_double( t );

    if ( _has_item(j4) )
    {
        //! \note align the dst angle
        float normAngle = 270 - var.j4;
        normAngle = alignP360( normAngle );

        //! \note config the speed not time
        int tmoms;
        tmoms = guessTmo( 3, 360, var.v );
        localRet = mrgSetRobotWristPose( local_vi(),
                                              robot_handle(),
                                              wave_table,
                                              normAngle,
                                              var.v,
                                              tmoms );
    }

    return localRet;
}

int MRX_T4Service::on_movej_proc( QJsonDocument &doc )
{
    pre_def( IntfMovej );

    post_call( on_movej_proc );

    return 0;
}

#define move_sub_joint( id, jid ) \
if ( _has_item( jid ) )\
{\
    dist = var.jid - angles[id];\
    if ( qAbs(var.v) < FLT_EPSILON )\
    { return -2; }\
    else\
    { t = qAbs( dist ) / qAbs( var.v ); }\
\
    if ( _has_item(t) )\
    { t = var.t; }\
\
    if ( t < FLT_EPSILON )\
    { return -2; }\
\
    localRet = mrgMRQAdjust( local_vi(),\
                             device_handle(),\
                             id,\
                             wave_table,\
                             dist,\
                             t,\
                             t * 10000\
                             );\
    if ( localRet != 0 )\
    { return localRet; }\
}

int MRX_T4Service::post_on_movej_proc( QJsonDocument &doc )
{
    pre_def( IntfMovej );

    try_deload_double( j1 );
    try_deload_double( j2 );
    try_deload_double( j3 );
    try_deload_double( j4 );
    try_deload_double( j5 );

    try_deload_double( a );
    deload_double( v );
    try_deload_double( t );

    float angles[16] = {0};
    localRet = mrgGetRobotJointAngle( local_vi(),
                                  robot_handle(),
                                  -1,
                                  angles );
    if ( localRet != 5 )
    { return -1; }

    float dist,t;

    move_sub_joint( 0, j1 );
    move_sub_joint( 1, j2 );
    move_sub_joint( 2, j3 );
    move_sub_joint( 3, j4 );

    move_sub_joint( 4, j5 );

    return 0;
}

int MRX_T4Service::on_move_proc( QJsonDocument &doc )
{
    pre_def( IntfMove );

    post_call( on_move_proc );

    return 0;
}
int MRX_T4Service::post_on_move_proc( QJsonDocument &doc )
{
    pre_def( IntfMove );

    deload_double( x );
    deload_double( y );
    deload_double( z );

    deload_double( v );
    try_deload_double( a );
    try_deload_double( t );

    float t, tmo;
    localRet = guessTTmo( var.x, var.y, var.z, var.v, &t, &tmo );
    if ( localRet != 0 )
    { return localRet; }

    //! valid t
//    if ( var.bmMap.contains("t") && var.bmMap.value("t") )
    if ( _has_item(t) )
    { t = var.t; }

    //! check t
    if ( t < FLT_EPSILON )
    { return 0; }

    localRet = mrgRobotMove( local_vi(),
                              robot_handle(),
                              wave_table,
                              var.x, var.y, var.z,
                              t, tmo
                              );

    return localRet;
}

int MRX_T4Service::on_movel_proc( QJsonDocument &doc )
{
    pre_def( IntfMovel );

    post_call( on_movel_proc );

    return 0;
}
int MRX_T4Service::post_on_movel_proc( QJsonDocument &doc )
{
    pre_def( IntfMovel );

    deload_double( x );
    deload_double( y );
    deload_double( z );

    try_deload_double( step );

    deload_double( v );
    try_deload_double( a );
    try_deload_double( t );

    float t, tmo;
    localRet = guessTTmo( var.x, var.y, var.z, var.v, &t, &tmo );
    if ( localRet != 0 )
    { return localRet; }

    //! valid t
//    if ( var.bmMap.contains("t") && var.bmMap.value("t") )
    if ( _has_item(t) )
    { t = var.t; }

    //! valid step
    if ( _has_item(step) )
    {
        mrgSetRobotInterPolateStep( local_vi(),
                                    robot_handle(),
                                    var.step );
    }

    //! check t
    if ( t < FLT_EPSILON )
    { return 0; }

    localRet = mrgRobotMoveL( local_vi(),
                              robot_handle(),
                              wave_table,
                              var.x, var.y, var.z,
                              t, tmo
                              );

    return localRet;
}

int MRX_T4Service::on_setio( QJsonDocument &doc )
{
    pre_def( IntfSetio );

    deload_string( type );

    if ( var.type == "syncdo" )
    {
        localRet = on_seto( doc );
    }
    else
    { return localRet; }

    return localRet;
}

//! 1,2,3,4
int MRX_T4Service::on_seto( QJsonDocument &doc )
{
    pre_def( IntfSetio );

    deload_string( type );
    deload_int( value );
    //! deload array
    if ( obj.contains("port") )
    {
        if ( obj.value("port").isArray() )
        {
            QJsonArray ary = obj.value("port").toArray();
            for ( int i = 0; i < ary.size(); i++ )
            {
                var.ports.append( ary.at(i).toInt() );
            }
        }
        else
        {
            var.ports.append( obj.value("port").toInt() );
        }
    }

    if ( var.ports.size() > 0  )
    {}
    else
    { return localRet; }

    //! setio
    for ( int i = 0; i < var.ports.size(); i++ )
    {
        localRet = mrgProjectSetYout( local_vi(),
                                      var.ports.at(i),
                                      var.value );
        if ( localRet != 0 )
        { return localRet; }
    }

    return 0;
}

int MRX_T4Service::on_getio( QJsonDocument &doc )
{
    pre_def( IntfGetio );

    deload_string( type );
    //! deload array
    if ( obj.contains("port") )
    {
        if ( obj.value("port").isArray() )
        {
            QJsonArray ary = obj.value("port").toArray();
            for ( int i = 0; i < ary.size(); i++ )
            {
                var.ports.append( ary.at(i).toInt() );
            }
        }
        else
        {
            var.ports.append( obj.value("port").toInt() );
        }
    }

    if ( var.ports.size() > 0  )
    {}
    else
    { return localRet; }

    //! query
    if ( var.type == "getdi" )
    {
        query_x( on_getdi, var.ports );
    }
    else if ( var.type == "getdo" )
    {
        query_x( on_getdo, var.ports );
    }
    else
    { return localRet; }

    return localRet;
}
int MRX_T4Service::on_getdi( QJsonDocument &doc, QList<int> &ports )
{
    pre_def( IntfDIOs );

    //! get all di
    quint32 dis;
    localRet = mrgProjectGetXinState( local_vi(), 0, &dis );
    if ( localRet < 0 )
    { return localRet; }

    //! return the need
    for ( int i = 0; i < ports.size(); i++ )
    {
        var.value.append( ( dis >> ( ports.at(i)-1 ) ) & 0x01 );
    }

    //! export
    obj.insert("command", "getdi" );
    export_list( value );

    doc.setObject( obj );

    return 0;
}
int MRX_T4Service::on_getdo( QJsonDocument &doc, QList<int> &ports )
{
    //! \todo get do

    pre_def( IntfDIOs );

    //! get all di
    quint32 dis;
    localRet = mrgProjectGetXinState( local_vi(), 0, &dis );
    if ( localRet < 0 )
    { return localRet; }

    //! return the need
    for ( int i = 0; i < ports.size(); i++ )
    {
        var.value.append( ( dis >> ( ports.at(i)-1 ) ) & 0x01 );
    }

    //! export
    obj.insert("command", "getdo" );
    export_list( value );

    doc.setObject( obj );

    return 0;
}

int MRX_T4Service::on_execute( QJsonDocument &doc )
{
    pre_def(IntfExecute);

    post_call( on_execute );

    return 0;
}
int MRX_T4Service::post_on_execute( QJsonDocument &doc )
{
    pre_def(IntfExecute);

    deload_string( script );

    QString curPath = QDir::currentPath();

    //! try the path
    QString wp = qApp->applicationDirPath() + "/execute/megarobo";
    QDir dir( wp );
    if ( dir.exists() )
    { QDir::setCurrent( wp ); }
    else
    { QDir::setCurrent("G:/study/py/megarobo"); }

    QString scriptFile = "scrpt.mrl";

    QFile file( scriptFile );
    if ( file.open( QIODevice::WriteOnly ) )
    {}
    else
    { return -1; }

    file.write( var.script.toLatin1() );

    file.close();



#ifdef _WIN32
    QString cmd="cmd";
    QStringList argList;
    argList<<"/c"<<"runmrl.bat"<<scriptFile;
#else
    QString cmd="sh";
    QStringList argList;
    argList<<"runmrl.sh"<<scriptFile;
#endif

    //! start console thread
    ConsoleThread *pConsoleThread = new ConsoleThread( cmd, argList );
    if ( NULL == pConsoleThread )
    { return -1; }

    _pLocalServer->connectConsole( pConsoleThread );

    pConsoleThread->start();

    logDbg();

    return localRet;
}

int MRX_T4Service::rawStatus( QString &status )
{
    check_connect();

    int localRet;
    char states[128];

    status = "exception_stoped";
    localRet = mrgGetRobotStates( local_vi(),
                      robot_handle(),
                      wave_table,
                      states );
    if ( localRet != 0 )
    { logError()<<"status read fail"; }
    else if ( QString( states ).toLower() == "idle" )
    {
        status = "stoped";
    }
    else
    {
        status = "running";
    }

    return localRet;
}

int MRX_T4Service::guessTmo( int joint, float dist, float speed )
{
    Q_ASSERT ( qAbs(speed) > FLT_EPSILON );

    float t = qAbs(dist/speed);

    if ( t < FLT_EPSILON )
    { return 1000; }

    return 60 * t *1000;
}

int MRX_T4Service::guessTTmo( float x, float y, float z,
                              float v, float *pT, float *pTmo )
{
    int localRet;

    float fx, fy, fz;
    localRet = mrgGetRobotCurrentPosition( m_pServer->deviceVi(),
                                           m_pServer->robotHandle(),
                                           &fx, &fy, &fz );
    if ( localRet != 0 )
    { return localRet; }


    //! calc the dist
    float dist = eulaDistance( x, y, z, fx, fy, fz );

    *pT = dist / qAbs( v );
    *pTmo = dist * 1000 + *pT + 500;

    if ( *pT < FLT_EPSILON )
    { *pT = 0; }

    return 0;
}

double MRX_T4Service::eulaDistance( double x, double y, double z,
                            double x1, double y1, double z1 )
{
    //! calc the distance
    double dist = sqrt( pow(  x - x1, 2) +
                       pow(  y - y1, 2) +
                       pow(  z - z1, 2)
                       );
    return dist;
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
