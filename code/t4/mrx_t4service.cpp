#include "mrx_t4service.h"
#include "../mydebug.h"
#include "t4para.h"

#include "mrx_t4server.h"
#include "../device/MegaGateway.h"

//#define SELF_SERVER()   (MRX_T4Server*)m_pServer

#define check_connect() \
Q_ASSERT ( m_pServer != NULL ); \
MRX_T4Server *_pLocalServer;\
_pLocalServer = dynamic_cast<MRX_T4Server*>(m_pServer);


MRX_T4Service::MRX_T4Service( qintptr ptr, QObject *parent ) : MAppService( ptr, parent )
{   
    //! fill map
    mProcMap.insert( QString("step"), (MAppService::P_PROC)&MRX_T4Service::on_step_proc);
    mProcMap.insert( QString("joint step"), (MAppService::P_PROC)&MRX_T4Service::on_joint_step_proc);
    mProcMap.insert( QString("action"), (MAppService::P_PROC)&MRX_T4Service::on_action_proc);
    mProcMap.insert( QString("indicator"), (MAppService::P_PROC)&MRX_T4Service::on_indicator_proc);
    mProcMap.insert( QString("add"), (MAppService::P_PROC)&MRX_T4Service::on_add_proc);
    mProcMap.insert( QString("query"), (MAppService::P_PROC)&MRX_T4Service::on_query_proc);
    mProcMap.insert( QString("link status"), (MAppService::P_PROC)&MRX_T4Service::on_link_status_proc);
    mProcMap.insert( QString("device status"), (MAppService::P_PROC)&MRX_T4Service::on_device_status_proc);
    mProcMap.insert( QString("exception"), (MAppService::P_PROC)&MRX_T4Service::on_exception_proc);
    mProcMap.insert( QString("pose"), (MAppService::P_PROC)&MRX_T4Service::on_pose_proc);
}

void MRX_T4Service::on_step_proc( const QJsonObject &obj )
{
    Q_ASSERT ( m_pServer != NULL );
    MRX_T4Server *_pLocalServer;
    _pLocalServer = dynamic_cast<MRX_T4Server*>(m_pServer);

    logDbg()<<obj;
    logDbg()<<_pLocalServer->deviceVi();
}
void MRX_T4Service::on_joint_step_proc( const QJsonObject &obj )
{
    logDbg()<<obj;
}
void MRX_T4Service::on_action_proc( const QJsonObject &obj )
{
    logDbg()<<obj;

    if ( obj.contains("action") )
    {}
    else
    { return; }

    check_connect();

    QString action = obj.value( "item" ).toString();
    if (  action == "home" )
    {
        int ret;
        ret = mrgRobotGoHome( _pLocalServer->deviceVi(),
                        _pLocalServer->robotHandle(), 5000 );
        logDbg()<<ret;
    }
}
void MRX_T4Service::on_indicator_proc( const QJsonObject &obj )
{
    logDbg()<<obj;
}
void MRX_T4Service::on_add_proc( const QJsonObject &obj )
{
    logDbg()<<obj;
}
void MRX_T4Service::on_query_proc( const QJsonObject &obj )
{
    logDbg()<<obj;
}
void MRX_T4Service::on_link_status_proc( const QJsonObject &obj )
{
    logDbg()<<obj;
}
void MRX_T4Service::on_device_status_proc( const QJsonObject &obj )
{
    logDbg()<<obj;
}
void MRX_T4Service::on_exception_proc( const QJsonObject &obj )
{
    logDbg()<<obj;
}
void MRX_T4Service::on_pose_proc( const QJsonObject &obj )
{
    logDbg()<<obj;
}
