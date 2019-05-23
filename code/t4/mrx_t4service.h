#ifndef MRX_T4SERVICE_H
#define MRX_T4SERVICE_H

#include "mappservice.h"

class MRX_T4Service : public MAppService
{
public:
    MRX_T4Service( qintptr ptr, quint16 port, QObject *parent = nullptr );
    virtual ~MRX_T4Service();

public:
    virtual bool onUserEvent(QEvent *pEvent);
public:
    virtual void attachServer( MAppServer *pServer );

protected:
    virtual int _on_preProc( QJsonDocument &doc );
    virtual int _on_postProc( QJsonDocument &doc );

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
    int on_device_status_raw_proc( QJsonDocument &doc );
    int on_controller_status_proc( QJsonDocument &doc );

    int on_exception_proc(  QJsonDocument &doc );
    int on_pose_proc(  QJsonDocument &doc );

    int on_cpose_proc( QJsonDocument &doc );
    int on_cjoint_proc( QJsonDocument &doc );
    int on_parameter_proc(  QJsonDocument &doc );

    int on_dataset_proc(  QJsonDocument &doc );
    int on_meta_proc(  QJsonDocument &doc );
    int on_config_proc(  QJsonDocument &doc );
    int post_on_config_proc(  QJsonDocument &doc );

    int on_config_proc_q(  QJsonDocument &doc );

    //! motion
    int on_movej_proc( QJsonDocument &doc );
    int post_on_movej_proc( QJsonDocument &doc );

    int on_move_proc( QJsonDocument &doc );
    int post_on_move_proc( QJsonDocument &doc );

    int on_movel_proc( QJsonDocument &doc );
    int post_on_movel_proc( QJsonDocument &doc );

    int on_setio( QJsonDocument &doc );
    int on_seto( QJsonDocument &doc );

    int on_getio( QJsonDocument &doc );
    int on_getdi( QJsonDocument &doc, QList<int> &ports );
    int on_getdo( QJsonDocument &doc, QList<int> &ports );

    int on_execute( QJsonDocument &doc );
    int post_on_execute( QJsonDocument &doc );

protected:
    int rawStatus( QString &status );

    int guessTmo( int joint, float dist, float speed );
    int guessTTmo( float x, float y, float z,
                   float v,
                   float *pT, float *pTmo );

    double eulaDistance( double x, double y, double z,
                         double x1, double y1, double z1 );

    float alignP360( float p );
    float alignN360( float p );
};

#endif // MRX_T4SERVICE_H
