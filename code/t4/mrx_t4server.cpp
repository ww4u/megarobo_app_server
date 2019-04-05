#include "mrx_t4server.h"
#include "mrx_t4tcpserver.h"

#include "../mydebug.h"
#include "MegaGateway.h"

MRX_T4Server::MRX_T4Server( int portBase, int cnt, QObject *pParent ) : MAppServer( pParent )
{
    mPorts.clear();
    for ( int i = 0; i < cnt; i++ )
    {
        mPorts<<(portBase+i);
    }

#ifndef _RASPBERRY
    mAddr = "TCPIP0::169.254.1.2::inst0::INSTR";        //! descriptor in case sensitive    
//    mAddr = "TCPIP0::169.254.142.71::inst0::INSTR";
#else
    mAddr = "TCPIP0::127.0.0.1::inst0::INSTR";
#endif
}

int MRX_T4Server::start()
{
    int ret;

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
    vi = mrgOpenGateWay( mAddr.toLatin1().data(), 500 );logDbg()<<vi;
    if ( vi > 0 )
    { mVi = vi; }
    else
    { return -1; }

    int names[16];

    //! \todo names overflow
    ret = mrgGetRobotName( mVi, names );
    if ( ret > 16 || ret < 1 )
    { return -1; }

    mRobotHandle = names[0];

    //! device handle
    int deviceHandles[16];
    ret = mrgGetRobotDevice( self_robot_var(), deviceHandles );
    if ( ret > 16 || ret < 1 )
    { return -1; }
    mDeviceHandle = deviceHandles[0];

    //! \todo update para
    char idns[128];
    ret = mrgGateWayIDNQuery( mVi, idns );
    if ( ret == 0 )
    {
        mSn = QString( idns );
        logDbg()<<mSn;
    }

    return 0;
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
        if ( mWorkings.at(i)->isRunning() )
        { return state_working; }
    }

    return state_idle;
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
