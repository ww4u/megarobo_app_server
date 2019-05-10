#include "let_server.h"
#include "let_tcpserver.h"

#include "../mydebug.h"
#include "MegaGateway.h"

Let_Server::Let_Server( int portBase, int cnt, QObject *pParent ) : MAppServer( portBase, cnt, pParent )
{
    mAddr = "TCPIP0::192.168.1.54::inst0::INSTR";        //! descriptor in case sensitive
}

int Let_Server::start()
{
    int ret;

    //! load the data
    ret = load();

    //! create a few servers
    Let_TcpServer *pServer;
    foreach( quint16 port, mPorts )
    {
        pServer = new Let_TcpServer( this );
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

int Let_Server::open()
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

        ret = mrgGetRobotType( mVi, mRobotHandle );
        if ( ret < 0 )
        { break; }
        logDbg()<<"robot"<<ret;

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

            mSN = idnList.at( 2 );
            logDbg()<<mSN;

            //! attach axes
            mZAxes.attachDevice( mVi, mDeviceHandle );
            mZAxes.attachAxes( 2 );
            mZAxes.setInvert( false );
//            mZAxes.setInvert( true );
//            mZAxes.stop();
//            mZAxes.zero();
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

void Let_Server::close()
{
    if ( mVi > 0 )
    {
        mrgCloseGateWay( mVi );
        mVi = 0;
    }
}

void Let_Server::rst()
{
    LetPara::rst();
}

MAppServer::ServerStatus Let_Server::status()
{
    //! \todo the device status

    //! service pending
    for ( int i = 0; i < mServices.size(); i++ )
    {
        Q_ASSERT( (MAppService*)mServices.at(i) != NULL );
        if ( ( (MAppService*)mServices.at(i) )->isPending() )
        { return state_pending; }
    }

    //! the self status
    for ( int i = 0; i < mWorkings.size(); i++ )
    {
        if ( mWorkings.at(i)->isRunning() )
        {   
            return state_working;
        }
    }

    return state_idle;
}

void Let_Server::on_dislink()
{
//    mbLink = false;
}

bool Let_Server::isLinked()
{
//    if ( mbLink )
//    { return mServices.size() > 0; }
//    else
//    { return false; }
    return true;
}

int Let_Server::load()
{
    int ret;

    ret = loadConfig();
    if ( ret != 0 )
    { return ret; }

    return ret;
}

int Let_Server::zpdir()
{
    return mZGap > 0 ? 1 : -1;
}
