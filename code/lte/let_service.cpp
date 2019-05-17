#include "let_service.h"
#include "let_server.h"
#include "let_intfseries.h"

#include <float.h>
#include "../myjson.h"
#include "../mydebug.h"

#include "MegaGateway.h"

#define api_class   Let_Service
#define api_server  Let_Server

#define check_local_ret()  if( localRet != 0 ){ return localRet; }

Let_Service::Let_Service( qintptr ptr, QObject *parent ) : MAppService( ptr, parent )
{
    attachProc( QString("request"), (api_type)&api_class::on_request );
    attachProc( QString("query"), (api_type)&api_class::on_query );
    attachProc( QString("config"), (api_type)&api_class::on_config );

    m_pWorkingThread = new WorkingThread();
    Q_ASSERT( NULL != m_pWorkingThread );
}
Let_Service::~Let_Service()
{

}

bool Let_Service::onUserEvent(QEvent *pEvent)
{
//    logDbg_Thread();

    MServiceEvent *pLocalEvent = (MServiceEvent*)pEvent;
    QJsonDocument localDoc = pLocalEvent->mVar1.toJsonDocument();

    //! proc the json obj
    proc( localDoc, pLocalEvent->mTs );

    return true;
}

void Let_Service::attachServer( MAppServer *pServer )
{
    //! attach
    MAppService::attachServer( pServer );

    //! link the working
    pServer->connectWorking( m_pWorkingThread );
}

int Let_Service::on_request( QJsonDocument &doc )
{
    pre_def( IntfRequest );

    deload_string( item );

    if (  var.item == "home" )
    {
        //! stop at first
        on_action_stop( doc );
        post_call( on_action_home );
    }
    else if ( var.item == "homez" )
    {
        on_action_stop( doc );
        post_call( on_action_homez );
    }
    else if ( var.item == "origin" )
    {
        post_call( on_action_origin );
    }
    else if ( var.item == "to" )
    {
        post_call( on_action_to );
    }
    else if ( var.item == "step" )
    {
        clearContinue();
        post_call( on_action_step );
    }
    else if ( var.item == "zigzagx" )
    {
        clearContinue();
        post_call( on_action_zigzagX );
    }
    else if ( var.item == "zigzagy" )
    {
        clearContinue();
        post_call( on_action_zigzagY );
    }
    else if ( var.item == "snakex" )
    {
        clearContinue();
        post_call( on_action_snakeX );
    }
    else if ( var.item == "snakey" )
    {
        clearContinue();
        post_call( on_action_snakeY );
    }
    else if ( var.item == "slope" )
    {
        clearContinue();
        post_call( on_action_slope );
    }
    else if ( var.item == "continue" )
    {
        continueNext();
        localRet = 0;
    }
    else if ( var.item == "stop" )
    {
        localRet = on_action_stop( doc );
    }
    else if ( var.item == "estop" )
    {
        localRet = on_action_eStop( doc );
    }
    else
    {}

    return localRet;
}

int Let_Service::on_action_stop( QJsonDocument &doc )
{
    pre_def( IntfRequest );
logDbg();
    //! terminate working
    m_pWorkingThread->requestInterruption();
    m_pWorkingThread->wait();
logDbg();
    clearContinue();
logDbg();
    localRet = _pLocalServer->mZAxes.stop();
    check_local_ret();
logDbg();
    localRet = mrgRobotStop( local_vi(), robot_handle(), wave_table );
    check_local_ret();
logDbg();
    return 0;
}
int Let_Service::on_action_eStop( QJsonDocument &doc )
{
    pre_def( IntfRequest );

    //! terminate working
    m_pWorkingThread->requestInterruption();
    m_pWorkingThread->wait();

    clearContinue();

    localRet = mrgSysSetEmergencyStop( local_vi(), 1 );
    check_local_ret();

    localRet = mrgSysSetEmergencyStop( local_vi(), 0 );
    check_local_ret();

    return 0;
}

