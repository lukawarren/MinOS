#include <stdio.h>

#include "pdclib/_PDCLIB_glue.h"
#include "pdclib/_PDCLIB_defguard.h"
#include <errno.h>

_PDCLIB_fd_t open(const char* filename, unsigned int mode);

_PDCLIB_fd_t _PDCLIB_open( const char * const filename, unsigned int mode )
{
    _PDCLIB_fd_t rc;

    rc = open(filename, mode);

    if ( rc == _PDCLIB_NOHANDLE )
    {
        /* The 1:1 mapping in _PDCLIB_config.h ensures this works. */
        *_PDCLIB_errno_func() = errno;
    }

    return 0;
}