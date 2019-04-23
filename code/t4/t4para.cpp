#include "t4para.h"

#include "../myjson.h"
#include "../mydebug.h"

//! config file
#define config_dir        qApp->applicationDirPath() + "/data"
#define data_dir        config_dir
#define config_file     config_dir + "/config.json"
#define data_file       data_dir + "/data.json"

T4Para::T4Para()
{
    reset();
}

T4Para::~T4Para()
{
    qDeleteAll( mPoints );
}

void T4Para::reset()
{
    mSn = "SN12345";
    mModel = "MRX-T4";
    mAlias = "myROBOT";
    mbHasHand = true;

    //! tick timeout
    mTickTmo = 15 * 60 * 1000;

    mStep = 10;
    mJointStep = 5;
    mSpeed = 1.0;

    mMaxBodySpeed = 100;    //! mm/s
    mMaxJointSpeed = 100;   //! degree/s

    mLinkStatus = "idle";
}

int T4Para::saveConfig()
{
    Intfconfig var;

    var.timeout = mTickTmo;

    var.step = mStep;
    var.joint_step = mJointStep;
    var.speed = mSpeed;

    var.max_body_speed = mMaxBodySpeed;
    var.max_joint_speed = mMaxJointSpeed;

    QJsonObject obj;
    json_obj( timeout );
    json_obj( step );
    json_obj( joint_step );
    json_obj( speed );

    json_obj( max_body_speed );
    json_obj( max_joint_speed );

    //! save
    QJsonDocument doc( obj );

    if ( 0 == assurePath(config_dir) )
    {}
    else
    { return -1; }

    QFile fileOut( config_file );
    if ( fileOut.open( QIODevice::WriteOnly) )
    {}
    else
    { return -1; }

    QByteArray localAry = doc.toJson();
    if ( localAry.length() != fileOut.write( localAry ) )
    {
        logDbg()<<"write fail";
    }
    else
    {}

    fileOut.close();

    return 0;
}
int T4Para::loadConfig()
{
    //! load config
    QFile fileIn( config_file );
    if( fileIn.open( QIODevice::ReadOnly) )
    {}
    else
    { return -1; }

    QJsonDocument doc = QJsonDocument::fromJson( fileIn.readAll() );

    fileIn.close();

    Intfconfig var;

    //! convert
    if ( doc.isObject() )
    {
        QJsonObject obj;

        obj = doc.object();
        deload_int( timeout );
        deload_double( step );
        deload_double( joint_step );
        deload_double( speed );

        deload_double( max_joint_speed );
        deload_double( max_body_speed )
    }

    //! recover
    mTickTmo = var.timeout;
    mStep = var.step;
    mJointStep = var.joint_step;
    mSpeed = var.speed;
    mMaxJointSpeed = var.max_joint_speed;
    mMaxBodySpeed = var.max_body_speed;

    return 0;
}

int T4Para::saveDataSet()
{
    QJsonArray jArray;

    int ret;
    ret = _dataDataToArray( jArray );
    if ( ret != 0 )
    { return ret; }

    QJsonDocument doc( jArray );
    QByteArray dataAry = doc.toJson();

    //! assure path
    if ( 0 == assurePath(data_dir) )
    {}
    else
    { return -1; }

    //! export
    QFile file( data_file );
    if ( file.open( QIODevice::WriteOnly ) )
    {}
    else
    { return -1; }

    if ( file.write( dataAry) != dataAry.size() )
    {
        file.close();
        return -1;
    }

    file.close();
    return 0;
}

int T4Para::loadDataSet()
{
    //! load config
    QFile fileIn( data_file );
    if( fileIn.open( QIODevice::ReadOnly) )
    {}
    else
    { return -1; }

    QJsonDocument doc = QJsonDocument::fromJson( fileIn.readAll() );
    fileIn.close();

    QList< ObjPoint * > localPoints;
    int ret;
    ret = _loadDataSet( doc, localPoints );
    if ( ret != 0 )
    { return ret; }
    else
    {
        //! recover
        qDeleteAll( mPoints );
        mPoints.clear();

        //! \note shift the data
        mPoints = localPoints;
    }

    return 0;
}

void T4Para::addPoint( const QString name,
               ObjPose &pose )
{
    ObjPoint *pPoint;

    pPoint = new ObjPoint();
    if ( NULL == pPoint )
    { return; }

    pPoint->name = name;
    pPoint->pose = pose;

    mPoints.append( pPoint );
}

int T4Para::_loadDataSet( QJsonDocument &doc,
                          QList< ObjPoint * > &localPoints )
{
//    if ( doc.isObject() )
//    {}
//    else
//    { return -1; }

    if ( doc.isArray() )
    {}
    else
    { return -1; }

    //! value
    QJsonValue itemVal;
    ObjPoint *pPoint;

    QJsonObject itemObj, pointObj;
    for( int i = 0; i < doc.array().size(); i++ )
    {
        itemVal = doc.array().at( i );
        itemObj = itemVal.toObject();

        pPoint = new ObjPoint();
        if ( NULL == pPoint )
        { return -1; }

        pPoint->name = itemObj.value( "name" ).toString();

        pointObj = itemObj.value( "pose" ).toObject();

        pPoint->pose.x = pointObj.value( "x" ).toDouble();
        pPoint->pose.y = pointObj.value( "y" ).toDouble();
        pPoint->pose.z = pointObj.value( "z" ).toDouble();
        pPoint->pose.w = pointObj.value( "w" ).toDouble();
        pPoint->pose.h = pointObj.value( "h" ).toDouble();

        localPoints.append( pPoint );
    }

    return 0;
}

int T4Para::assurePath( const QString &path )
{
    //! create path
    QDir dir( path );
    if ( dir.exists() )
    {}
    else
    {
        if ( dir.mkpath( path) )
        {}
        else
        {
            return -1;
        }
    }

    return 0;
}

int T4Para::_dataDataToArray( QJsonArray &objAry )
{
    ObjPoint *pPoint;
    for ( int i = 0; i < mPoints.size(); i++ )
    {
        QJsonObject obj, pointObj;

        pPoint = mPoints.at( i );
        obj.insert( "name", pPoint->name );

        pointObj.insert( "x", pPoint->pose.x );
        pointObj.insert( "y", pPoint->pose.y );
        pointObj.insert( "z", pPoint->pose.z );
        pointObj.insert( "w", pPoint->pose.w );
        pointObj.insert( "h", pPoint->pose.h );

        obj.insert( "pose", pointObj );

        objAry<<obj;
    }

    return 0;
}