int Let_Service::post_on_action_home( QJsonDocument &doc )
{
    pre_def( IntfRequest );

    deload_double( velocity );

    //! home
    {
        localRet = mrgRobotGoHome( local_vi(),
                                   robot_handle(),
                                   1000*1000 / qAbs( var.velocity ) + 120000 );
        if ( localRet != 0 )
        { return localRet; }
    }

    //! to ox, oy
    {
        float t, dist;
        int tmo;
        localRet = guessT_tmo( _pLocalServer->mOx, _pLocalServer->mOy, 0,
                    var.velocity, &t, &tmo, &dist );
        if ( localRet != 0 ){ return localRet; }

        if ( dist < FLT_EPSILON )
        { return 0; }

        localRet = mrgRobotRelMove( local_vi(),
                                 robot_handle(),
                                 wave_table,
                                 _pLocalServer->mOx,
                                 _pLocalServer->mOy,
                                 0,
                                 t, tmo
                                 );
        if ( localRet != 0 ){ return localRet; }
    }

    return localRet;
}

int Let_Service::post_on_action_homez( QJsonDocument &doc )
{
    pre_def( IntfRequest );

    deload_double( velocity );

    //! home
    {
        _pLocalServer->mZAxes.configZero( var.velocity,
                                          _pLocalServer->mZGap,
                                          _pLocalServer->mZGapSpeed );

        localRet = _pLocalServer->mZAxes.zero();
        if ( localRet != 0 )
        { return localRet; }
    }

    return localRet;
}

int Let_Service::post_on_action_origin( QJsonDocument &doc )
{
    pre_def( IntfRequest );
    deload_double( velocity );
    deload_double( x );
    deload_double( y );
    deload_double( z );

    float t;
    int tmo;

    localRet = guessT_tmo( var.x, var.y, var.z, var.velocity, &t, &tmo );
    if ( localRet != 0 )
    { return localRet; }

    //! x, y
    localRet = mrgRobotMove( local_vi(),
                             robot_handle(),
                             wave_table,
                             var.x, var.y, 0,
                             t,
                             tmo );
    check_local_ret();

    //! config origin
    _pLocalServer->mOx = var.x;
    _pLocalServer->mOy = var.y;
//    _pLocalServer->mOz = var.z;

    localRet = _pLocalServer->saveConfig();

    return localRet;
}

int Let_Service::post_on_action_to( QJsonDocument &doc )
{
    pre_def( IntfRequest );
    deload_double( velocity );
    deload_double( x );
    deload_double( y );
    deload_double( z );

    toWorld( var.x, var.y, var.z );

    float t;
    int tmo;

    localRet = guessT_tmo( var.x, var.y, var.z, var.velocity, &t, &tmo );
    if ( localRet != 0 )
    { return localRet; }

    //! x, y
    localRet = mrgRobotMove( local_vi(),
                             robot_handle(),
                             wave_table,
                             var.x, var.y, 0,
                             t,
                             tmo );
    check_local_ret();

//    localRet = _pLocalServer->mZAxes.move( wave_table,
//                                           var.z,
//                                           var.velocity );
//    check_local_ret();

    return localRet;
}

#define begin_pend()    try{
#define end_pend()      }catch( QException &e )\
                        { mPendSema.acquire();\
                          throw e; }
//#define pend_for_next()
#define pend_for_next()     mPendSema.release(); \
{\
    {\
        while( true )\
        {\
            if ( mContSemaphore.tryAcquire( 1, 1000 ) )\
            {\
                logDbg()<<"continue";\
                break;\
            }\
            else\
            { logDbg()<<"paused"; }\
\
            if ( QThread::currentThread()->isInterruptionRequested() )\
            { throw QException(); }\
        }\
    }\
}\
mPendSema.acquire();

int Let_Service::post_on_action_step( QJsonDocument &doc )
{
    pre_def( IntfRequest );

    deload_double( velocity );
    deload_double( x );
    deload_double( y );
    deload_double( z );

    _try_deload_xx( obj, n, Int, 1 );

    stepToWorld( var.x, var.y, var.z );

    double distxy = distance( 0,0,0, var.x, var.y, 0 );

    float t = motionTime( distxy, var.velocity );
    int tmoms = motionTimeoutms( distxy, var.velocity );

    int i = 0;
    do
    {
        begin_pend()
//        try{
            if ( distxy > 0 )
            {
                localRet = mrgRobotRelMove( local_vi(),
                                     robot_handle(),
                                     wave_table,
                                     var.x, var.y, 0,
                                     t,
                                     tmoms );
                if ( localRet != 0 )
                { return localRet; }
            }
            else if ( var.z != 0 )
            {
                _pLocalServer->mZAxes.step( wave_table, var.z, var.velocity );
            }
            else
            {}

            if ( (++i) < var.n )
            {}
            else
            { break; }

            pend_for_next();

        end_pend()

    }while( true );

    return localRet;
}

