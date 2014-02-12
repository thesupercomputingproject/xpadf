#include <xpadf/internal/common/objects.h>

#include <malloc.h>
#include <string.h>

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_AllocateObject, (XPADF_OUT PXPADF_OBJECT               *_ppObject,
                                                              XPADF_IN  XPADF_OBJECT_TYPE            _eType,
                                                              XPADF_IN  PXPADFCleanupObjectCallback  _pCleanupObjectCallback,
                                                              XPADF_IN  XPADF_SIZE                   _nSize)) {
  if(_ppObject                            &&
     (XPADF_OBJECT_TYPE_INVALID > _eType) &&
     _pCleanupObjectCallback              &&
     (sizeof(XPADF_OBJECT) < _nSize)) {
    if(*_ppObject = (PXPADF_OBJECT)malloc(_nSize)) {
      memset(*_ppObject, 0, _nSize);

      (*_ppObject)->m_eType                  = _eType;
      (*_ppObject)->m_nReferenceCount        = 1;
      (*_ppObject)->m_pCleanupObjectCallback = _pCleanupObjectCallback;

      return XPADF_OK;
    } return XPADF_ERROR_OUT_OF_MEMORY;
  } return XPADF_ERROR_INVALID_PARAMETERS;
}

XPADF_INTERNAL_FUNCTION(void, _xpadf_ReferenceObject, (XPADF_INOUT PXPADF_OBJECT _pObject)) {
  XPADF_ATOMIC_INCREMENT(&_pObject->m_nReferenceCount);
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_DestroyObject, (XPADF_IN XPADF_HANDLE _hObject)) {
  if(_hObject) {
    PXPADF_OBJECT _pObject = (PXPADF_OBJECT)_hObject;

    if(XPADF_OBJECT_TYPE_INVALID > _pObject->m_eType) {
      if(XPADF_ATOMIC_DECREMENT(&_pObject->m_nReferenceCount))
        return XPADF_OK;
      else {
        XPADF_RESULT _result = _pObject->m_pCleanupObjectCallback(_pObject);

        if(XPADF_SUCCEEDED(_result))
          free(_pObject);

        return _result;
      }
    }
  } return XPADF_ERROR_INVALID_PARAMETERS;
}
