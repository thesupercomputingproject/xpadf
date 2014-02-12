#if !defined(__67C9D30A8E1E2AFB2FF05321EE2__)
# define __67C9D30A8E1E2AFB2FF05321EE2__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

XPADF_DEFINE_DATA_TYPE(XPADF_PVOID, XPADF_HANDLE);

# if defined(__cplusplus)
extern "C" {
# endif /* __cplusplus */

  XPADF_FUNCTION(XPADF_RESULT, xpadf_DestroyObject, (XPADF_IN XPADF_HANDLE _hObject));

# if defined(__cplusplus)
}
# endif /* __cplusplus */

#endif /* !__67C9D30A8E1E2AFB2FF05321EE2__ */
