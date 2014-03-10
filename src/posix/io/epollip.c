#include <unistd.h>
#include <malloc.h>
#include <errno.h>

#include <sys/epoll.h>

#include <xpadf/internal/posix/io/epollip.h>

#include <xpadf/internal/posix/thread/tpool.h>

XPADF_STATIC_FUNCTION(XPADF_RESULT, _xpadf_DestroyEPollIOPoller, (XPADF_IN PXPADF_IO_POLLER _pIOPoller)) {
  XPADF_RESULT _result = XPADF_OK;

  if(!_pIOPoller->m_bRunning || XPADF_SUCCEEDED(_result = xpadf_StopIOPoller((XPADF_HANDLE)_pIOPoller))) {
    PXPADF_IO              _pIO;
    PXPADF_IO              _pNextIO;
    PXPADF_IO_POLLER_EVENT _pEvent;
    PXPADF_IO_POLLER_EVENT _pNextEvent;

    close(_pIOPoller->m_aPipes[0]);
    close(_pIOPoller->m_aPipes[1]);
    close(_pIOPoller->m_hEPoll);
    
    for(_pIO = _pIOPoller->m_pIOListHead; _pIO; _pIO = _pNextIO) {
      _pNextIO = _pIO->m_pNext;
      
      xpadf_DestroyObject((XPADF_HANDLE)_pIO);
    }
    
    for(_pEvent = _pIOPoller->m_pFreeEventListHead; _pEvent; _pEvent = _pNextEvent) {
      _pNextEvent = _pEvent->m_pNext;
      
      free(_pEvent);
    }
    
    pthread_mutex_destroy(&_pIOPoller->m_hLock);
  }

  return _result;
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _xpadf_PerformIO, (XPADF_IN_OPT XPADF_HANDLE           _hThreadPool,
                                                       XPADF_IN     PXPADF_IO_POLLER_EVENT _pEvent)) {
  XPADF_RESULT _result;

  if(_pEvent->m_nOperations) {
    PXPADF_IO _pIO = _pEvent->m_pIO;
    
    _pIO->m_nOperations = 0;
    
    if(_pEvent->m_nOperations & XPADF_IO_OPERATION_READ)
      _result = _pIO->m_sRead.m_pInternalCallback(_pIO);
    else
      _result = XPADF_OK;
    
    if(XPADF_SUCCEEDED(_result) && (_pEvent->m_nOperations & XPADF_IO_OPERATION_WRITE))
      _result = _pIO->m_sWrite.m_pInternalCallback(_pIO);
    
    if(XPADF_SUCCEEDED(_result))
      _result = _xpadf_EnableIOOperations(_pIO);
    
    if(XPADF_FAILED(_result))
      xpadf_DestroyObject((XPADF_HANDLE)_pIO);
  } else _result = XPADF_OK;

  return _result;
}

XPADF_STATIC_FUNCTION(void, _xpadf_EventCleanup, (XPADF_IN_OPT XPADF_HANDLE           _hThreadPool,
                                                  XPADF_IN     PXPADF_IO_POLLER_EVENT _pEvent,
                                                  XPADF_IN     XPADF_RESULT           _nStatus)) {
  PXPADF_IO_POLLER _pIOPoller = (PXPADF_IO_POLLER)_pEvent->m_pIO->m_hIOPoller;

  XPADF_ATOMIC_PUSH_SLIST_HEAD(_pEvent, _pIOPoller->m_pFreeEventListHead, m_pNext);
}

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_EnableIOOperations, (XPADF_IN PXPADF_IO _pIO)) {
  XPADF_RESULT       _result;
  PXPADF_IO_POLLER   _pIOPoller = (PXPADF_IO_POLLER)_pIO->m_hIOPoller;
  struct epoll_event _sEvent    = {0
#if defined(HAVE_DECL_EPOLLONESHOT)
                                   | EPOLLONESHOT
#endif /* HAVE_DECL_EPOLLONESHOT */
                                   , {.ptr = _pIO}};
  
  if(_pIO->m_nOperations & XPADF_IO_OPERATION_READ)
    _sEvent.events |= EPOLLIN;
  
  if(_pIO->m_nOperations & XPADF_IO_OPERATION_WRITE)
    _sEvent.events |= EPOLLOUT;
  
  if(epoll_ctl(_pIOPoller->m_hEPoll, EPOLL_CTL_MOD, _pIO->m_hFD, &_sEvent))
    _result = XPADF_ERROR_REGISTER_IO;
  else _result = XPADF_OK;

  return _result;
}

