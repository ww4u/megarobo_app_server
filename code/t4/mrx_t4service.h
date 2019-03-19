#ifndef MRX_T4SERVICE_H
#define MRX_T4SERVICE_H

#include "mappservice.h"

class MRX_T4Service : public MAppService
{
public:
    MRX_T4Service( qintptr ptr, QObject *parent = nullptr );



public:
    bool onUserEvent(QEvent *pEvent);

protected:
    int on_ack_proc( QJsonDocument &obj );
    int on_step_proc(  QJsonDocument &obj );
    int on_joint_step_proc(  QJsonDocument &obj );
    int on_action_proc(  QJsonDocument &obj );

    int on_indicator_proc(  QJsonDocument &obj );
    int on_add_proc(  QJsonDocument &obj );
    int on_query_proc(  QJsonDocument &obj );
    int on_link_status_proc(  QJsonDocument &obj );

    int on_device_status_proc(  QJsonDocument &obj );
    int on_exception_proc(  QJsonDocument &obj );
    int on_pose_proc(  QJsonDocument &obj );
    int on_parameter_proc(  QJsonDocument &obj );

    int on_dataset_proc(  QJsonDocument &obj );
    int on_meta_proc(  QJsonDocument &obj );
    int on_config_proc(  QJsonDocument &obj );

    int on_config_proc_q(  QJsonDocument &obj );
    int on_link_status_proc_q(  QJsonDocument &obj );

protected:

};

#endif // MRX_T4SERVICE_H
