#if !defined(__7D27741735B7E7F9888ED8708C3__)
# define __7D27741735B7E7F9888ED8708C3__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

# include <xpadf/common/objects.h>

# if defined(__cplusplus)
extern "C" {
# endif /* __cplusplus */

  XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateLock, (XPADF_OUT PXPADF_HANDLE _phLock));

  XPADF_FUNCTION(XPADF_RESULT, xpadf_Lock, (XPADF_IN XPADF_HANDLE _hLock));
  XPADF_FUNCTION(XPADF_RESULT, xpadf_UnLock, (XPADF_IN XPADF_HANDLE _hLock));

# if defined(__cplusplus)
}
# endif /* __cplusplus */

#endif /* !__7D27741735B7E7F9888ED8708C3__ */