#define delta( v1, v2 )     qAbs( (v1) - (v2) )
#define delta_lt( v1, v2, er )     delta( v1, v2 ) < (er)

#define resolution_error    0.5
#define align_count( len, div ) ( ( qAbs(len) + resolution_error ) / div)
int Let_Service::post_on_action_zigzagX( QJsonDocument &doc )
{
    pre_def( IntfRequest );

    deload_double( velocity );
    deload_double( x );     //! +/-1 0
    deload_double( y );     //! +/-1 0
    deload_double( z );     //! +/-1 0

    float dx, dy, dz;

    stepToWorld( var.x, var.y, var.z );

    dx = var.x * _pLocalServer->mdH;
    dy = var.y * _pLocalServer->mdW;
    dz = 0;

    int n, m;

    float cx, cy, cz;
    localRet = mrgGetRobotCurrentPosition( local_vi(),
                                           robot_handle(),
                                           &cx, &cy, &cz );
    if ( localRet != 0 )
    { return localRet; }

    //! expect the n && m
    if ( var.x > 0 )
    {
        n = align_count( _pLocalServer->mH - cx, _pLocalServer->mdH );
    }
    else if ( var.x < 0 )
    {
        n = align_count( cx, _pLocalServer->mdH );
    }
    else
    { return -1; }

    if ( var.y > 0 )
    {
        m = align_count( _pLocalServer->mW - cy, _pLocalServer->mdW );
    }
    else if ( var.y < 0 )
    {
        m = align_count( cy, _pLocalServer->mdW );
    }
    else
    { return -1; }

    double dist = distance( 0,0,0,
                            dx,
                            dy,
                            dz );

    float t = motionTime( qAbs(dx), var.velocity );
    int tmoms = motionTimeoutms( qAbs(dx), var.velocity );

    begin_pend()

        for ( int my = 0; my <= m; my++ )
        {
            for( int nx = 0; nx < n; nx++ )
            {
                localRet = mrgRobotRelMove( local_vi(),
                                     robot_handle(),
                                     wave_table,
                                     dx, 0, dz,
                                     t,
                                     tmoms );
                if ( localRet != 0 )
                { return localRet; }

                pend_for_next();
            }

            //! return to next
            float t;
            int tmoms;
            localRet = guessT_tmo( cx, cy + dy, 0, _pLocalServer->mRV, &t, &tmoms );
            if ( localRet != 0 )
            { return localRet; }

            if ( my < m )
            {
                localRet = mrgRobotMove( local_vi(),
                                     robot_handle(),
                                     wave_table,
                                     cx, cy + dy*(my+1), cz,
                                     t, tmoms
                                        );
                if ( localRet != 0 )
                { return localRet; }
                pend_for_next();
            }
        }

        post_on_action_home( doc );
    end_pend()

    return localRet;
}
int Let_Service::post_on_action_zigzagY( QJsonDocument &doc )
{
    pre_def( IntfRequest );

    deload_double( velocity );
    deload_double( x );     //! +/-1 0
    deload_double( y );     //! +/-1 0
    deload_double( z );     //! +/-1 0

    float dx, dy, dz;

    stepToWorld( var.x, var.y, var.z );

    dx = var.x * _pLocalServer->mdH;
    dy = var.y * _pLocalServer->mdW;
    dz = 0;

    int n, m;

    float cx, cy, cz;
    localRet = mrgGetRobotCurrentPosition( local_vi(),
                                           robot_handle(),
                                           &cx, &cy, &cz );
    if ( localRet != 0 )
    { return localRet; }

    //! expect the n && m
    if ( var.x > 0 )
    {
        n = align_count( _pLocalServer->mH - cx, _pLocalServer->mdH );
    }
    else if ( var.x < 0 )
    {
        n = align_count( cx, _pLocalServer->mdH );
    }
    else
    { return -1; }

    if ( var.y > 0 )
    {
        m = align_count( _pLocalServer->mW - cy, _pLocalServer->mdW );
    }
    else if ( var.y < 0 )
    {
        m = align_count( cy, _pLocalServer->mdW );
    }
    else
    { return -1; }

    double dist = distance( 0,0,0,
                            dx,
                            dy,
                            dz );

    float t = motionTime( qAbs(dy), var.velocity );
    int tmoms = motionTimeoutms( qAbs(dy), var.velocity );

    begin_pend()
        for( int nx = 0; nx <= n; nx++ )
        {
            for ( int my = 0; my < m; my++ )
            {
                localRet = mrgRobotRelMove( local_vi(),
                                     robot_handle(),
                                     wave_table,
                                     0, dy, dz,
                                     t,
                                     tmoms );
                if ( localRet != 0 )
                { return localRet; }

                pend_for_next();
            }

            //! return to next
            float t;
            int tmoms;
            localRet = guessT_tmo( cx + dx, cy, 0, _pLocalServer->mRV, &t, &tmoms );
            if ( localRet != 0 )
            { return localRet; }

            if ( nx < n  )
            {
                localRet = mrgRobotMove( local_vi(),
                                     robot_handle(),
                                     wave_table,
                                     cx + dx *(nx+1), cy, cz,
                                     t, tmoms
                                        );
                if ( localRet != 0 )
                { return localRet; }

                pend_for_next();
            }
        }

        //! home
        post_on_action_home( doc );
    end_pend()

    return localRet;
}

