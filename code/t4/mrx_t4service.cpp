#include "mrx_t4service.h"
#include "../mydebug.h"
#include "../myjson.h"
#include "t4para.h"

#include "mrx_t4server.h"
#include "MegaGateway.h"

#include "intfseries.h"
#include <math.h>
#include <float.h>

#define flt_max (9.9e37)

#define deg_to_rad( deg )   ( (deg)* M_PI /180)

#define get_bit( d, n )     ( ( d >> n ) & 1 )

#define api_class   MRX_T4Service
#define api_server  MRX_T4Server

#define bg_refresh( proc )  postRefresh(true);\
                            try{ proc; }\
                            catch( QException &e )\
                            { postRefresh(false); throw e; }\
                            postRefresh(false);

#define ack_status( )   query_( on_pose_proc );query_( on_device_status_proc );

#define ack_raw_status()    query_( on_pose_proc );query_( on_device_status_raw_proc );

MRX_T4Service::MRX_T4Service( qintptr ptr, quint16 port, QObject *parent ) : MAppService( ptr, port, parent )
{
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

    //! fill map
    attachProc( QString("ack"), (api_type)&api_class::on_ack_proc, &pServer->mQueryFifoMutex );
    attachProc( QString("step"), (api_type)&api_class::on_step_proc, &pServer->mQueryFifoMutex, 500 );
    attachProc( QString("joint_step"), (api_type)&api_class::on_joint_step_proc, &pServer->mQueryFifoMutex, 500 );
    attachProc( QString("action"), (api_type)&api_class::on_action_proc, &pServer->mQueryFifoMutex );

    attachProc( QString("indicator"), (api_type)&api_class::on_indicator_proc, &pServer->mQueryFifoMutex);
    attachProc( QString("add"), (api_type)&api_class::on_add_proc, &pServer->mQueryFifoMutex);
    attachProc( QString("query"), (api_type)&api_class::on_query_proc, &pServer->mQueryFifoMutex);
    attachProc( QString("link_status"), (api_type)&api_class::on_link_status_proc, &pServer->mQueryFifoMutex);

    attachProc( QString("config"), (api_type)&api_class::on_config_proc, &pServer->mQueryFifoMutex);

    attachProc( QString("file"), (api_type)&api_class::on_file_proc, &pServer->mQueryFifoMutex);
    attachProc( QString("dir"), (api_type)&api_class::on_dir_proc, &pServer->mQueryFifoMutex);

    attachProc( QString("alignj"), (api_type)&api_class::on_alignj_proc, &pServer->mQueryFifoMutex );
    attachProc( QString("movej"), (api_type)&api_class::on_movej_proc, &pServer->mQueryFifoMutex );
    attachProc( QString("move"), (api_type)&api_class::on_move_proc, &pServer->mQueryFifoMutex );
    attachProc( QString("movel"), (api_type)&api_class::on_movel_proc, &pServer->mQueryFifoMutex );

    attachProc( QString("setio"), (api_type)&api_class::on_setio, &pServer->mQueryFifoMutex );
    attachProc( QString("getio"), (api_type)&api_class::on_getio, &pServer->mQueryFifoMutex );

    attachProc( QString("execute"), (api_type)&api_class::on_execute, &pServer->mQueryFifoMutex );
}

int MRX_T4Service::on_refreshtimeout()
{logDbg();
    int localRet;

    //! return the pose
    query_( on_pose_proc );

    return localRet;
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
        //! \note no speed ratio
        localRet = mrgRobotMoveOn( local_vi(),
                                   robot_handle(),
                                   wave_table,
                                   lx,
                                   ly,
                                   lz,
                                   pLocalServer->mMaxBodySpeed  );
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

    deload_int( joint );

    try_deload_double( value );
    try_deload_bool( continous );

    try_deload_double( step );

    post_call( on_joint_step_proc );
}

int MRX_T4Service::post_on_joint_step_proc(  QJsonDocument &doc )
{
    pre_def( Intfjoint_step );

    deload_int( joint );

    try_deload_double( value );
    try_deload_bool( continous );

    try_deload_double( step );

    if ( _has_item( step ) )
    { return joint_stepD_proc(doc); }
    else if ( _has_item( value ) && _has_item(continous) )
    { return joint_stepN_proc( doc ); }
    else
    { return -1; }
}

