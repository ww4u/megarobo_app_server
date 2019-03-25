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

    QString mLinkStatus;

    double mStep;
    double mJointStep;
    double mSpeed;      //! percent

    double mMaxJointSpeed, mMaxBodySpeed;

                        //! points
    QList<ObjPoint *> mPoints;
};

#endif // T4PARA_H