int Let_Service::post_on_action_snakeX( QJsonDocument &doc )
{
    pre_def( IntfRequest );

    deload_double( velocity );
    deload_double( x );     //! +/-1 0
    deload_double( y );     //! +/-1 0
    deload_double( z );     //! +/-1 0

    float dx, dy, dz;

    stepToWorld( var.x, var.y, var.z );

    dx = var.x * _pLocalServer->mdH;
    dy = var.y * _pLocalServer->mdW;
    dz = 0;

    int n, m;

    float cx, cy, cz;
    localRet = mrgGetRobotCurrentPosition( local_vi(),
                                           robot_handle(),
                                           &cx, &cy, &cz );
    if ( localRet != 0 )
    { return localRet; }

    //! expect the n && m
    if ( var.x > 0 )
    {
        n = align_count( _pLocalServer->mH - cx, _pLocalServer->mdH );
    }
    else if ( var.x < 0 )
    {
        n = align_count( cx, _pLocalServer->mdH );
    }
    else
    { return -1; }

    if ( var.y > 0 )
    {
        m = 1;
        dy = _pLocalServer->mW - cy;
    }
    else if ( var.y < 0 )
    {
        m = 1;
        dy = -cy;
    }
    else
    { return -1; }

    double dist = distance( 0,0,0,
                            dx,
                            dy,
                            dz );

    float tx = motionTime( qAbs(dx), var.velocity );
    float ty = motionTime( qAbs(dy), var.velocity );
    int tmomsx = motionTimeoutms( qAbs(dx), var.velocity );
    int tmomsy = motionTimeoutms( qAbs(dy), var.velocity );

    begin_pend()
        for( int nx = 0; nx < n; nx++ )
        {
            //! x
            localRet = mrgRobotRelMove( local_vi(),
                                 robot_handle(),
                                 wave_table,
                                 dx, 0, dz,
                                 tx,
                                 tmomsx );
            if ( localRet != 0 )
            { return localRet; }

            pend_for_next();

            //! y
            localRet = mrgRobotRelMove( local_vi(),
                                 robot_handle(),
                                 wave_table,
                                 0, dy * ( (nx % 2) ? -1 : 1 ), dz,
                                 ty,
                                 tmomsy );
            if ( localRet != 0 )
            { return localRet; }

            pend_for_next();
        }

        post_on_action_home( doc );
    end_pend()

    return localRet;
}

