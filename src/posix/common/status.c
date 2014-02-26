#include <xpadf/common/macros.h>
#include <xpadf/common/types.h>

#define XPADF_DEFINE_STATUS(__name__, __module__, __code__)             \
  CXPADF_RESULT XPADF_DEBUG_##__name__       = XPADF_DEFINE_STATUS_CODE(XPADF_DEBUG_MASK,       __module__, __code__); \
  CXPADF_RESULT XPADF_INFORMATION_##__name__ = XPADF_DEFINE_STATUS_CODE(XPADF_INFORMATION_MASK, __module__, __code__); \
  CXPADF_RESULT XPADF_WARNING_##__name__     = XPADF_DEFINE_STATUS_CODE(XPADF_WARNING_MASK,     __module__, __code__); \
  CXPADF_RESULT XPADF_ERROR_##__name__       = XPADF_DEFINE_STATUS_CODE(XPADF_ERROR_MASK,       __module__, __code__);

#include <xpadf/common/status.h>