//! n step
int MRX_T4Service::joint_stepN_proc(  QJsonDocument &doc )
{
    pre_def( Intfjoint_step );

    deload_int( joint );

    deload_double( value );
    deload_bool( continous );

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
                tmoms = guessTmo( 3,
                                  var.value * pLocalServer->localJStep(),
                                  pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio() );
                localRet = mrgMRQAdjust( local_vi(),
                                         device_handle(),
                                         3,
                                         wave_table,
                                         var.value * pLocalServer->localJStep(),
                                         pLocalServer->localJStep() / pLocalServer->mMaxJointSpeed / pLocalServer->localSpeedRatio(),
                                         tmoms );
            }
            //! aim
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
            tmoms = guessTmo( 4,
                              var.value * pLocalServer->localJStep(),
                              pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio() );
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

//! step distance
int MRX_T4Service::joint_stepD_proc(  QJsonDocument &doc )
{
    pre_def( Intfjoint_step );

    deload_int( joint );
    deload_double( step );

    //! \note too small
    if ( qAbs( var.step ) < FLT_EPSILON )
    { return 0; }

    int tmoms;
    if ( var.joint == 3 )
    {
        tmoms = guessTmo( 3, var.step, pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio() );logDbg()<<tmoms;
        localRet = mrgMRQAdjust( local_vi(),
                                 device_handle(),
                                 3,
                                 wave_table,
                                 var.step,
                                 qAbs(var.step) / pLocalServer->mMaxJointSpeed / pLocalServer->localSpeedRatio(),
                                 tmoms );
    }
    else if ( var.joint == 4 )
    {
        //! check the max/min
        if ( qAbs(var.step) >= ( flt_max - FLT_EPSILON ) )
        {
            if ( var.step > 0 )
            { var.step = 180; }
            else
            { var.step = -180; }
        }

        //! do the step
        tmoms = guessTmo( 4, var.step, pLocalServer->mMaxJointSpeed * pLocalServer->localSpeedRatio() );
        localRet = mrgMRQAdjust( local_vi(),
                                 device_handle(),
                                 4,
                                 wave_table,
                                 var.step,
                                 qAbs(var.step) / pLocalServer->mMaxJointSpeed / pLocalServer->localSpeedRatio(),
                                 tmoms );
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
            //! \note all workings
            _pLocalServer->stopWorkings();

            //! \note force stop
            localRet = mrgSysSetEmergencyStopState( local_vi(), 1 );

            localRet = mrgSysSetEmergencyStopState( local_vi(), 0 );
        }
        else
        {}
    }
    else if ( var.item == "emergency_stop" )
    {
        _pLocalServer->stopWorkings();

        //! \note force stop
        localRet = mrgSysSetEmergencyStopState( local_vi(), 1 );

        localRet = mrgSysSetEmergencyStopState( local_vi(), 0 );

        //! \errant wait for end
        localRet = mrgRobotWaitEnd( local_vi(), robot_handle(), wave_table, 100 );

        ack_raw_status();

        return localRet;
    }
    else if ( var.item == "stop" )

    {
        _pLocalServer->stopWorkings();

        localRet = mrgRobotStop( local_vi(), robot_handle(), wave_table );

        //! \errant wait for end
        localRet = mrgRobotWaitEnd( local_vi(), robot_handle(), wave_table, 100 );

        ack_raw_status();

        return localRet;
    }
    //! 1. stop
    //! 2. post package
    else if ( var.item == "package" )
    {
        _pLocalServer->stopWorkings();

        localRet = mrgRobotStop( local_vi(), robot_handle(), wave_table );

//        Q_ASSERT( NULL != m_pWorkingThread );
//        m_pWorkingThread->requestInterruption();
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
        bg_refresh( localRet = mrgRobotGoHome( local_vi(),
                                               robot_handle(),
                                               120000 ) );

//        //! start refresh pose
//        startRefresh();

//        try
//        {
//            localRet = mrgRobotGoHome( local_vi(),
//                                   robot_handle(),
//                                   120000 );
//        }
//        catch( QException &e )
//        {
//            stopRefresh();
//            throw e;
//        }

//        //! end refresh
//        stopRefresh();

    }
    else if ( var.item == "emergency_stop" )
    {
        //! \note has stoped
    }
    else if ( var.item == "stop" )
    {

        //! \note has stoped
    }
    else if ( var.item == "package" )
    {
        bg_refresh( localRet = mrgSetRobotFold( local_vi(),
                                    robot_handle(),
                               0,
                               18.8,
                               -57.4,
                               -103,
                               120000
                               ) );

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
        query_( on_ctorque_proc );
    }
    else if ( var.item == "ccurrent" )
    {
        query_( on_ccurrent_proc );
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
    { var.status = "running"; logDbg()<<m_pServer->workings(); }
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

    //! \note align to zero
    obj.insert( "j1", angles[0] - 0 );
    obj.insert( "j2", angles[1] - 180 );
    obj.insert( "j3", angles[2] - 90 );
    obj.insert( "j4", angles[3] - 90 );

    //! \todo the hand angle
    obj.insert( "j5", angles[5] );

    doc.setObject( obj );

    return 0;
}

