#ifndef T4PARA_H
#define T4PARA_H

#include <QtCore>
#include <QtCore>

#include "../intfserverpara.h"

#include "intfseries.h"

class T4Para : public IntfServerPara
{
public:
    T4Para();
    ~T4Para();

public:
    void reset();

    void setStep( double step );
    void setJStep( double step );
    void setSpeed( double spd );

    void setVelScale( double scale );
    void setMaxBodySpeed( double spd );

    void setAutoAcc( double acc );

    double localStep();
    double localJStep();

    double localSpeedRatio();

    int saveConfig();
    int loadConfig();

    int saveDataSet();
    int loadDataSet();

    void addPoint( const QString name,
                   ObjPose &pose );

protected:
    int _loadDataSet( QJsonDocument &doc,
                      QList< ObjPoint * > &localPoints);

public:
    int _dataDataToArray( QJsonArray &obj );

public:
    //! static
    QString mSn, mAlias, mModel;
    bool mbHasHand;

    double mCurrent[5];
    double mIdleCurrent[5];

    int mTickTmo;

    double mStep;       //! 1~100
    double mJointStep;  //! 1~100
    double mSpeed;      //! 1~100

    double mMaxJointSpeed, mMaxBodySpeed;
    double mMaxJointStep, mMaxBodyStep;
                        //! points
    QList<ObjPoint *> mPoints;

    double mVelScale;   //! 1-100
    double mAutoAcc;    //! 1~100

    //! dynamic
    bool mbLink;
};

#endif // T4PARA_H