XPADF_INTERNAL_FUNCTION(void, _xpadf_UnregisterIOFromIOPoller, (XPADF_IN PXPADF_IO _pIO)) {
  PXPADF_IO_POLLER _pIOPoller = (PXPADF_IO_POLLER)_pIO->m_hIOPoller;

  if(_pIOPoller) {
    pthread_mutex_lock(&_pIOPoller->m_hLock);
    
    if(_pIO->m_pPrevious)
      _pIO->m_pPrevious->m_pNext = _pIO->m_pNext;
    else
      _pIOPoller->m_pIOListHead = _pIO->m_pNext;
    
    if(_pIO->m_pNext)
      _pIO->m_pNext->m_pPrevious = _pIO->m_pPrevious;
    
    pthread_mutex_unlock(&_pIOPoller->m_hLock);
  }
}

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_RegisterIOWithIOPoller, (XPADF_IN XPADF_HANDLE _hIOPoller,
                                                                      XPADF_IN XPADF_HANDLE _hIO,
                                                                      XPADF_IN XPADF_ULONG  _nOperation)) {
  XPADF_RESULT _result = xpadf_RegisterIOWithIOPoller(_hIOPoller, _hIO);

  if(XPADF_SUCCEEDED(_result)) {
    PXPADF_IO_POLLER   _pIOPoller = (PXPADF_IO_POLLER)_hIOPoller;
    PXPADF_IO          _pIO       = (PXPADF_IO)_hIO;
    struct epoll_event _sEvent    = {0
#if defined(HAVE_DECL_EPOLLONESHOT)
                                     | EPOLLONESHOT
#endif /* HAVE_DECL_EPOLLONESHOT */
                                     , {.ptr = _pIO}};

    if(XPADF_IO_OPERATION_READ & _nOperation)
      _sEvent.events |= EPOLLIN;
    
    if(XPADF_IO_OPERATION_WRITE & _nOperation)
      _sEvent.events |= EPOLLOUT;

    if(epoll_ctl(_pIOPoller->m_hEPoll, EPOLL_CTL_MOD, _pIO->m_hFD, &_sEvent))
      _result = XPADF_ERROR_REGISTER_IO;
  }

  return _result;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateIOPoller, (XPADF_OUT PXPADF_HANDLE _phIOPoller)) {
  if(_phIOPoller) {
    XPADF_RESULT _result = _xpadf_AllocateObject((PXPADF_OBJECT *)_phIOPoller, XPADF_OBJECT_TYPE_IO_POLLER,
                                                 (PXPADFCleanupObjectCallback)_xpadf_DestroyEPollIOPoller,
                                                 sizeof(XPADF_IO_POLLER));

    if(XPADF_SUCCEEDED(_result)) {
      PXPADF_IO_POLLER _pIOPoller = (PXPADF_IO_POLLER)(*_phIOPoller);

      if(pthread_mutex_init(&_pIOPoller->m_hLock, NULL))
        _result = XPADF_ERROR_CREATE_LOCK;
      else {
#if defined(HAVE_EPOLL_CREATE1)
      if((_pIOPoller->m_hEPoll = epoll_create1(EPOLL_CLOEXEC)) < 0)
#elif defined(HAVE_EPOLL_CREATE)
        if((_pIOPoller->m_hEPoll = epoll_create(1)) < 0)
#else
# error ERROR: The XPADF I/O Poller implementation is using epoll however neither epoll_create nor epoll_create1 is defined
#endif
          _result = XPADF_ERROR_OPEN_DEVICE;
        else {
          if(pipe(_pIOPoller->m_aPipes))
            _result = XPADF_ERROR_OPEN_DEVICE;
          else {
            struct epoll_event _sEvent = {EPOLLIN, {.ptr = NULL}};

            if(epoll_ctl(_pIOPoller->m_hEPoll, EPOLL_CTL_ADD, _pIOPoller->m_aPipes[0], &_sEvent))
              _result = XPADF_ERROR_REGISTER_IO;
            else {
              _pIOPoller->m_bRunning = XPADF_TRUE;
            
              return _result;
            }

            close(_pIOPoller->m_aPipes[0]);
            close(_pIOPoller->m_aPipes[1]);
          }

          close(_pIOPoller->m_hEPoll);
        }
      
      pthread_mutex_destroy(&_pIOPoller->m_hLock);
      }
    }

    free(*_phIOPoller);

    return _result;
  } return XPADF_ERROR_INVALID_PARAMETERS;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_RegisterIOWithIOPoller, (XPADF_IN XPADF_HANDLE _hIOPoller,
                                                            XPADF_IN XPADF_HANDLE _hIO)) {
  if(_hIOPoller && _hIO) {
    PXPADF_IO        _pIO       = (PXPADF_IO)_hIO;
    PXPADF_IO_POLLER _pIOPoller = (PXPADF_IO_POLLER)_hIOPoller;

    if((XPADF_OBJECT_TYPE_IO        == _pIO->m_sObject.m_eType      ) &&
       (XPADF_OBJECT_TYPE_IO_POLLER == _pIOPoller->m_sObject.m_eType)) {
      if(_pIOPoller->m_bRunning) {
        XPADF_RESULT _result;
        
        if(_pIO->m_hIOPoller)
          _result = XPADF_ERROR_INVALID_OPERATION;
        else {
          struct epoll_event _sEvent = {0
#if defined(HAVE_DECL_EPOLLONESHOT)
                                        | EPOLLONESHOT
#endif /* HAVE_DECL_EPOLLONESHOT */
                                        , {.ptr = _pIO}};

          _xpadf_ReferenceObject((PXPADF_OBJECT)_pIO);

          _pIO->m_hIOPoller = (XPADF_HANDLE)_pIOPoller;
          _pIO->m_pPrevious = NULL;
          
          pthread_mutex_lock(&_pIOPoller->m_hLock);
          if((_pIO->m_pNext = _pIOPoller->m_pIOListHead))
            _pIO->m_pNext->m_pPrevious = _pIO;
          
          _pIOPoller->m_pIOListHead = _pIO;
          pthread_mutex_unlock(&_pIOPoller->m_hLock);
          
          if(epoll_ctl(_pIOPoller->m_hEPoll, EPOLL_CTL_ADD, _pIO->m_hFD, &_sEvent))
            _result = XPADF_ERROR_REGISTER_IO;
          else return XPADF_OK;

          xpadf_DestroyObject((XPADF_HANDLE)_pIO);
        }

        return _result;
      } return XPADF_ERROR_INVALID_OPERATION;
    }
  } return XPADF_ERROR_INVALID_PARAMETERS;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_PollIO, (XPADF_IN     XPADF_HANDLE _hIOPoller,
                                            XPADF_IN_OPT XPADF_HANDLE _hThreadPool)) {
  if(_hIOPoller) {
    PXPADF_IO_POLLER _pIOPoller = (PXPADF_IO_POLLER)_hIOPoller;

    if(XPADF_OBJECT_TYPE_IO_POLLER == _pIOPoller->m_sObject.m_eType) {
      if(_pIOPoller->m_bRunning) {
        XPADF_RESULT       _result      = XPADF_OK;
        PXPADF_THREAD_POOL _pThreadPool = (PXPADF_THREAD_POOL)_hThreadPool;
        struct epoll_event _aEvents[
#if defined(HAVE_DECL_EPOLLONESHOT)
                                    XPADF_MAXIMUM_IO_EVENTS
#else
                                    1
#endif /* HAVE_DECL_EPOLLONESHOT */
                                    ];
        int                _nLimit;
        int                _i;
        
        _xpadf_ReferenceObject((PXPADF_OBJECT)_pIOPoller);
        
        if(_pThreadPool) {
          if(XPADF_OBJECT_TYPE_THREAD_POOL == _pThreadPool->m_sObject.m_sObject.m_eType)
            _xpadf_ReferenceObject((PXPADF_OBJECT)_pThreadPool);
          else {
            xpadf_DestroyObject((XPADF_HANDLE)_pIOPoller);
            
            return XPADF_ERROR_INVALID_PARAMETERS;
          }
        }
        
        if((_nLimit = epoll_wait(_pIOPoller->m_hEPoll, _aEvents, sizeof(_aEvents) / sizeof(struct epoll_event), -1)) < 0)
          _result = (EINTR == errno) ? XPADF_OK : XPADF_ERROR_POLL_IO;
        else if(_pThreadPool) {
#if !defined(HAVE_DECL_EPOLLONESHOT)
          PXPADF_IO          _pIO    = (PXPADF_IO)_aEvents->data.ptr;
          struct epoll_event _sEvent = {0, {.ptr = _pIO}};

          if(epoll_ctl(_pIOPoller->m_hEPoll, EPOLL_CTL_MOD, _pIO->m_hFD, &_sEvent))
            _result = XPADF_ERROR_REGISTER_IO;
          else {
#endif /* !HAVE_DECL_EPOLLONESHOT */
            PXPADF_IO_POLLER_EVENT _pEvent;
            
            for(_i = 0; (_i < _nLimit) && XPADF_SUCCEEDED(_result); ++_i) {
              if(_aEvents[_i].data.ptr) {
                XPADF_ATOMIC_POP_SLIST_HEAD(_pEvent, _pIOPoller->m_pFreeEventListHead, m_pNext);
                
                if(!_pEvent)
                  _pEvent = (PXPADF_IO_POLLER_EVENT)malloc(sizeof(XPADF_IO_POLLER_EVENT));
                
                if(_pEvent) {
                  if(_aEvents[_i].events & EPOLLIN)
                    _pEvent->m_nOperations = XPADF_IO_OPERATION_READ;
                  else
                    _pEvent->m_nOperations = 0;
                  
                  if(_aEvents[_i].events & EPOLLOUT)
                    _pEvent->m_nOperations |= XPADF_IO_OPERATION_WRITE;
                  
                  _pEvent->m_pIO = (PXPADF_IO)_aEvents[_i].data.ptr;

                  _result = xpadf_EnqueueThreadPoolWork(_hThreadPool, XPADF_FALSE, _pEvent, (PXPADFThreadCallback)_xpadf_PerformIO,
                                                        (PXPADFThreadCleanupCallback)_xpadf_EventCleanup);
                } else _result = XPADF_ERROR_OUT_OF_MEMORY;
              } else break;
            }
#if !defined(HAVE_DECL_EPOLLONESHOT)
          }
#endif /* !HAVE_DECL_EPOLLONESHOT */
        } else {
          for(_i = 0; _i < _nLimit; ++_i) {
            if(_aEvents[_i].data.ptr) {
              XPADF_IO_POLLER_EVENT _sEvent = {NULL, 0, (PXPADF_IO)_aEvents[_i].data.ptr};
              
              if(_aEvents[_i].events & EPOLLIN)
                _sEvent.m_nOperations = XPADF_IO_OPERATION_READ;
              
              if(_aEvents[_i].events & EPOLLOUT)
                _sEvent.m_nOperations |= XPADF_IO_OPERATION_WRITE;

              _xpadf_PerformIO(NULL, &_sEvent);
            } else break;
          }
        }
        
        if(_pThreadPool)
          xpadf_DestroyObject((XPADF_HANDLE)_pThreadPool);
        
        xpadf_DestroyObject((XPADF_HANDLE)_pIOPoller);
        
        return _result;
      } return XPADF_ERROR_INVALID_OPERATION;
    }
  } return XPADF_ERROR_INVALID_PARAMETERS;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_StopIOPoller, (XPADF_IN XPADF_HANDLE _hIOPoller)) {
  if(_hIOPoller) {
    PXPADF_IO_POLLER _pIOPoller = (PXPADF_IO_POLLER)_hIOPoller;

    if(XPADF_OBJECT_TYPE_IO_POLLER == _pIOPoller->m_sObject.m_eType) {
      if(_pIOPoller->m_bRunning) {
        _pIOPoller->m_bRunning = XPADF_FALSE;

        if(write(_pIOPoller->m_aPipes[1], &_pIOPoller->m_bRunning, sizeof(_pIOPoller->m_bRunning)) > 0)
          return XPADF_OK;

        _pIOPoller->m_bRunning = XPADF_TRUE;

        return XPADF_ERROR_WRITE_IO;
      } return XPADF_ERROR_INVALID_OPERATION;
    }
  } return XPADF_ERROR_INVALID_PARAMETERS;
}
