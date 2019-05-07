#ifndef SERVERINTF_H
#define SERVERINTF_H

#include <QtCore>

#define self_robot_var()  (ViSession)deviceVi(),robotHandle()
#define self_device_var()    (ViSession)deviceVi(),deviceHandle()

class ServerIntf
{
public:
    ServerIntf();

public:
    int deviceVi();
    int robotHandle();
    int deviceHandle();

    bool isOpend();

public:
    virtual int open();
    virtual void close();
    virtual void rst();

protected:
    QString mAddr;
    int mVi;

    int mRobotHandle;
    int mDeviceHandle;

    QString mSN, mFirmwareVer, mMechanicalVer;
};

#endif // SERVERINTF_H
