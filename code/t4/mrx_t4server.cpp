#include "mrx_t4server.h"
#include "mrx_t4service.h"

#include "../mydebug.h"
#include "../device/MegaGateway.h"

MRX_T4Server::MRX_T4Server( QObject *parent ) : MAppServer( parent )
{
//    mAddr = "TCPIP0::169.254.1.2::INST0::INSTR";
    mAddr = "TCPIP0::169.254.1.2::inst0::INSTR";
}

void MRX_T4Server::incomingConnection(qintptr socketDescriptor)
{
    //! has opened
    if ( isOpend() )
    {}
    else
    {
        logDbg()<<"fail open the device";
        return;
    }

    MAppService *thread = new MRX_T4Service(socketDescriptor, 0 );
    thread->moveToThread( thread );
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->attachServer( this );

    thread->start();
}

int MRX_T4Server::open()
{
    int ret;

    int vi;
    vi = mrgOpenGateWay( mAddr.toLatin1().data(), 500 );
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

    return 0;
}

void MRX_T4Server::close()
{
    if ( mVi > 0 )
    {
        mrgCloseGateWay( mVi );
        mVi = 0;
    }

    return;
}
