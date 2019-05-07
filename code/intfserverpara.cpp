#include "intfserverpara.h"

IntfServerPara::IntfServerPara()
{

}


int IntfServerPara::assurePath( const QString &path )
{
    //! create path
    QDir dir( path );
    if ( dir.exists() )
    {}
    else
    {
        if ( dir.mkpath( path) )
        {}
        else
        {
            return -1;
        }
    }

    return 0;
}
