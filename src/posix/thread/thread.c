#include <xpadf/internal/posix/thread/thread.h>

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_AllocateThreadObject, (XPADF_OUT    PXPADF_OBJECT               *_ppObject,
                                                                    XPADF_IN     XPADF_OBJECT_TYPE            _eType,
                                                                    XPADF_IN     PXPADFCleanupObjectCallback  _pCleanupObjectCallback,
                                                                    XPADF_IN     XPADF_SIZE                   _nSize,
                                                                    XPADF_IN_OPT XPADF_PVOID                  _pContext,
                                                                    XPADF_IN_OPT PXPADFThreadCallback         _pStopCallback)) {
  if(sizeof(XPADF_THREAD_OBJECT) < _nSize) {
    XPADF_RESULT _result = _xpadf_AllocateObject(_ppObject, _eType, _pCleanupObjectCallback, _nSize);
    
    if(XPADF_SUCCEEDED(_result)) {
      PXPADF_THREAD_OBJECT _pThreadObject = (PXPADF_THREAD_OBJECT)(*_ppObject);

      _pThreadObject->m_pContext      = _pContext;
      _pThreadObject->m_pStopCallback = _pStopCallback;
    }
    
    return _result;
  } return XPADF_ERROR_INVALID_PARAMETERS;
}
