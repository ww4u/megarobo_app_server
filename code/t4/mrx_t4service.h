#ifndef MRX_T4SERVICE_H
#define MRX_T4SERVICE_H

#include "mappservice.h"

class MRX_T4Service : public MAppService
{
    Q_OBJECT

public:
    MRX_T4Service( qintptr ptr, quint16 port, QObject *parent = nullptr );
    virtual ~MRX_T4Service();

public:
    virtual bool onUserEvent(QEvent *pEvent);
public:
    virtual void attachServer( MAppServer *pServer );

protected:
    virtual int on_refreshtimeout();

    virtual int _on_preProc( QJsonDocument &doc );
    virtual int _on_postProc( QJsonDocument &doc );

    int on_ack_proc( QJsonDocument &doc );

    int on_step_proc(  QJsonDocument &doc );
    int post_on_step_proc(  QJsonDocument &doc );

    int on_joint_step_proc(  QJsonDocument &doc );
    int post_on_joint_step_proc(  QJsonDocument &doc );

    //! n step
    int joint_stepN_proc(  QJsonDocument &doc );
    //! step distance
    int joint_stepD_proc(  QJsonDocument &doc );

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
    int on_ctorque_proc( QJsonDocument &doc );
    int on_ccurrent_proc( QJsonDocument &doc );

    int on_parameter_proc(  QJsonDocument &doc );

    int on_dataset_proc(  QJsonDocument &doc );
    int on_meta_proc(  QJsonDocument &doc );
    int on_config_proc(  QJsonDocument &doc );
    int post_on_config_proc(  QJsonDocument &doc );

    int on_config_proc_q(  QJsonDocument &doc );

    int on_file_proc( QJsonDocument &doc );

    int on_file_write( QJsonDocument &doc );
    int on_file_read( QJsonDocument &doc );
    int on_file_delete( QJsonDocument &doc );
    int on_file_size( QJsonDocument &doc );

    int on_dir_proc( QJsonDocument &doc );

    int on_dir_create( QJsonDocument &doc );
    int on_dir_delete( QJsonDocument &doc );
    int on_dir_size( QJsonDocument &doc );
    int on_dir_list( QJsonDocument &doc );

    //! motion
    int on_alignj_proc( QJsonDocument &doc );
    int post_on_alignj_proc( QJsonDocument &doc );

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

    int _getio( QJsonDocument &doc, QList<int> &ports,
                int shift,
                int from, QList<int> &portVals );

    int on_execute( QJsonDocument &doc );
    int on_velset( QJsonDocument &doc );

    int post_on_execute( QJsonDocument &doc );
    int post_on_execute_script( QJsonDocument &doc );
    int post_on_execute_shell( QJsonDocument &doc );

    void switch_shell_dir();
    int run_shell( const QString &fileName, const QStringList &args );

protected:
    float scaleT( float t );

    int rawStatus( QString &status );

    int guessTmoT( int joint, float angle, float t );
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
