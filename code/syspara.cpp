#include "syspara.h"
#include <QString>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QDebug>
static SysPara _sys_para;

SysPara::SysPara()
{
    mArgc = 0;
    mArgv = NULL;

    m_pInputFile = NULL;
    m_pOutputFile = NULL;
}

SysPara::~SysPara()
{qDebug()<<__FUNCTION__<<__LINE__;
    if ( m_pInputFile != NULL )
    {
        m_pInputFile->close();
        delete m_pInputFile;
    }

    if ( m_pOutputFile != NULL )
    {
        m_pOutputFile->close();
        delete m_pOutputFile;
    }
}

void SysPara::logFile( QFile *pFile, const QString &content )
{
    if ( NULL == pFile )
    { return; }

    QString fmtContent=QString("%1 %2\n").arg( QDateTime::currentDateTime().toString("yyyy/M/d h:m:s.z"))
                                       .arg(content);
    qint64 ret =
    pFile->write( fmtContent.toLatin1() );
    //! \note flush the buffer
    pFile->flush();

    if ( ret != fmtContent.length() )
    {
        //! error

    }
//    qDebug()<<ret;
}

QFile *SysPara::open( const QString &name )
{
    QFile *pFile;

    //! assure path
    QString fullPath = qApp->applicationDirPath() + "/data";
    QDir dir( fullPath );
    if ( dir.exists() )
    {}
    else
    {
        if ( dir.mkdir( fullPath) )
        {}
        else
        { return NULL; }
    }

    pFile = new QFile(  fullPath + "/" + name, NULL );
    if ( NULL == pFile )
    { return NULL; }

    if ( pFile->open( QIODevice::Append  ) )
    {
        return pFile;
    }
    else
    {
        delete pFile;
        return NULL;
    }
}

void sysSetArg( int argc, char **argv )
{
    _sys_para.mArgc = argc;
    _sys_para.mArgv = argv;
}
bool sysHasArg( const QString &arg )
{
    for ( int i = 1; i < _sys_para.mArgc; i++ )
    {
        if ( QString::compare( arg, _sys_para.mArgv[i], Qt::CaseInsensitive ) == 0 )
        { return true; }
    }

    return false;
}

void sysLogIn( const QString &input )
{
    if ( sysHasArg("-loginput") )
    {}
    else
    { return; }

    if ( NULL == _sys_para.m_pInputFile )
    {
        _sys_para.m_pInputFile = _sys_para.open( "login.txt" );
    }

    //! log it
    _sys_para.logFile( _sys_para.m_pInputFile, input);
}

void sysLogOut( const QString &input )
{
    if ( sysHasArg("-logoutput") )
    {}
    else
    { return; }

    if ( NULL == _sys_para.m_pOutputFile )
    {
        _sys_para.m_pOutputFile = _sys_para.open( "logout.txt" );
    }

    //! log it
    _sys_para.logFile( _sys_para.m_pOutputFile, input);
}
