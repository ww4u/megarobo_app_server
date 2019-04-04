#ifndef SYSPARA_H
#define SYSPARA_H

#include <QFile>

class SysPara
{
public:
    SysPara();
    ~SysPara();

public:
    void logFile( QFile *pFile, const QString &content );
    QFile *open( const QString &name );

public:
    int mArgc;
    char **mArgv;

    QFile *m_pInputFile;
    QFile *m_pOutputFile;
};

void sysSetArg( int argc, char **argv );
bool sysHasArg( const QString &arg );

void sysLogIn( const QString &intput );
void sysLogOut( const QString &intput );

#endif // SYSPARA_H