int Let_Service::post_on_action_snakeY( QJsonDocument &doc )
{
    pre_def( IntfRequest );

    deload_double( velocity );
    deload_double( x );     //! +/-1 0
    deload_double( y );     //! +/-1 0
    deload_double( z );     //! +/-1 0

    float dx, dy, dz;

    stepToWorld( var.x, var.y, var.z );

    dx = var.x * _pLocalServer->mdH;
    dy = var.y * _pLocalServer->mdW;
    dz = 0;

    int n, m;

    float cx, cy, cz;
    localRet = mrgGetRobotCurrentPosition( local_vi(),
                                           robot_handle(),
                                           &cx, &cy, &cz );
    if ( localRet != 0 )
    { return localRet; }

    //! expect the n && m
    if ( var.x > 0 )
    {
        n = 1;
        dx = _pLocalServer->mH - cx;
    }
    else if ( var.x < 0 )
    {
        n = 1;
        dx = - cx;
    }
    else
    { return -1; }

    if ( var.y > 0 )
    {
        m = align_count( _pLocalServer->mW - cy, _pLocalServer->mdW );
    }
    else if ( var.y < 0 )
    {
        m = align_count( cy, _pLocalServer->mdW );
    }
    else
    { return -1; }

    double dist = distance( 0,0,0,
                            dx,
                            dy,
                            dz );

    float tx = motionTime( qAbs(dx), var.velocity );
    float ty = motionTime( qAbs(dy), var.velocity );
    int tmomsx = motionTimeoutms( qAbs(dx), var.velocity );
    int tmomsy = motionTimeoutms( qAbs(dy), var.velocity );

    begin_pend()
        for( int my = 0; my < m; my++ )
        {
            //! y
            localRet = mrgRobotRelMove( local_vi(),
                                 robot_handle(),
                                 wave_table,
                                 0, dy, dz,
                                 ty,
                                 tmomsy );
            if ( localRet != 0 )
            { return localRet; }

            pend_for_next();

            //! x
            localRet = mrgRobotRelMove( local_vi(),
                                 robot_handle(),
                                 wave_table,
                                 dx * ( (my % 2) ? -1 : 1 ), 0, dz,
                                 tx,
                                 tmomsx );
            if ( localRet != 0 )
            { return localRet; }

            pend_for_next();
        }
        //! home
        post_on_action_home( doc );
    end_pend()

    return localRet;
}

int Let_Service::post_on_action_slope( QJsonDocument &doc )
{
    pre_def( IntfRequest );

    deload_double( velocity );
    deload_double( x );
    deload_double( y );
    deload_double( z );

    int n, m;

    float cx, cy, cz;
    localRet = mrgGetRobotCurrentPosition( local_vi(),
                                           robot_handle(),
                                           &cx, &cy, &cz );
    if ( localRet != 0 )
    { return localRet; }

    cz = _pLocalServer->mZAxes.value();

    toWorld( var.x, var.y, var.z );

    //! expect the n && m
    n = align_count( var.x - cx, _pLocalServer->mdH );
    m = align_count( var.y - cy, _pLocalServer->mdW );

    //! select the min
    int slice = qMin( qAbs(n), qAbs(m) );
    if ( slice < 1 )
    { return -1; }

    float dx, dy, dz;
    dx = (var.x-cx)/slice;
    dy = (var.y-cy)/slice;
//    dz = (var.z-cz)/slice;
    dz = 0;

    double dist = distance( 0,0,0,
                            dx,
                            dy,
                            dz );

    float t = motionTime( dist, var.velocity );
    int tmoms = motionTimeoutms( dist, var.velocity );

    begin_pend()

        for ( int i = 1; i <= slice; i++ )
        {
            {
                localRet = mrgRobotMove( local_vi(),
                                     robot_handle(),
                                     wave_table,
                                     cx + i * dx,
                                     cy + i * dy,
                                     0,
                                     t,
                                     tmoms );
                if ( localRet != 0 )
                { return localRet; }

                //! \note the zaxes
//                localRet = _pLocalServer->mZAxes.move( wave_table, cz + i * dz, var.velocity );
//                if ( localRet != 0 )
//                { return localRet; }

                pend_for_next();
            }
        }

        post_on_action_home( doc );
    end_pend()

    return localRet;
}

int Let_Service::on_query( QJsonDocument &doc )
{
    pre_def(IntfQuery);

    deload_string( item );

    if ( var.item == "status" )
    {
         query_( on_q_status );
    }
    else if ( var.item == "pose" )
    {
        query_( on_q_pose );
    }
    else if ( var.item == "config" )
    {
        query_( on_q_config );
    }
    else
    { }

    return localRet;
}

