#include <malloc.h>

#include <xpadf/internal/posix/thread/looper.h>

XPADF_STATIC_FUNCTION(XPADF_RESULT, _xpadf_DestroyLooper, (XPADF_IN PXPADF_LOOPER _pLooper)) {
  if(pthread_equal(pthread_self(), _pLooper->m_hThread))
     return XPADF_ERROR_INVALID_OPERATION;
  else {
    XPADF_RESULT _result;
    
    _pLooper->m_bRunning = XPADF_FALSE;
    
    if(_pLooper->m_sObject.m_pStopCallback)
      _result = _pLooper->m_sObject.m_pStopCallback(_pLooper, _pLooper->m_sObject.m_pContext);
    else _result = XPADF_OK;
    
    if(XPADF_SUCCEEDED(_result)) {
      void *_pTemp = NULL;
      
      pthread_join(_pLooper->m_hThread, &_pTemp);

      return _result;
    }

    _pLooper->m_bRunning = XPADF_TRUE;
    
    return _result;
  }
}

XPADF_STATIC_FUNCTION(void *, _xpadf_Looper, (XPADF_IN PXPADF_LOOPER _pLooper)) {
  XPADF_RESULT _result;

  if(_pLooper->m_pInitializeCallback)
    _result = _pLooper->m_pInitializeCallback(_pLooper, _pLooper->m_sObject.m_pContext);
  else _result = XPADF_OK;

  while(_pLooper->m_bRunning && XPADF_SUCCEEDED(_result))
    _result = _pLooper->m_pExecuteCallback(_pLooper, _pLooper->m_sObject.m_pContext);

  if(_pLooper->m_pCleanupCallback)
    _pLooper->m_pCleanupCallback(_pLooper, _pLooper->m_sObject.m_pContext, _result);

  return NULL;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateLooper, (XPADF_OUT    PXPADF_HANDLE               _phLooper,
                                                  XPADF_IN     XPADF_PVOID                 _pContext,
                                                  XPADF_IN_OPT PXPADFThreadCallback        _pInitializeCallback,
                                                  XPADF_IN     PXPADFThreadCallback        _pExecuteCallback,
                                                  XPADF_IN_OPT PXPADFThreadCallback        _pStopCallback,
                                                  XPADF_IN_OPT PXPADFThreadCleanupCallback _pCleanupCallback)) {
  XPADF_RESULT _result;

  if(_pExecuteCallback) {
    if(XPADF_SUCCEEDED(_result = _xpadf_AllocateThreadObject((PXPADF_OBJECT *)_phLooper, XPADF_OBJECT_TYPE_LOOPER, (PXPADFCleanupObjectCallback)_xpadf_DestroyLooper,
                                                             sizeof(XPADF_LOOPER), _pContext, _pStopCallback))) {
      pthread_attr_t _sAttribute;

      if(pthread_attr_init(&_sAttribute))
        _result = XPADF_ERROR_INITIALIZE_THREAD_ATTRIBUTE;
      else {
        if(pthread_attr_setdetachstate(&_sAttribute, PTHREAD_CREATE_JOINABLE))
          _result = XPADF_ERROR_SET_THREAD_ATTRIBUTE;
        else {
          PXPADF_LOOPER _pLooper = (PXPADF_LOOPER)(*_phLooper);

          _pLooper->m_bRunning            = XPADF_TRUE;
          _pLooper->m_pInitializeCallback = _pInitializeCallback;
          _pLooper->m_pExecuteCallback    = _pExecuteCallback;
          _pLooper->m_pCleanupCallback    = _pCleanupCallback;
          
          if(pthread_create(&_pLooper->m_hThread, &_sAttribute, (PXPADFPThreadCallback)_xpadf_Looper, _pLooper))
            _result = XPADF_ERROR_CREATE_THREAD;
          else {
            pthread_attr_destroy(&_sAttribute);
            
            return _result;
          }
        }

        pthread_attr_destroy(&_sAttribute);
      }

      free(*_phLooper);
    }
  } else _result = XPADF_ERROR_INVALID_PARAMETERS;

  if(_pCleanupCallback)
    _pCleanupCallback(NULL, _pContext, _result);
  
  return _result;
}
