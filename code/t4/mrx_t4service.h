#ifndef MRX_T4SERVICE_H
#define MRX_T4SERVICE_H

#include "mappservice.h"

class MRX_T4Service : public MAppService
{
public:
    MRX_T4Service( qintptr ptr, QObject *parent = nullptr );
    ~MRX_T4Service();
public:
    virtual bool onUserEvent(QEvent *pEvent);
public:
    virtual void attachServer( MAppServer *pServer );

protected:
    int on_ack_proc( QJsonDocument &doc );

    int on_step_proc(  QJsonDocument &doc );
    int post_on_step_proc(  QJsonDocument &doc );

    int on_joint_step_proc(  QJsonDocument &doc );
    int post_on_joint_step_proc(  QJsonDocument &doc );

    int on_action_proc(  QJsonDocument &doc );
    int post_on_action_proc(  QJsonDocument &doc );

    int on_indicator_proc(  QJsonDocument &doc );
    int post_on_indicator_proc(  QJsonDocument &doc );

    int on_add_proc(  QJsonDocument &doc );
    int post_on_add_proc(  QJsonDocument &doc );

    int on_query_proc(  QJsonDocument &doc );
    int on_link_status_proc(  QJsonDocument &doc );
    int on_link_status_proc_q(  QJsonDocument &doc );

    int on_device_status_proc(  QJsonDocument &doc );
    int on_exception_proc(  QJsonDocument &doc );
    int on_pose_proc(  QJsonDocument &doc );
    int on_parameter_proc(  QJsonDocument &doc );

    int on_dataset_proc(  QJsonDocument &doc );
    int on_meta_proc(  QJsonDocument &doc );
    int on_config_proc(  QJsonDocument &doc );
    int post_on_config_proc(  QJsonDocument &doc );

    int on_config_proc_q(  QJsonDocument &doc );

protected:
    int guessTmo( int joint, float dist, float speed );
    float alignP360( float p );
    float alignN360( float p );
};

#endif // MRX_T4SERVICE_H
