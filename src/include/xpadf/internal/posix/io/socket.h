#if !defined(__3B484D6EBE9895FB58F176CA8E6__)
# define __3B484D6EBE9895FB58F176CA8E6__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

# include <xpadf/io/io.h>

# include <xpadf/internal/posix/io/io.h>

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_AllocateSocketObject, (XPADF_OUT    PXPADF_OBJECT               *_ppObject,
                                                                    XPADF_IN     int                          _nAddressFamily,
                                                                    XPADF_IN     int                          _nSocketType,
                                                                    XPADF_IN_OPT XPADF_PVOID                  _pContext,
                                                                    XPADF_IN_OPT PXPADFIOCleanupCallback      _pCleanupCallback,
                                                                    XPADF_IN_OPT PXPADFIOCallback             _pInternalReadCallback,
                                                                    XPADF_IN_OPT PXPADFIOCallback             _pInternalWriteCallback,
                                                                    XPADF_IN_OPT PXPADFIOCallback             _pExternalReadCallback,
                                                                    XPADF_IN_OPT PXPADFIOCallback             _pExternalWriteCallback));

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_TCPSocketRead, (XPADF_IN PXPADF_IO _pIO));
XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_TCPSocketWrite, (XPADF_IN PXPADF_IO _pIO));
XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_TCPSocketAccept, (XPADF_IN PXPADF_IO _pIO));
XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_TCPSocketConnect, (XPADF_IN PXPADF_IO _pIO));

#endif /* !__3B484D6EBE9895FB58F176CA8E6__ */
