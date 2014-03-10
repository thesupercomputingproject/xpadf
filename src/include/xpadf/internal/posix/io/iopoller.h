#if !defined(__109F6C85FBC56E563D99AFEF8F7__)
# define __109F6C85FBC56E563D99AFEF8F7__

# include <xpadf/io/iopoller.h>

# include <xpadf/internal/common/objects.h>

# include <xpadf/internal/posix/io/io.h>

XPADF_INTERNAL_FUNCTION(void, _xpadf_UnregisterIOFromIOPoller, (XPADF_IN PXPADF_IO _pIO));
XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_RegisterIOWithIOPoller, (XPADF_IN XPADF_HANDLE _hIOPoller,
                                                                      XPADF_IN XPADF_HANDLE _hIO,
                                                                      XPADF_IN XPADF_ULONG  _nOperation));
XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_EnableIOOperations, (XPADF_IN PXPADF_IO _pIO));

#endif /* !__109F6C85FBC56E563D99AFEF8F7__ */
