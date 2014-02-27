#if !defined(__B630E00958B55CAE922DF3C2A3E__)
# define __B630E00958B55CAE922DF3C2A3E__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

# include <xpadf/common/objects.h>

XPADF_CALLBACK(void, PXPADFIOCleanupCallback, (XPADF_IN     XPADF_HANDLE _hIO,
                                               XPADF_IN_OPT XPADF_PVOID  _pContext,
                                               XPADF_IN     XPADF_RESULT _nStatus));

# if defined(__cplusplus)
extern "C" {
# endif /* __cplusplus */

  

# if defined(__cplusplus)
}
# endif /* __cplusplus */

#endif /* !__B630E00958B55CAE922DF3C2A3E__ */
