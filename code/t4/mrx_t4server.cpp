#include "mrx_t4server.h"
#include "mrx_t4tcpserver.h"

#include "../mydebug.h"
#include "MegaGateway.h"
#include "../syspara.h"
MRX_T4Server::MRX_T4Server( int portBase, int cnt, QObject *pParent ) : MAppServer( portBase, cnt, pParent )
{
    setServerName("MRX-T4");
#ifndef _RASPBERRY
//    mAddr = "TCPIP0::192.168.1.54::inst0::INSTR";        //! descriptor in case sensitive
//    mAddr = "TCPIP0::192.168.1.122::inst0::INSTR";
    mAddr = "TCPIP0::169.254.1.2::inst0::INSTR";
//    mAddr = "TCPIP0::192.168.1.159::inst0::INSTR";
//    mAddr = "TCPIP0::192.168.1.231::inst0::INSTR";
#else
    mAddr = "TCPIP0::127.0.0.1::inst0::INSTR";
#endif

    //! eop
    setRecvEOP( "\u00A9" );
}

int MRX_T4Server::start()
{
    int ret;

    ret = MAppServer::start();
    if ( ret != 0 )
    { return ret; }

    //! load the data
    ret = load();

    //! create a few servers
    MRX_T4TcpServer *pServer;
    foreach( quint16 port, mPorts )
    {
        pServer = new MRX_T4TcpServer( this );
        if ( NULL == pServer )
        { return -11; }

        pServer->attachServer( this );

        if ( pServer->start( port ) )
        {}
        else
        { return -12; }

        //! append
        mTcpServers.append( pServer );
    }

    return ret;
}

int MRX_T4Server::open()
{
//#ifdef QT_DEBUG
//    mVi = 1;
//    return 0;
//#endif

    int ret;

    int vi;

    int iMode;
#ifdef WIN32
    iMode = BUS_VXI;
#else
    iMode = BUS_SOCKET;
#endif

    vi = mrgOpenGateWay( iMode, mAddr.toLatin1().data(), 2000 );logDbg()<<vi;
    if ( vi > 0 )
    { mVi = vi; }
    else
    { return -1; }

    do
    {
        int names[16];

        //! \todo names overflow
        ret = mrgGetRobotName( mVi, names );
        if ( ret > 16 || ret < 1 )
        {
            ret = -2;
            break;
        }

        mRobotHandle = names[0];

        //! device handle
        int deviceHandles[16];
        ret = mrgGetRobotDevice( self_robot_var(), deviceHandles );
        if ( ret > 16 || ret < 1 )
        {
            ret = -3;
            break;
        }
        mDeviceHandle = deviceHandles[0];

        //! \note get sn
        char sns[128];
        ret = mrgGetDeviceSerialNumber( mVi, mDeviceHandle, sns );
        if ( ret != 0 )
        {
            ret = -4;
            break;
        }
        mDeviceSNs.append( sns );
        mSn = mDeviceSNs.at( 0 );
        logDbg()<<mDeviceSNs;

        //! \todo update para
        char idns[128];
        ret = mrgGateWayIDNQuery( mVi, idns );
        if ( ret == 0 )
        {
            QString strIdns = idns;

            QStringList idnList = strIdns.split(',');
            if ( idnList.size() < 4 )
            { return -1; }

            logDbg()<<strIdns;
        }

        //! config terminal
        QString baStr = QString("4@%1").arg( deviceHandle() );
        //! \note F2
        ret = mrgRobotToolSet( mVi, robotHandle(), 0, baStr.toLatin1().data() );
        if(ret != 0){
            return -1;
        }

        int dataUpdate = 1;
        if ( sysHasArg("-upload") )
        { dataUpdate = 1; }
        else
        { dataUpdate = 0; }

        //! open sg/se
        for ( int i = 0; i < 5; i++ )
        {
            ret = mrgMRQReportState( mVi, mDeviceHandle, i, 0, dataUpdate );
            if ( ret != 0 )
            {
                break;
            }
        }

    }while( 0 );

    //! get fail
    if ( ret != 0 )
    {
        mrgCloseGateWay( mVi );
        mVi = 0;
    }

    return ret;
}

void MRX_T4Server::close()
{
    if ( mVi > 0 )
    {
        mrgCloseGateWay( mVi );
        mVi = 0;
    }
}

MAppServer::ServerStatus MRX_T4Server::status()
{
    //! \todo the device status

    //! the self status
    for ( int i = 0; i < mWorkings.size(); i++ )
    {
//        if ( mWorkings.at(i)->isRunning() )
        if ( mWorkings.at(i)->isWorking() )
        {
            return state_working;
        }
    }

    return state_idle;
}

void MRX_T4Server::on_dislink()
{
    mbLink = false;
}

bool MRX_T4Server::isLinked()
{
    if ( mbLink )
    { return mServices.size() > 0; }
    else
    { return false; }
}

int MRX_T4Server::load()
{
    int ret;

    ret = loadConfig();
    if ( ret != 0 )
    { return ret; }

    ret = loadDataSet();

    return ret;
}
