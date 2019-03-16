#ifndef MRX_T4SERVICE_H
#define MRX_T4SERVICE_H

#include "mappservice.h"

class MRX_T4Service : public MAppService
{
public:
    MRX_T4Service( qintptr ptr, QObject *parent = nullptr );

protected:
    int on_ack_proc(  QJsonObject &obj );
    int on_step_proc(  QJsonObject &obj );
    int on_joint_step_proc(  QJsonObject &obj );
    int on_action_proc(  QJsonObject &obj );

    int on_indicator_proc(  QJsonObject &obj );
    int on_add_proc(  QJsonObject &obj );
    int on_query_proc(  QJsonObject &obj );
    int on_link_status_proc(  QJsonObject &obj );

    int on_device_status_proc(  QJsonObject &obj );
    int on_exception_proc(  QJsonObject &obj );
    int on_pose_proc(  QJsonObject &obj );
    int on_parameter_proc(  QJsonObject &obj );

    int on_dataset_proc(  QJsonObject &obj );
    int on_meta_proc(  QJsonObject &obj );
    int on_config_proc(  QJsonObject &obj );
};

#endif // MRX_T4SERVICE_H
