#ifndef LETPARA_H
#define LETPARA_H

#include "../intfserverpara.h"

class LetPara : public IntfServerPara
{
public:
    LetPara();

public:
    void rst();

    int saveConfig();
    int loadConfig();

public:
    float mOx, mOy, mOz;
    int mDirx, mDiry;

    float mW, mH, mZ;
    float mdW, mdH, mdZ;
    float mRV;

    float mZGap, mZGapSpeed, mZHomeSpeed;
};

#endif // LETPARA_H
