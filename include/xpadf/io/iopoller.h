#if !defined(__C974F6FF94BF0DADC206336DB5B__)
# define __C974F6FF94BF0DADC206336DB5B__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

# include <xpadf/common/objects.h>

# include <xpadf/io/io.h>

# include <xpadf/thread/tpool.h>

# if defined(__cplusplus)
extern "C" {
# endif /* __cplusplus */

  XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateIOPoller, (XPADF_OUT PXPADF_HANDLE _phIOPoller));

  XPADF_FUNCTION(XPADF_RESULT, xpadf_RegisterIOWithIOPoller, (XPADF_IN XPADF_HANDLE _hIOPoller,
                                                              XPADF_IN XPADF_HANDLE _hIO));

  XPADF_FUNCTION(XPADF_RESULT, xpadf_PollIO, (XPADF_IN     XPADF_HANDLE _hIOPoller,
                                              XPADF_IN_OPT XPADF_HANDLE _hThreadPool));

# if defined(__cplusplus)
}
# endif /* __cplusplus */

#endif /* !__C974F6FF94BF0DADC206336DB5B__ */