int Let_Service::on_q_status( QJsonDocument &doc )
{
    pre_def( IntfStatus );

    var.status = "exception_stopd";

    char states[128];

    do
    {
        //! pending
        if ( _pLocalServer->status() == MAppServer::state_pending )
        {
            var.status = "pending";
            break;
        }

        //! pending
        if ( _pLocalServer->status() == MAppServer::state_working )
        {
            var.status = "running";
            break;
        }

        //! raw status
        localRet = mrgGetRobotStates( local_vi(),
                          robot_handle(),
                          wave_table,
                          states );
        if ( localRet != 0 )
        {}
        else if ( QString( states ).toLower() == "idle"
                  || QString( states ).toLower() == "stop" )
        {
            var.status = "stoped";
        }
        else
        {
            var.status = "running";
        }
    }while( false );

    json_obj( command );
    json_obj( status );

    doc.setObject( obj );

    return 0;
}

int Let_Service::on_q_pose( QJsonDocument &doc )
{
    pre_def( IntfPose );

    float x, y, z;
    localRet = mrgGetRobotCurrentPosition( local_vi(),
                                           robot_handle(),
                                           &x, &y, &z );

    if ( localRet != 0 )
    { return localRet; }

    //! z axes
    z = _pLocalServer->mZAxes.value();

    toTcp( x, y, z );

    var.x = x;
    var.y = y;
    var.z = z;

    //! \todo vx,vy,vz
    var.vx = 0;
    var.vy = 0;
    var.vz = 0;

    json_obj( command );

    json_obj( x );
    json_obj( y );
    json_obj( z );
    json_obj( vx );
    json_obj( vy );
    json_obj( vz );

    doc.setObject( obj );

    return 0;
}

int Let_Service::on_config( QJsonDocument &doc )
{
    pre_def( IntfConfig );

    deload_string( item );

    if( var.item == "rst" )
    { localRet = on_config_rst( doc); }
    else if( var.item == "origin" )
    { localRet = on_config_origin( doc); }
    else if( var.item == "dir" )
    { localRet = on_config_dir( doc); }
    else if ( var.item == "whz" )
    { localRet = on_config_whd(doc); }
    else if ( var.item == "dwdhdz")
    { localRet = on_config_dwdhdd(doc); }
    else if ( var.item == "rv")
    { localRet = on_config_rv(doc); }
    else if ( var.item == "gap")
    { localRet = on_config_gap(doc); }
    else
    {}

    //! save
    if ( localRet == 0 )
    {
        localRet = _pLocalServer->saveConfig();
    }

    return localRet;
}

int Let_Service::on_config_rst( QJsonDocument &doc )
{
    pre_def( IntfConfig );

    _pLocalServer->rst();

    return 0;
}

int Let_Service::on_config_origin( QJsonDocument &doc )
{
    pre_def( IntfConfig );

    deload_double( x );
    deload_double( y );
    deload_double( z );

    //! x, y, z must > 0

    _pLocalServer->mOx = var.x;
    _pLocalServer->mOy = var.y;
    _pLocalServer->mOz = var.z;

    return 0;
}

int Let_Service::on_config_dir( QJsonDocument &doc )
{
    pre_def( IntfConfig );

    deload_int( dirx );
    deload_int( diry );
//    deload_double( z );

    //! x, y, z must > 0

    _pLocalServer->mDirx = var.dirx > 0 ? 1 : -1;
    _pLocalServer->mDiry = var.diry > 0 ? 1 : -1;
//    _pLocalServer->mOz = var.z;

    return 0;
}

int Let_Service::on_config_whd( QJsonDocument &doc )
{
    pre_def( IntfConfig );

    deload_double( w );
    deload_double( h );
    deload_double( d );

    _pLocalServer->mW = var.w;
    _pLocalServer->mH = var.h;
    _pLocalServer->mZ = var.d;

    return 0;
}
int Let_Service::on_config_dwdhdd( QJsonDocument &doc )
{
    pre_def( IntfConfig );

    deload_double( dw );
    deload_double( dh );
    deload_double( dd );

    _pLocalServer->mdW = var.dw;
    _pLocalServer->mdH = var.dh;
    _pLocalServer->mdZ = var.dd;

    return 0;
}
int Let_Service::on_config_rv( QJsonDocument &doc )
{
    pre_def( IntfConfig );

    deload_double( rv );

    _pLocalServer->mRV = var.rv;

    return 0;
}