int MRX_T4Service::on_ctorque_proc( QJsonDocument &doc )
{
    pre_def( Intfpose );

    quint32 datas[ 5 ];
    qint32 val;

    for ( int i = 0 ; i < 5; i++ )
    {
        localRet = mrgMRQReportData_Query( local_vi(),
                             device_handle(),
                             i,
                             0,
                             datas + i );
        if ( localRet != 1 )
        { return -1; }
    }

    for ( int i = 0; i < sizeof_array( datas); i++ )
    {
        val = (qint32)( (datas[i]>> 8 ) & 0xff );
        obj.insert( QString("j%1").arg(i+1), val );
    }

    doc.setObject( obj );

    return 0;
}
int MRX_T4Service::on_ccurrent_proc( QJsonDocument &doc )
{
    pre_def( Intfpose );

    quint32 datas[ 5 ];
    qint32 val;

    for ( int i = 0 ; i < 5; i++ )
    {
        localRet = mrgMRQReportData_Query( local_vi(),
                             device_handle(),
                             i,
                             0,
                             datas + i );

        if ( localRet != 1 )
        { return -1; }
    }

    for ( int i = 0; i < sizeof_array( datas ); i++ )
    {
        val = (qint32)( datas[i] & 0xff);
        obj.insert( QString("j%1").arg(i+1), val );
    }

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

int MRX_T4Service::on_file_proc( QJsonDocument &doc )
{
    pre_def( IntfFile );

    deload_string( action );
    if ( var.action == "write" )
    { do_query_(on_file_write); }
    else if ( var.action == "read" )
    { do_query_(on_file_read); }
    else if ( var.action == "delete" )
    { do_query_(on_file_delete); }
    else if ( var.action == "size" )
    { do_query_(on_file_size); }
    else
    { }

    return localRet;
}

int MRX_T4Service::on_file_write( QJsonDocument &doc )
{
    pre_def( IntfFile );

    deload_string( command );
    deload_string( name );
    deload_string( contents );
    deload_string( action );

    QFile file( m_pServer->homePath() + "/" + var.name );
    if ( !file.open( QIODevice::WriteOnly ) )
    { var.ret = -1; }
    else
    {
        int len = var.contents.toLatin1().size();
        if ( len != file.write( var.contents.toLatin1() ) )
        { var.ret = -1; }
        else
        { var.ret = 0; }

        file.close();
    }

    json_obj( command );
    json_obj( name );
    json_obj( action );
    json_obj( contents );

    json_obj( ret );

    doc.setObject( obj );
    return 0;
}
int MRX_T4Service::on_file_read( QJsonDocument &doc )
{
    pre_def( IntfFile );

    deload_string( command );
    deload_string( name );
    deload_string( action );
    QFile file( m_pServer->homePath() + "/" + var.name );
    if ( !file.open( QIODevice::ReadOnly ) )
    { var.ret = -1; }
    else
    {
        var.contents = file.readAll();
        file.close();
        var.ret = 0;
    }

    json_obj( command );
    json_obj( name );
    json_obj( action );
    json_obj( contents );

    json_obj( ret );

    doc.setObject( obj );
    return 0;
}
int MRX_T4Service::on_file_delete( QJsonDocument &doc )
{
    pre_def( IntfFile );

    deload_string( command );
    deload_string( name );
    deload_string( action );

    QFile file( m_pServer->homePath()+"/" + var.name );
    if ( file.remove() )
    { var.ret = 0; }
    else
    { var.ret = -1; }

    json_obj( command );
    json_obj( name );
    json_obj( action );
    json_obj( contents );

    json_obj( ret );

    doc.setObject( obj );
    return 0;
}
int MRX_T4Service::on_file_size( QJsonDocument &doc )
{
    pre_def( IntfFile );

    int fSize = 0;
    deload_string( command );
    deload_string( name );
    deload_string( action );
    QFile file( m_pServer->homePath() + "/" + var.name );
    if ( !file.open(QIODevice::ReadOnly ) )
    {
        var.ret = -1;
    }
    else
    {
        fSize = file.size();
        file.close();
        var.ret = 0;
    }

    json_obj( command );
    json_obj( name );
    json_obj( action );
//    json_obj( contents );
    obj.insert( "contents", fSize );

    json_obj( ret );

    doc.setObject( obj );

    return 0;
}

int MRX_T4Service::on_dir_proc( QJsonDocument &doc )
{
    pre_def( IntfDir );

    deload_string( action );
    if ( var.action == "create" )
    { do_query_(on_dir_create); }
    else if ( var.action == "delete" )
    { do_query_(on_dir_delete); }
    else if ( var.action == "size" )
    { do_query_(on_dir_size); }
    else if ( var.action == "list" )
    { do_query_(on_dir_list); }
    else
    { }

    return localRet;
}

int MRX_T4Service::on_dir_create( QJsonDocument &doc )
{
    pre_def( IntfDir );

    deload_string( command );
    deload_string( name );
    deload_string( action );

    QString path = m_pServer->homePath() + "/" + var.name;
    QDir dir( path );
    if ( dir.exists() )
    { var.ret = 0; }
    else
    {}

    if ( dir.mkpath(path) )
    { var.ret = 0; }
    else
    { var.ret = -1; }

    json_obj( command );
    json_obj( name );
    json_obj( action );
//    json_obj( contents );

    json_obj( ret );

    doc.setObject( obj );
    return 0;
}
int MRX_T4Service::on_dir_delete( QJsonDocument &doc )
{
    pre_def( IntfDir );

    deload_string( command );
    deload_string( name );
    deload_string( action );

    QString path = m_pServer->homePath() + "/" + var.name;
    QDir dir( path );

    if ( dir.removeRecursively() )
    { var.ret = 0; }
    else
    { var.ret = -1; }

    json_obj( command );
    json_obj( name );
    json_obj( action );
//    json_obj( contents );

    json_obj( ret );

    doc.setObject( obj );
    return 0;
}
int MRX_T4Service::on_dir_size( QJsonDocument &doc )
{
    pre_def( IntfDir );

    deload_string( command );
    deload_string( name );
    deload_string( action );

    QString path = m_pServer->homePath() + "/" + var.name;
    QDir dir( path );

    int dirSize = 0;
    QFileInfoList infoList;
    if ( dir.exists() )
    {
        infoList = dir.entryInfoList();
        var.ret = 0;
        dirSize = infoList.size();
    }
    else
    { var.ret = -1; }

    json_obj( command );
    json_obj( name );
    json_obj( action );

    obj.insert( "contents", dirSize );

    json_obj( ret );

    doc.setObject( obj );
    return 0;
}
int MRX_T4Service::on_dir_list( QJsonDocument &doc )
{
    pre_def( IntfDir );

    deload_string( command );
    deload_string( name );
    deload_string( action );

    QString path = m_pServer->homePath() + "/" + var.name;
    QDir dir( path );

    QFileInfoList infoList;
    infoList = dir.entryInfoList();

    //! for the info list
    QJsonArray infoAry;
    for ( int i = 0; i < infoList.size(); i++ )
    {
        QJsonObject infoObj;

        infoObj.insert( "name", infoList.at(i).fileName() );
        infoObj.insert( "type", infoList.at(i).isFile() ? "file":"dir" );
        infoObj.insert( "size", infoList.at(i).size() );
        infoObj.insert( "ts", infoList.at(i).lastModified().toString() );

        infoAry.append( infoObj );
    }

    obj.insert( "contents", infoAry );

    var.ret = 0;

    json_obj( command );
    json_obj( name );
    json_obj( action );
//    json_obj( contents );

    json_obj( ret );

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

    //! init
    var.j1 = 0;
    var.j2 = 0;
    var.j3 = 0;
    var.j4 = 0;
    var.j5 = 0;

    try_deload_double( j1 );
    try_deload_double( j2 );
    try_deload_double( j3 );
    try_deload_double( j4 );
    try_deload_double( j5 );

    try_deload_double( a );
    try_deload_double( v );
    try_deload_double( t );

    //! check a, v, t
    if ( _has_item(v) || _has_item(t) )
    {}
    else
    { return -1; }

    //! check contents
    if ( _has_item(j1) || _has_item(j2) || _has_item(j3) || _has_item(j4) || _has_item(j5) )
    {
        //! guess the max angle
        double angle = qMax( qAbs(var.j1), qAbs(var.j2) );
        angle = qMax( qAbs(var.j3), angle );
        angle = qMax( qAbs(var.j4), angle );
        angle = qMax( qAbs(var.j5), angle );

        do
        {
            if ( angle < FLT_EPSILON )
            { break; }

            float rt;
            if ( _has_item(t) )
            {
                rt = var.t;
            }
            else if ( _has_item(v) )
            {
                rt = angle / qAbs(var.v);
            }
            else
            { return -1; }

            //! check t
            if ( rt < FLT_EPSILON )
            { return -1; }

            float angles[5]={ var.j1, var.j2, var.j3, var.j4, var.j5 };

            localRet = mrgRobotMoveJ( local_vi(),
                                      robot_handle(),
                                      angles,
                                      rt,
                                      5,
                                      guessTmoT( 0, angle, rt )
                                      );

            if ( localRet != 0 )
            { return localRet; }
        }while( 0 );
    }
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

    //! check port
    for ( int i = 0; i < var.ports.size(); i++ )
    {
        if ( var.ports.at(i) < IOSET_Y1 || var.ports.at(i) > IOSET_Y4 )
        { logDbg();return -1; }
    }

    qint32 mask = 0;
    for ( int i = 0; i < var.ports.size(); i++ )
    {
        mask |= 1<<var.ports.at(i);
    }

    localRet = mrgProjectIOSet( local_vi(), IOSET_ALL, var.value, mask );
    return localRet;

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

//! di: 1,2,3,4,5,,,10
int MRX_T4Service::on_getdi( QJsonDocument &doc, QList<int> &ports )
{
    pre_def( IntfDIOs );

    //! check port
    for ( int i = 0; i < ports.size(); i++ )
    {
        if ( ports.at(i) < IOGET_X1 || ports.at(i) > IOGET_X10 )
        { return -1; }
    }

    localRet = _getio( doc, ports, IOGET_X1, var.value );
    if ( localRet != 0 )
    { return localRet; }

    //! export
    obj.insert("command", "getdi" );
    export_list( value );

    doc.setObject( obj );

    return 0;
}

//! :1,2,3,4
int MRX_T4Service::on_getdo( QJsonDocument &doc, QList<int> &ports )
{
    pre_def( IntfDIOs );

    //! check port
    for ( int i = 0; i < ports.size(); i++ )
    {
        if ( ports.at(i) < (IOGET_Y1 - IOGET_Y1 + 1) || ports.at(i) > (IOGET_Y4 - IOGET_Y1 + 1) )
        { return -1; }
    }

    localRet = _getio( doc, ports, IOGET_Y1, var.value );
    if ( localRet != 0 )
    { return localRet; }

    //! export
    obj.insert("command", "getdo" );
    export_list( value );

    doc.setObject( obj );

    return 0;
}

int MRX_T4Service::_getio( QJsonDocument &doc, QList<int> &ports, int from, QList<int> &portVals )
{
    pre_def( IntfDIOs );

    //! get all di
    char diostat[128];
    localRet = mrgProjectIOGet( local_vi(), IOGET_ALL, diostat );
    if ( localRet < 0 )
    { return localRet; }

    //! convert the dis to value
    quint32 dis = 0;
    QString str = diostat;
    QStringList strList = str.split(',', QString::SkipEmptyParts );
    foreach( QString sub, strList )
    {
        if ( sub == "H" )
        { dis |= 1; }
        else if ( sub == "L" )
        { dis |= 0; }
        else
        { }

        dis <<= 1;
    }

    //! return the need
    for ( int i = 0; i < ports.size(); i++ )
    {
        portVals.append( ( dis >> ( ports.at(i)-(int)from ) ) & 0x01 );
    }

    return 0;
}

int MRX_T4Service::on_execute( QJsonDocument &doc )
{
    pre_def(IntfExecute);

    //! \note exec now for e_stop in next step
    return post_on_execute( doc );
}
int MRX_T4Service::post_on_execute( QJsonDocument &doc )
{
    pre_def(IntfExecute);

    try_deload_string( script );
    try_deload_string( shell );
    try_deload_string( args );

    if ( _has_item(script) )
    {
        localRet = post_on_execute_script( doc );
    }
    else if ( _has_item(shell) )
    {
        localRet = post_on_execute_shell( doc );
    }
    else
    { return -1; }

    return localRet;
}

int MRX_T4Service::post_on_execute_script( QJsonDocument &doc )
{
    pre_def(IntfExecute);

    deload_string( script );

    //! try the path
    switch_shell_dir();

    QString scriptFile = "scrpt.mrl";
    QFile file( scriptFile );
    if ( file.open( QIODevice::WriteOnly ) )
    {}
    else
    { return -1; }

    file.write( var.script.toLatin1() );

    file.close();

    localRet = run_shell( scriptFile, var.args.split(" ") );

    return localRet;
}

int MRX_T4Service::post_on_execute_shell( QJsonDocument &doc )
{
    pre_def(IntfExecute);

    deload_string( shell );
    try_deload_string( args );

    switch_shell_dir();

    localRet = run_shell( var.shell, var.args.split(" ") );

    return localRet;
}

void MRX_T4Service::switch_shell_dir()
{
    //! try the path
    QString wp = qApp->applicationDirPath() + "/execute/megarobo";
    QDir dir( wp );
    if ( dir.exists() )
    { QDir::setCurrent( wp ); }
    else
    { QDir::setCurrent("G:/study/py/megarobo"); }
}

int MRX_T4Service::run_shell( const QString &scriptFile, const QStringList &args )
{
    check_connect();

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
    ConsoleThread *pConsoleThread = new ConsoleThread( cmd, argList, args );
    if ( NULL == pConsoleThread )
    { return -1; }

    _pLocalServer->connectConsole( pConsoleThread );

    connnectConsoleWorkings( pConsoleThread );

    pConsoleThread->start();

    logDbg();

    return 0;
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

#define STATE_DELAY_TIME    1000
#define WORKING_TIME_OUT    5000
int MRX_T4Service::guessTmoT( int joint, float angle, float t )
{
    return t * 1000 + STATE_DELAY_TIME + WORKING_TIME_OUT;
}

int MRX_T4Service::guessTmo( int joint, float dist, float speed )
{
    Q_ASSERT ( qAbs(speed) > FLT_EPSILON );

    float t = qAbs(dist/speed);

    if ( t < FLT_EPSILON )
    { return 1000; }

    return t *1000 + STATE_DELAY_TIME + WORKING_TIME_OUT;
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
