#include <pthread.h>
#include <malloc.h>

#include <xpadf/internal/posix/thread/tpool.h>

XPADF_STATIC_FUNCTION(XPADF_RESULT, _xpadf_DestroyThreadPool, (XPADF_IN PXPADF_THREAD_POOL _pThreadPool)) {
  XPADF_RESULT _result;

  _pThreadPool->m_bRunning = XPADF_FALSE;

  if(_pThreadPool->m_sObject.m_pStopCallback)
    _result = _pThreadPool->m_sObject.m_pStopCallback(_pThreadPool, _pThreadPool->m_sObject.m_pContext);
  else _result = XPADF_OK;

  if(XPADF_SUCCEEDED(_result)) {
    PXPADF_THREAD_POOL_WORK _pWork;
    PXPADF_THREAD_POOL_WORK _pNext;
    XPADF_SIZE              _i;

    for(_i = 0; _i < _pThreadPool->m_nActiveWorkers; ++_i)
      sem_post(&_pThreadPool->m_hJobEvent);

    sem_wait(&_pThreadPool->m_hShutdownEvent);

    for(_pWork = _pThreadPool->m_pPendingWorkListHead; _pWork; _pWork = _pNext) {
      _pNext = _pWork->m_pNext;

      if(_pWork->m_pCleanupCallback)
        _pWork->m_pCleanupCallback(_pThreadPool, _pWork->m_pContext, XPADF_WARNING_DRAINING);

      free(_pWork);
    }

    for(_pWork = _pThreadPool->m_pFreeWorkListHead; _pWork; _pWork = _pNext) {
      _pNext = _pWork->m_pNext;

      free(_pWork);
    }

    sem_destroy(&_pThreadPool->m_hShutdownEvent);
    sem_destroy(&_pThreadPool->m_hJobEvent);

    pthread_mutex_destroy(&_pThreadPool->m_hLock);

    return _result;
  }

  _pThreadPool->m_bRunning = XPADF_TRUE;

  return _result;
}

