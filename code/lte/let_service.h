#ifndef LET_SERVICE_H
#define LET_SERVICE_H

#include "../mappservice.h"

class Let_Service : public MAppService
{
public:
    Let_Service( qintptr ptr, quint16 port, QObject *parent = nullptr );
    virtual ~Let_Service();

public:
    virtual bool onUserEvent(QEvent *pEvent);
public:
    virtual void attachServer( MAppServer *pServer );

protected:
    int on_request( QJsonDocument &doc );

    int on_action_stop( QJsonDocument &doc );
    int on_action_eStop( QJsonDocument &doc );

    int post_on_action_home( QJsonDocument &doc );
    int post_on_action_homez( QJsonDocument &doc );
    int post_on_action_origin( QJsonDocument &doc );
    int post_on_action_to( QJsonDocument &doc );
    int post_on_action_step( QJsonDocument &doc );

    int post_on_action_zigzagX( QJsonDocument &doc );
    int post_on_action_zigzagY( QJsonDocument &doc );

    int post_on_action_snakeX( QJsonDocument &doc );
    int post_on_action_snakeY( QJsonDocument &doc );
    int post_on_action_slope( QJsonDocument &doc );

    int on_query( QJsonDocument &doc );

    int on_q_status( QJsonDocument &doc );
    int on_q_pose( QJsonDocument &doc );

    int on_config( QJsonDocument &doc );
    int on_config_rst( QJsonDocument &doc );
    int on_config_origin( QJsonDocument &doc );
    int on_config_dir( QJsonDocument &doc );
    int on_config_whd( QJsonDocument &doc );
    int on_config_dwdhdd( QJsonDocument &doc );
    int on_config_rv( QJsonDocument &doc );
    int on_config_gap( QJsonDocument &doc );

    int on_q_config( QJsonDocument &doc );

protected:
    int guessT_tmo( float x, float y, float z, float v,
                    float *pt, int *pTmo, float *pDist=NULL );

    void toWorld( float &x, float &y, float &z );
    void toTcp( float &x, float &y, float &z );

    void toWorld( double &x, double &y, double &z );
    void toTcp( double &x, double &y, double &z );

    void stepToWorld( float &x, float &y, float &z );
    void stepToWorld( double &x, double &y, double &z );

    bool isInRegion( float x, float y, float z );
};

#endif // LET_SERVICE_H
