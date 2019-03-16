#ifndef MRX_T4SERVICE_H
#define MRX_T4SERVICE_H

#include "mappservice.h"

class MRX_T4Service : public MAppService
{
public:
    MRX_T4Service( qintptr ptr, QObject *parent = nullptr );

protected:
    void on_step_proc( const QJsonObject &obj );
    void on_joint_step_proc( const QJsonObject &obj );
    void on_action_proc( const QJsonObject &obj );
    void on_indicator_proc( const QJsonObject &obj );
    void on_add_proc( const QJsonObject &obj );
    void on_query_proc( const QJsonObject &obj );
    void on_link_status_proc( const QJsonObject &obj );
    void on_device_status_proc( const QJsonObject &obj );
    void on_exception_proc( const QJsonObject &obj );
    void on_pose_proc( const QJsonObject &obj );
};

#endif // MRX_T4SERVICE_H