XPADF_STATIC_FUNCTION(void *, _xpadf_ThreadPool, (XPADF_IN PXPADF_THREAD_POOL _pThreadPool)) {
  PXPADF_THREAD_POOL_WORK _pWork;
  XPADF_RESULT            _result;

  while(_pThreadPool->m_bRunning) {
    pthread_mutex_lock(&_pThreadPool->m_hLock);
    if((_pWork = _pThreadPool->m_pPendingWorkListHead)) {
      _pThreadPool->m_pPendingWorkListHead = _pWork->m_pNext;
      pthread_mutex_unlock(&_pThreadPool->m_hLock);

      if(XPADF_FAILED(_result = _pWork->m_pWorkCallback(_pThreadPool, _pWork->m_pContext)) || !_pWork->m_bPermanent) {
        if(_pWork->m_pCleanupCallback)
          _pWork->m_pCleanupCallback(_pThreadPool, _pWork->m_pContext, _result);

        XPADF_ATOMIC_PUSH_SLIST_HEAD(_pWork, _pThreadPool->m_pFreeWorkListHead, m_pNext);
      } else {
        _pWork->m_pNext = NULL;

        pthread_mutex_lock(&_pThreadPool->m_hLock);
        if(_pThreadPool->m_pPendingWorkListTail)
          _pThreadPool->m_pPendingWorkListTail            =
            _pThreadPool->m_pPendingWorkListTail->m_pNext = _pWork;
        else
          _pThreadPool->m_pPendingWorkListTail   =
            _pThreadPool->m_pPendingWorkListHead = _pWork;
        pthread_mutex_unlock(&_pThreadPool->m_hLock);
      }
    } else if(_pThreadPool->m_nActiveWorkers > _pThreadPool->m_nMinimumWorkers) {
      --_pThreadPool->m_nActiveWorkers;
      pthread_mutex_unlock(&_pThreadPool->m_hLock);

      break;
    } else {
      pthread_mutex_unlock(&_pThreadPool->m_hLock);

      sem_wait(&_pThreadPool->m_hJobEvent);
    }
  }

  if(!_pThreadPool->m_nActiveWorkers)
    sem_post(&_pThreadPool->m_hShutdownEvent);

  return NULL;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateThreadPool, (XPADF_OUT    PXPADF_HANDLE        _phThreadPool,
                                                      XPADF_IN     XPADF_SIZE           _nMinimumWorkers,
                                                      XPADF_IN     XPADF_SIZE           _nMaximumWorkers,
                                                      XPADF_IN_OPT XPADF_PVOID          _pContext,
                                                      XPADF_IN_OPT PXPADFThreadCallback _pStopCallback)) {
  if(_phThreadPool && _nMinimumWorkers && (_nMinimumWorkers <= _nMaximumWorkers)) {
    XPADF_RESULT _result = _xpadf_AllocateThreadObject((PXPADF_OBJECT *)_phThreadPool, XPADF_OBJECT_TYPE_THREAD_POOL,
                                                       (PXPADFCleanupObjectCallback)_xpadf_DestroyThreadPool,
                                                       sizeof(XPADF_THREAD_POOL), _pContext, _pStopCallback);

    if(XPADF_SUCCEEDED(_result)) {
      PXPADF_THREAD_POOL _pThreadPool = (PXPADF_THREAD_POOL)(*_phThreadPool);

      if(pthread_mutex_init(&_pThreadPool->m_hLock, NULL))
        _result = XPADF_ERROR_CREATE_LOCK;
      else {
        if(sem_init(&_pThreadPool->m_hShutdownEvent, 0, 0))
          _result = XPADF_ERROR_CREATE_SEMAPHORE;
        else {
          if(sem_init(&_pThreadPool->m_hJobEvent, 0, 0))
            _result = XPADF_ERROR_CREATE_SEMAPHORE;
          else {
            _pThreadPool->m_nMinimumWorkers = _nMinimumWorkers;
            _pThreadPool->m_nMaximumWorkers = _nMaximumWorkers;
            _pThreadPool->m_bRunning        = XPADF_TRUE;
            
            return XPADF_OK;
          }
          
          sem_destroy(&_pThreadPool->m_hShutdownEvent);
        }

        pthread_mutex_destroy(&_pThreadPool->m_hLock);
      }

      _xpadf_DereferenceObject(*_phThreadPool); 
    }

    return _result;
  } return XPADF_ERROR_INVALID_PARAMETERS;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_EnqueueThreadPoolWork, (XPADF_IN     XPADF_HANDLE                _hThreadPool,
                                                           XPADF_IN     XPADF_BOOL                  _bPermanent,
                                                           XPADF_IN_OPT XPADF_PVOID                 _pContext,
                                                           XPADF_IN     PXPADFThreadCallback        _pWorkCallback,
                                                           XPADF_IN_OPT PXPADFThreadCleanupCallback _pCleanupCallback)) {
  XPADF_RESULT _result;

  if(_hThreadPool && _pWorkCallback) {
    PXPADF_THREAD_POOL _pThreadPool = (PXPADF_THREAD_POOL)_hThreadPool;

    if(XPADF_OBJECT_TYPE_THREAD_POOL == _pThreadPool->m_sObject.m_sObject.m_eType) {
      if(_pThreadPool->m_bRunning) {
        PXPADF_THREAD_POOL_WORK _pWork;

        XPADF_ATOMIC_POP_SLIST_HEAD(_pWork, _pThreadPool->m_pFreeWorkListHead, m_pNext);

        if(!_pWork)
          _pWork = (PXPADF_THREAD_POOL_WORK)malloc(sizeof(XPADF_THREAD_POOL_WORK));

        if(_pWork) {
          _pWork->m_pNext            = NULL;
          _pWork->m_bPermanent       = _bPermanent;
          _pWork->m_pContext         = _pContext;
          _pWork->m_pWorkCallback    = _pWorkCallback;
          _pWork->m_pCleanupCallback = _pCleanupCallback;

          pthread_mutex_lock(&_pThreadPool->m_hLock);
          if(_pThreadPool->m_pPendingWorkListTail)
            _pThreadPool->m_pPendingWorkListTail            =
              _pThreadPool->m_pPendingWorkListTail->m_pNext = _pWork;
          else
            _pThreadPool->m_pPendingWorkListHead   =
              _pThreadPool->m_pPendingWorkListTail = _pWork;

          if(_pThreadPool->m_nActiveWorkers < _pThreadPool->m_nMaximumWorkers) {
            pthread_attr_t _sAttribute;

            ++_pThreadPool->m_nActiveWorkers;
            pthread_mutex_unlock(&_pThreadPool->m_hLock);

            if(pthread_attr_init(&_sAttribute))
              _result = XPADF_ERROR_INITIALIZE_THREAD_ATTRIBUTE;
            else {
              if(pthread_attr_setdetachstate(&_sAttribute, PTHREAD_CREATE_DETACHED))
                _result = XPADF_ERROR_SET_THREAD_ATTRIBUTE;
              else {
                pthread_t _hThread;

                if(pthread_create(&_hThread, &_sAttribute, (PXPADFPThreadCallback)_xpadf_ThreadPool, _pThreadPool))
                  _result = XPADF_ERROR_CREATE_THREAD;
                else {
                  pthread_attr_destroy(&_sAttribute);
                  
                  return XPADF_OK;
                }
              }

              pthread_attr_destroy(&_sAttribute);
            }

            pthread_mutex_lock(&_pThreadPool->m_hLock);
            if((--_pThreadPool->m_nActiveWorkers)) {
              pthread_mutex_unlock(&_pThreadPool->m_hLock);

              sem_post(&_pThreadPool->m_hJobEvent);

              return XPADF_CONVERT_ERROR_TO_WARNING(_result);
            } else pthread_mutex_unlock(&_pThreadPool->m_hLock);
          } else {
            pthread_mutex_unlock(&_pThreadPool->m_hLock);

            sem_post(&_pThreadPool->m_hJobEvent);

            return XPADF_OK;
          }
        } else _result = XPADF_ERROR_OUT_OF_MEMORY;
      } else _result = XPADF_ERROR_INVALID_OPERATION;
    } else _result = XPADF_ERROR_INVALID_PARAMETERS;
  } else _result = XPADF_ERROR_INVALID_PARAMETERS;

  if(_pCleanupCallback)
    _pCleanupCallback(_hThreadPool, _pContext, _result);

  return _result;
}
