#include "serverintf.h"

ServerIntf::ServerIntf()
{
    mVi = 0;
    mRobotHandle = 0;
    mDeviceHandle = 0;
}

int ServerIntf::deviceVi()
{ return mVi; }

int ServerIntf::robotHandle()
{ return mRobotHandle; }
int ServerIntf::deviceHandle()
{ return mDeviceHandle; }

bool ServerIntf::isOpend()
{ return mVi > 0; }

int ServerIntf::open()
{
    return 0;
}
void ServerIntf::close()
{}
