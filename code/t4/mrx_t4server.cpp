#include "mrx_t4server.h"
#include "mrx_t4tcpserver.h"

#include "../mydebug.h"
#include "MegaGateway.h"

MRX_T4Server::MRX_T4Server( int portBase, int cnt, QObject *pParent ) : MAppServer( portBase, cnt, pParent )
{

#ifndef _RASPBERRY
//    mAddr = "TCPIP0::192.168.1.54::inst0::INSTR";        //! descriptor in case sensitive
    mAddr = "TCPIP0::192.168.1.234::inst0::INSTR";        //! descriptor in case sensitive
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
    vi = mrgOpenGateWay( mAddr.toLatin1().data(), 2000 );logDbg()<<vi;
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

        //! \todo update para
        char idns[128];
        ret = mrgGateWayIDNQuery( mVi, idns );
        if ( ret == 0 )
        {
            QString strIdns = idns;

            QStringList idnList = strIdns.split(',');
            if ( idnList.size() < 4 )
            { return -1; }

            mSn = idnList.at( 2 );
            logDbg()<<strIdns;
            logDbg()<<mSn;
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
        if ( mWorkings.at(i)->isRunning() )
        {
//            logDbg();
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
