#if !defined(__26536C95CCDD70B67DEA67DAA7E__)
# define __26536C95CCDD70B67DEA67DAA7E__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

# include <xpadf/common/objects.h>

XPADF_CALLBACK(XPADF_RESULT, PXPADFThreadCallback, (XPADF_IN     XPADF_HANDLE _hThreadObject,
                                                    XPADF_IN_OPT XPADF_PVOID  _pContext));
XPADF_CALLBACK(void, PXPADFThreadCleanupCallback, (XPADF_IN     XPADF_HANDLE _hThreadObject,
                                                   XPADF_IN_OPT XPADF_PVOID  _pContext,
                                                   XPADF_IN     XPADF_RESULT _nStatus));

#endif /* !__26536C95CCDD70B67DEA67DAA7E__ */
