#include "letpara.h"
#include "let_intfseries.h"

#include "../myjson.h"
#include "../mydebug.h"

//! config file
#define config_dir      qApp->applicationDirPath() + "/data/let"
#define data_dir        config_dir

#define config_file     config_dir + "/config.json"
#define data_file       data_dir + "/data.json"


LetPara::LetPara()
{
    rst();
}

void LetPara::rst()
{
    mOx = 0;
    mOy = 0;
    mOz = 0;

    mW = 300;
    mH = 350;
    mZ = 50;

    mdW = 50;
    mdH = 50;
    mdZ = 10;

    mRV = 300;

    mZGap = 1;
    mZGapSpeed = 10;
    mZHomeSpeed = 10;
}

#define assign( a, v )  if ( _r_assign) { v = a; }\
                        else{ a = v; }

int LetPara::saveConfig()
{
    IntfConfig var;

    bool _r_assign = false;
    assign( var.x, mOx );
    assign( var.y, mOy );
    assign( var.z, mOz );

    assign( var.w, mW );
    assign( var.h, mH );
    assign( var.d, mZ );

    assign( var.dw, mdW );
    assign( var.dh, mdH );
    assign( var.dd, mdZ );

    assign( var.rv, mRV );

    assign( var.gap, mZGap );
    assign( var.gapspeed, mZGapSpeed );
    assign( var.zhomespeed, mZHomeSpeed );

    QJsonObject obj;
    json_obj3s( x,y,z );

    json_obj3s( w, h, d );

    json_obj3s( dw, dh, dd );

    json_obj( rv );

    json_obj3s( gap, gapspeed, zhomespeed );

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
int LetPara::loadConfig()
{
    //! load config
    QFile fileIn( config_file );
    if( fileIn.open( QIODevice::ReadOnly) )
    {}
    else
    { return -1; }

    QJsonDocument doc = QJsonDocument::fromJson( fileIn.readAll() );

    fileIn.close();

    IntfConfig var;

    //! convert
    if ( doc.isObject() )
    {
        QJsonObject obj;

        obj = doc.object();  
        deload_double3s( x,y,z );

        deload_double3s( w, h, d );

        deload_double3s( dw, dh, dd );

        deload_double( rv );

        deload_double3s( gap, gapspeed, zhomespeed );
    }
    else
    { return -1; }

    //! recover
    bool _r_assign = true;
    assign( var.x, mOx );
    assign( var.y, mOy );
    assign( var.z, mOz );

    assign( var.w, mW );
    assign( var.h, mH );
    assign( var.d, mZ );

    assign( var.dw, mdW );
    assign( var.dh, mdH );
    assign( var.dd, mdZ );

    assign( var.rv, mRV );

    assign( var.gap, mZGap );
    assign( var.gapspeed, mZGapSpeed );
    assign( var.zhomespeed, mZHomeSpeed );

    return 0;
}
