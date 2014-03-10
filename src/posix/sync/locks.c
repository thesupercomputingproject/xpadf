#include <malloc.h>

#include <xpadf/internal/posix/sync/locks.h>

XPADF_STATIC_FUNCTION(XPADF_RESULT, _xpadf_DestroyLock, (XPADF_IN PXPADF_LOCK _pLock)) {
  pthread_mutex_destroy(&_pLock->m_hLock);

  return XPADF_OK;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateLock, (XPADF_OUT PXPADF_HANDLE _phLock)) {
  if(_phLock) {
    XPADF_RESULT _result = _xpadf_AllocateObject((PXPADF_OBJECT *)_phLock, XPADF_OBJECT_TYPE_LOCK, (PXPADFCleanupObjectCallback)_xpadf_DestroyLock, sizeof(XPADF_LOCK));
    
    if(XPADF_SUCCEEDED(_result)) {
      PXPADF_LOCK _pLock = (PXPADF_LOCK)(*_phLock);
      
      if(pthread_mutex_init(&_pLock->m_hLock, NULL))
        _result = XPADF_ERROR_CREATE_LOCK;
      else return XPADF_OK;
      
      free(*_phLock);
    }
    
    return _result;
  } return XPADF_ERROR_INVALID_PARAMETERS;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_Lock, (XPADF_IN XPADF_HANDLE _hLock)) {
  if(_hLock) {
    PXPADF_LOCK _pLock = (PXPADF_LOCK)_hLock;

    if(XPADF_OBJECT_TYPE_LOCK == _pLock->m_sObject.m_eType)
      return pthread_mutex_lock(&_pLock->m_hLock) ? XPADF_ERROR_LOCK_OBJECT : XPADF_OK;
  } return XPADF_ERROR_INVALID_PARAMETERS;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_UnLock, (XPADF_IN XPADF_HANDLE _hLock)) {
  if(_hLock) {
    PXPADF_LOCK _pLock = (PXPADF_LOCK)_hLock;

    if(XPADF_OBJECT_TYPE_LOCK == _pLock->m_sObject.m_eType)
      return pthread_mutex_unlock(&_pLock->m_hLock) ? XPADF_ERROR_UNLOCK_OBJECT : XPADF_OK;
  } return XPADF_ERROR_INVALID_PARAMETERS;
}
