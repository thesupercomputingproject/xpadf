#if !defined(__B630E00958B55CAE922DF3C2A3E__)
# define __B630E00958B55CAE922DF3C2A3E__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

# include <xpadf/common/objects.h>

XPADF_CALLBACK(XPADF_RESULT, PXPADFIOReadCallback, (XPADF_IN     XPADF_HANDLE    _hIO,
                                                    XPADF_IN_OPT XPADF_PVOID     _pContext,
                                                    XPADF_IN_OPT PXPADF_BUFFER64 _pBuffer));
XPADF_CALLBACK(XPADF_RESULT, PXPADFIOWriteCallback, (XPADF_IN     XPADF_HANDLE _hIO,
                                                     XPADF_IN_OPT XPADF_PVOID  _pContext));
XPADF_CALLBACK(void, PXPADFIOCleanupCallback, (XPADF_IN     XPADF_HANDLE _hIO,
                                               XPADF_IN_OPT XPADF_PVOID  _pContext,
                                               XPADF_IN     XPADF_RESULT _nStatus));

# if defined(__cplusplus)
extern "C" {
# endif /* __cplusplus */

  XPADF_FUNCTION(XPADF_RESULT, xpadf_UpdateIOCleanupCallback, (XPADF_IN     XPADF_HANDLE            _hIO,
                                                               XPADF_IN_OPT XPADF_PVOID             _pContext,
                                                               XPADF_IN_OPT PXPADFIOCleanupCallback _pCleanupCallback));

  XPADF_FUNCTION(XPADF_RESULT, xpadf_IORead, (XPADF_IN     XPADF_HANDLE         _hIO,
                                              XPADF_IN_OPT XPADF_SIZE           _nSize,
                                              XPADF_IN_OPT XPADF_PVOID          _pContext,
                                              XPADF_IN     PXPADFIOReadCallback _pReadCompleteCallback));
  XPADF_FUNCTION(XPADF_RESULT, xpadf_IOWrite, (XPADF_IN     XPADF_HANDLE          _hIO,
                                               XPADF_IN     PXPADF_BUFFER64       _pBuffer,
                                               XPADF_IN_OPT XPADF_PVOID           _pContext,
                                               XPADF_IN     PXPADFIOWriteCallback _pWriteCompleteCallback));

# if defined(__cplusplus)
}
# endif /* __cplusplus */

#endif /* !__B630E00958B55CAE922DF3C2A3E__ */