int Let_Service::on_config_gap( QJsonDocument &doc )
{
    pre_def( IntfConfig );

    deload_double3s( gap, gapspeed, zhomespeed );

    _pLocalServer->mZGap = var.gap;
    _pLocalServer->mZGapSpeed = var.gapspeed;
    _pLocalServer->mZHomeSpeed = var.zhomespeed;

    return 0;
}

int Let_Service::on_q_config( QJsonDocument &doc )
{
    pre_def( IntfConfig );

    var.x = _pLocalServer->mOx;
    var.y = _pLocalServer->mOy;
    var.z = _pLocalServer->mOz;

    var.dirx = pLocalServer->mDirx;
    var.diry = pLocalServer->mDiry;

    var.w = _pLocalServer->mW;
    var.h = _pLocalServer->mH;
    var.d = _pLocalServer->mZ;

    var.dw = _pLocalServer->mdW;
    var.dh = _pLocalServer->mdH;
    var.dd = _pLocalServer->mdZ;

    var.rv = _pLocalServer->mRV;

    var.gap = _pLocalServer->mZGap;
    var.gapspeed = _pLocalServer->mZGapSpeed;
    var.zhomespeed = _pLocalServer->mZHomeSpeed;

    json_obj( command );
    json_obj3s( x, y, z );
    json_obj2s( dirx,diry );
    json_obj3s( w, h, d );
    json_obj3s( dw, dh, dd );
    json_obj( rv );

    json_obj3s( gap, gapspeed, zhomespeed );

    doc.setObject( obj );

    return 0;
}

int Let_Service::guessT_tmo( float x, float y, float z, float v, float *pt, int *pTmo, float *pDist )
{
    check_connect();

    int localRet;

    float cx, cy, cz;
    localRet = mrgGetRobotCurrentPosition( local_vi(),
                                           robot_handle(),
                                           &cx, &cy, &cz );
    if ( localRet != 0 )
    { return localRet; }

    double dist = distance( cx,cy,cz,
                            x,
                            y,
                            z );
    if ( NULL != pDist )
    { *pDist = dist; }

    *pt = motionTime( dist, v );
    *pTmo = motionTimeoutms( dist, v );

    return 0;
}

void Let_Service::toWorld( float &x, float &y, float &z )
{
    Q_ASSERT( NULL != m_pServer );
    api_server *pLocalServer = (api_server*)m_pServer;

    x = x * pLocalServer->mDirx + pLocalServer->mOx;
    y = y * pLocalServer->mDiry + pLocalServer->mOy;
//    z = z + pLocalServer->mOz;
}

void Let_Service::toTcp( float &x, float &y, float &z )
{
    Q_ASSERT( NULL != m_pServer );
    api_server *pLocalServer = (api_server*)m_pServer;

    x = (x - pLocalServer->mOx) * pLocalServer->mDirx;
    y = (y - pLocalServer->mOy ) * pLocalServer->mDiry;
//    z = z - pLocalServer->mOz;
}

void Let_Service::toWorld( double &x, double &y, double &z )
{
    Q_ASSERT( NULL != m_pServer );
    api_server *pLocalServer = (api_server*)m_pServer;

    x = x * pLocalServer->mDirx + pLocalServer->mOx;
    y = y * pLocalServer->mDiry + pLocalServer->mOy;

}

void Let_Service::toTcp( double &x, double &y, double &z )
{
    Q_ASSERT( NULL != m_pServer );
    api_server *pLocalServer = (api_server*)m_pServer;

    x = (x - pLocalServer->mOx) * pLocalServer->mDirx;
    y = (y - pLocalServer->mOy ) * pLocalServer->mDiry;
//    z = z - pLocalServer->mOz;
}

void Let_Service::stepToWorld( float &x, float &y, float &z )
{
    Q_ASSERT( NULL != m_pServer );
    api_server *pLocalServer = (api_server*)m_pServer;

    x = x * pLocalServer->mDirx;
    y = y * pLocalServer->mDiry;
}
void Let_Service::stepToWorld( double &x, double &y, double &z )
{
    Q_ASSERT( NULL != m_pServer );
    api_server *pLocalServer = (api_server*)m_pServer;

    x = x * pLocalServer->mDirx;
    y = y * pLocalServer->mDiry;
}
