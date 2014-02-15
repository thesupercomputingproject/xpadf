#if !defined(__FA219C000271B19000D0204848D__)
# define __FA219C000271B19000D0204848D__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

# include <xpadf/common/objects.h>

# include <xpadf/thread/thread.h>

# if defined(__cplusplus)
extern "C" {
# endif /* __cplusplus */

  XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateLooper, (XPADF_OUT    PXPADF_HANDLE               _phLooper,
                                                    XPADF_IN     XPADF_PVOID                 _pContext,
                                                    XPADF_IN_OPT PXPADFThreadCallback        _pInitializeCallback,
                                                    XPADF_IN     PXPADFThreadCallback        _pExecuteCallback,
                                                    XPADF_IN_OPT PXPADFThreadCallback        _pStopCallback,
                                                    XPADF_IN_OPT PXPADFThreadCleanupCallback _pCleanupCallback));

# if defined(__cplusplus)
}
# endif /* __cplusplus */

#endif /* !__FA219C000271B19000D0204848D__ */
