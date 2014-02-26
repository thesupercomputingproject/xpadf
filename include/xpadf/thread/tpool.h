#if !defined(__C78A084FB26EAEEF5E17AB16471__)
# define __C78A084FB26EAEEF5E17AB16471__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

# include <xpadf/common/objects.h>

# include <xpadf/thread/thread.h>

# if defined(__cplusplus)
extern "C" {
# endif /* __cplusplus */

  XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateThreadPool, (XPADF_OUT    PXPADF_HANDLE        _phThreadPool,
                                                        XPADF_IN     XPADF_SIZE           _nMinimumWorkers,
                                                        XPADF_IN     XPADF_SIZE           _nMaximumWorkers,
                                                        XPADF_IN_OPT XPADF_PVOID          _pContext,
                                                        XPADF_IN_OPT PXPADFThreadCallback _pStopCallback));

  XPADF_FUNCTION(XPADF_RESULT, xpadf_EnqueueThreadPoolWork, (XPADF_IN     XPADF_HANDLE                _hThreadPool,
                                                             XPADF_IN     XPADF_BOOL                  _bPermanent,
                                                             XPADF_IN_OPT XPADF_PVOID                 _pContext,
                                                             XPADF_IN     PXPADFThreadCallback        _pWorkCallback,
                                                             XPADF_IN_OPT PXPADFThreadCleanupCallback _pCleanupCallback));

# if defined(__cplusplus)
}
# endif /* __cplusplus */

#endif /* !__C78A084FB26EAEEF5E17AB16471__ */
