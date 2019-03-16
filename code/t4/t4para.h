#ifndef T4PARA_H
#define T4PARA_H

#include <QtCore>
#include "../intfserverpara.h"

class T4Para : public IntfServerPara
{
public:
    T4Para();

public:
    double mCurrent[5];
    double mIdleCurrent[5];

};

#endif // T4PARA_H
