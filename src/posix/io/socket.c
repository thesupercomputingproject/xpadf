#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <xpadf/io/tsocket.h>

#include <xpadf/internal/posix/io/socket.h>
#include <xpadf/internal/posix/io/iopoller.h>

XPADF_STATIC_FUNCTION(XPADF_RESULT, _xpadf_DestroySocketObject, (XPADF_IN PXPADF_IO _pIO)) {
  shutdown(_pIO->m_hFD, SHUT_RDWR);
  close(_pIO->m_hFD);

  if(_pIO->m_sRead.m_sBuffer.m_pBuffer)
    free(_pIO->m_sRead.m_sBuffer.m_pBuffer);

  if(_pIO->m_sWrite.m_sBuffer.m_pBuffer)
    free(_pIO->m_sWrite.m_sBuffer.m_pBuffer);

  _xpadf_UnregisterIOFromIOPoller(_pIO);

  if(_pIO->m_pCleanupCallback)
    _pIO->m_pCleanupCallback((XPADF_HANDLE)_pIO, _pIO->m_pContext, XPADF_OK);

  return XPADF_OK;
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _xpadf_MakeSocketNonBlocking, (XPADF_INOUT PXPADF_IO _pIO)) {
  XPADF_RESULT _result;
  int          _nFlags = fcntl(_pIO->m_hFD, F_GETFL, 0);
  
  if(_nFlags < 0)
    _result = XPADF_ERROR_QUERY_IO_ATTRIBUTES;
  else {
    if(fcntl(_pIO->m_hFD, F_SETFL, _nFlags | O_NONBLOCK) < 0)
      _result = XPADF_ERROR_SET_IO_ATTRIBUTES;
    else _result = XPADF_OK;
  }

  return _result;
}

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_AllocateSocketObject, (XPADF_OUT    PXPADF_OBJECT               *_ppObject,
                                                                    XPADF_IN     int                          _nAddressFamily,
                                                                    XPADF_IN     int                          _nSocketType,
                                                                    XPADF_IN_OPT XPADF_PVOID                  _pContext,
                                                                    XPADF_IN_OPT PXPADFIOCleanupCallback      _pCleanupCallback,
                                                                    XPADF_IN_OPT PXPADFIOCallback             _pInternalReadCallback,
                                                                    XPADF_IN_OPT PXPADFIOCallback             _pInternalWriteCallback,
                                                                    XPADF_IN_OPT PXPADFIOCallback             _pExternalReadCallback,
                                                                    XPADF_IN_OPT PXPADFIOCallback             _pExternalWriteCallback)) {
  XPADF_RESULT _result = _xpadf_AllocateIOObject(_ppObject, (PXPADFCleanupObjectCallback)_xpadf_DestroySocketObject, sizeof(XPADF_IO), _pContext, _pCleanupCallback,
                                                 _pInternalReadCallback, _pInternalWriteCallback, _pExternalReadCallback, _pExternalWriteCallback);

  if(XPADF_SUCCEEDED(_result)) {
    PXPADF_IO _pIO = (PXPADF_IO)(*_ppObject);

    if((_pIO->m_hFD = socket(_nAddressFamily, _nSocketType, 0)) < 0)
      _result = XPADF_ERROR_CREATE_IO_OBJECT;
    else {
      if(XPADF_SUCCEEDED(_result = _xpadf_MakeSocketNonBlocking(_pIO)))
        return _result;

      close(_pIO->m_hFD);
    }

    free(*_ppObject);
  }

  return _result;
}

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_TCPSocketRead, (XPADF_IN PXPADF_IO _pIO)) {
  PXPADF_IO_OPERATION _pOperation = &_pIO->m_sRead;
  int                 _nRead      = recv(_pIO->m_hFD, _pOperation->m_pIterator, (size_t)(_pOperation->m_pLimit - _pOperation->m_pIterator), 0);

  if(_nRead > 0) {
    _pOperation->m_pIterator += _nRead;

    if(_pOperation->m_pIterator < _pOperation->m_pLimit)
      return _xpadf_TCPSocketRead(_pIO);
      
    return _pOperation->m_pReadCompleteCallback((XPADF_HANDLE)_pIO, _pOperation->m_pContext, &_pOperation->m_sBuffer);
  } else if(!_nRead) return XPADF_ERROR_IO_CONNECTION_CLOSED;
  else if((EAGAIN == errno) || (EWOULDBLOCK == errno)) {
    _xpadf_SetIOOperation(_pIO, XPADF_IO_OPERATION_READ);

    return XPADF_OK;
  } else return XPADF_ERROR_READ_IO;
}

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_TCPSocketWrite, (XPADF_IN PXPADF_IO _pIO)) {
  PXPADF_IO_OPERATION _pOperation = &_pIO->m_sWrite;
  int                 _nWritten   = send(_pIO->m_hFD, _pOperation->m_pIterator, (size_t)(_pOperation->m_pLimit - _pOperation->m_pIterator), 0);

  if(_nWritten > 0) {
    _pOperation->m_pIterator += _nWritten;

    if(_pOperation->m_pIterator < _pOperation->m_pLimit)
      return _xpadf_TCPSocketWrite(_pIO);

    return _pOperation->m_pWriteCompleteCallback((XPADF_HANDLE)_pIO, _pOperation->m_pContext);
  } else if((EAGAIN == errno) || (EWOULDBLOCK == errno)) {
    _xpadf_SetIOOperation(_pIO, XPADF_IO_OPERATION_WRITE);

    return XPADF_OK;
  } else return XPADF_ERROR_WRITE_IO;
}

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_TCPSocketAccept, (XPADF_IN PXPADF_IO _pIO)) {
  PXPADF_IO    _pNewIO = NULL;
  XPADF_RESULT _result = _xpadf_AllocateIOObject((PXPADF_OBJECT *)&_pNewIO, (PXPADFCleanupObjectCallback)_xpadf_DestroySocketObject, sizeof(XPADF_IO), _pIO->m_pContext,
                                                 _pIO->m_pCleanupCallback, _xpadf_TCPSocketRead, _xpadf_TCPSocketWrite, _xpadf_TCPSocketRead, _xpadf_TCPSocketWrite);

  if(XPADF_SUCCEEDED(_result)) {
    unsigned char _aAddress[sizeof(struct sockaddr_in) + sizeof(struct sockaddr_in6)];
    socklen_t     _nAddress = sizeof(_aAddress);

    if((_pNewIO->m_hFD = accept(_pIO->m_hFD, (struct sockaddr *)_aAddress, &_nAddress)) < 0) {
      if((EAGAIN == errno) || (EWOULDBLOCK == errno)) {
        _xpadf_SetIOOperation(_pIO, XPADF_IO_OPERATION_READ);

        _result = XPADF_OK;
      } else _result = XPADF_ERROR_ACCEPT_NEW_CONNECTION;
    } else {
      if(XPADF_SUCCEEDED(_result = _xpadf_MakeSocketNonBlocking(_pNewIO))) {
        if(XPADF_SUCCEEDED(_result = _pIO->m_sRead.m_pAcceptCompleteCallback((XPADF_HANDLE)_pIO, _pIO->m_sRead.m_pContext,
                                                                             _pNewIO, _aAddress, _nAddress))) {
          if(XPADF_SUCCEEDED(_result = _xpadf_EnableIOOperations(_pNewIO)))
            return _xpadf_TCPSocketAccept(_pIO);
        }
      }

      close(_pNewIO->m_hFD);
    }

    free(_pNewIO);
  }

  return _result;
}

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_TCPSocketConnect, (XPADF_IN PXPADF_IO _pIO)) {
  XPADF_RESULT _result;

  _pIO->m_sRead.m_pExternalCallback    =
    _pIO->m_sRead.m_pInternalCallback  = _xpadf_TCPSocketRead;
  _pIO->m_sWrite.m_pExternalCallback   =
    _pIO->m_sWrite.m_pInternalCallback = _xpadf_TCPSocketWrite;

  if(XPADF_SUCCEEDED(_result = _pIO->m_sWrite.m_pConnectCompleteCallback((XPADF_HANDLE)_pIO, _pIO->m_sWrite.m_pContext)))
    _result = _xpadf_EnableIOOperations(_pIO);

  return _result;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateTCPServerSocket, (XPADF_OUT    PXPADF_HANDLE               _phTCPServerSocket,
                                                           XPADF_IN     XPADF_SOCKET_ADDRESS_FAMILY _eAddressHint,
                                                           XPADF_IN_OPT XPADF_PORT                  _nBindPort,
                                                           XPADF_IN_OPT CXPADF_STRING               _szBindAddress,
                                                           XPADF_IN     XPADF_HANDLE                _hIOPoller,
                                                           XPADF_IN_OPT XPADF_PVOID                 _pContext,
                                                           XPADF_IN     PXPADFAcceptSocketCallback  _pAcceptCallback,
                                                           XPADF_IN_OPT PXPADFIOCleanupCallback     _pCleanupCallback)) {
  XPADF_RESULT _result;

  if(_phTCPServerSocket && _hIOPoller && _pAcceptCallback && (XPADF_SOCKET_ADDRESS_FAMILY_UNSPECIFIED >= _eAddressHint)) {
    PXPADF_IO _pIO = NULL;
    int       _on = 1;

    if(_szBindAddress) {
      size_t _nBindAddress = strlen(_szBindAddress);

      if(_nBindAddress) {
        char *_szAddress = (char *)malloc(_nBindAddress += 8);

        if(_szAddress) {
          struct addrinfo  _sHints;
          struct addrinfo *_pInfo  = NULL;
          char            *_szPort;

          if(strchr(_szBindAddress, ':'))
            strcpy(_szAddress, _szBindAddress);
          else
            snprintf(_szAddress, _nBindAddress, "%s:%hu", _szBindAddress, _nBindPort);

          *(_szPort = (char *)strchr(_szAddress, ':')) = 0;
          ++_szPort;
          
          memset(&_sHints, 0, sizeof(_sHints));
          
          switch(_eAddressHint) {
          case XPADF_SOCKET_ADDRESS_FAMILY_IPV4:
            _sHints.ai_family = AF_INET;
            break;
            
          case XPADF_SOCKET_ADDRESS_FAMILY_IPV6:
            _sHints.ai_family = AF_INET6;
            break;
            
          default:
            _sHints.ai_family = AF_UNSPEC;
          }
          
          _sHints.ai_socktype = SOCK_STREAM;
          _sHints.ai_flags    = AI_PASSIVE;
          
          if(getaddrinfo(_szAddress, _szPort, &_sHints, &_pInfo))
            _result = XPADF_ERROR_RESOLVE_ADDRESS;
          else {
            struct addrinfo *_i;

            _result = XPADF_ERROR_RESOLVE_ADDRESS;
            
            for(_i = _pInfo; _i; _i = _i->ai_next) {
              if(XPADF_SUCCEEDED(_result = _xpadf_AllocateSocketObject((PXPADF_OBJECT *)&_pIO, _i->ai_family, SOCK_STREAM, _pContext,
                                                                       _pCleanupCallback, _xpadf_TCPSocketAccept, NULL, NULL, NULL))) {
                socklen_t _nAddressSize;
                
                if(AF_INET == _i->ai_family)
                  _nAddressSize = sizeof(struct sockaddr_in);
                else
                  _nAddressSize = sizeof(struct sockaddr_in6);
                
                if(!setsockopt(_pIO->m_hFD, SOL_SOCKET, SO_REUSEADDR, (const char *)&_on, sizeof(_on))) {
                  if(!bind(_pIO->m_hFD, _i->ai_addr, _nAddressSize)) {
                    if(!listen(_pIO->m_hFD, SOMAXCONN)) {
                      _pIO->m_sRead.m_pContext                = _pContext;
                      _pIO->m_sRead.m_pAcceptCompleteCallback = _pAcceptCallback;

                      if(XPADF_SUCCEEDED(_result = _xpadf_RegisterIOWithIOPoller(_hIOPoller, (XPADF_HANDLE)_pIO, XPADF_IO_OPERATION_READ))) {
                        *_phTCPServerSocket = (XPADF_HANDLE)_pIO;
                        
                        free(_szAddress);
                        freeaddrinfo(_pInfo);
                        
                        return _result;
                      }
                    }
                  }
                }
                
                close(_pIO->m_hFD);
                free(_pIO);
              } else break;
            }
            
            freeaddrinfo(_pInfo);
          }

          free(_szAddress);
        } else _result = XPADF_ERROR_OUT_OF_MEMORY;

        if(_pCleanupCallback)
          _pCleanupCallback(NULL, _pContext, _result);
          
        return _result;
      }
    }

    if(XPADF_SOCKET_ADDRESS_FAMILY_IPV4 == _eAddressHint) {
      if(XPADF_SUCCEEDED(_result = _xpadf_AllocateSocketObject((PXPADF_OBJECT *)&_pIO, AF_INET, SOCK_STREAM, _pContext,
                                                               _pCleanupCallback, _xpadf_TCPSocketAccept, NULL, NULL, NULL))) {
        if(setsockopt(_pIO->m_hFD, SOL_SOCKET, SO_REUSEADDR, (const char *)&_on, sizeof(_on)) < 0)
          _result = XPADF_ERROR_SET_IO_ATTRIBUTES;
        else {
          struct sockaddr_in _sAddress;
          
          memset(&_sAddress, 0, sizeof(_sAddress));
          _sAddress.sin_family      = AF_INET;
          _sAddress.sin_port        = htons(_nBindPort);
          _sAddress.sin_addr.s_addr = INADDR_ANY;
          
          _pIO->m_sRead.m_pContext                = _pContext;
          _pIO->m_sRead.m_pAcceptCompleteCallback = _pAcceptCallback;

          if(bind(_pIO->m_hFD, (struct sockaddr *)&_sAddress, sizeof(_sAddress)) < 0)
            _result = XPADF_ERROR_BIND_PORT;
          else if(listen(_pIO->m_hFD, SOMAXCONN) < 0)
            _result = XPADF_ERROR_SOCKET_LISTEN;
          else if(XPADF_SUCCEEDED(_result = _xpadf_RegisterIOWithIOPoller(_hIOPoller, (XPADF_HANDLE)_pIO, XPADF_IO_OPERATION_READ))) {
            *_phTCPServerSocket = (XPADF_HANDLE)_pIO;

            return _result;
          }
        }

        close(_pIO->m_hFD);
        free(_pIO);
      }
    } else if(XPADF_SOCKET_ADDRESS_FAMILY_IPV6 == _eAddressHint) {
      if(XPADF_SUCCEEDED(_result = _xpadf_AllocateSocketObject((PXPADF_OBJECT *)&_pIO, AF_INET6, SOCK_STREAM, _pContext,
                                                               _pCleanupCallback, _xpadf_TCPSocketAccept, NULL, NULL, NULL))) {
        if(setsockopt(_pIO->m_hFD, SOL_SOCKET, SO_REUSEADDR, (const char *)&_on, sizeof(_on)) < 0)
          _result = XPADF_ERROR_SET_IO_ATTRIBUTES;
        else {
          struct sockaddr_in6 _sAddress;
          
          memset(&_sAddress, 0, sizeof(_sAddress));
          _sAddress.sin6_family = AF_INET6;
          _sAddress.sin6_port   = htons(_nBindPort);
          memcpy(&_sAddress.sin6_addr, &in6addr_any, sizeof(_sAddress.sin6_addr));
          
          _pIO->m_sRead.m_pContext                = _pContext;
          _pIO->m_sRead.m_pAcceptCompleteCallback = _pAcceptCallback;

          if(bind(_pIO->m_hFD, (struct sockaddr *)&_sAddress, sizeof(_sAddress)) < 0)
            _result = XPADF_ERROR_BIND_PORT;
          else if(listen(_pIO->m_hFD, SOMAXCONN) < 0)
            _result = XPADF_ERROR_SOCKET_LISTEN;
          else if(XPADF_SUCCEEDED(_result = _xpadf_RegisterIOWithIOPoller(_hIOPoller, (XPADF_HANDLE)_pIO, XPADF_IO_OPERATION_READ))) {
            *_phTCPServerSocket = (XPADF_HANDLE)_pIO;

            return _result;
          }
        }

        close(_pIO->m_hFD);
        free(_pIO);
      }
    } else {
      if(XPADF_SUCCEEDED(_result = xpadf_CreateTCPServerSocket(_phTCPServerSocket, XPADF_SOCKET_ADDRESS_FAMILY_IPV6, _nBindPort,
                                                               _szBindAddress, _hIOPoller, _pContext, _pAcceptCallback, NULL)) ||
         XPADF_SUCCEEDED(_result = xpadf_CreateTCPServerSocket(_phTCPServerSocket, XPADF_SOCKET_ADDRESS_FAMILY_IPV4, _nBindPort,
                                                               _szBindAddress, _hIOPoller, _pContext, _pAcceptCallback, NULL))) {
        ((PXPADF_IO)(*_phTCPServerSocket))->m_pCleanupCallback = _pCleanupCallback;

        return _result;
      }
    }
  } else _result = XPADF_ERROR_INVALID_PARAMETERS;

  if(_pCleanupCallback)
    _pCleanupCallback(NULL, _pContext, _result);

  return _result;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateTCPClientSocket, (XPADF_IN     XPADF_SOCKET_ADDRESS_FAMILY _eAddressHint,
                                                           XPADF_IN_OPT XPADF_PORT                  _nServerPort,
                                                           XPADF_IN     CXPADF_STRING               _szServerAddress,
                                                           XPADF_IN     XPADF_HANDLE                _hIOPoller,
                                                           XPADF_IN_OPT XPADF_PVOID                 _pContext,
                                                           XPADF_IN     PXPADFConnectSocketCallback _pConnectCallback,
                                                           XPADF_IN_OPT PXPADFIOCleanupCallback     _pCleanupCallback)) {
  XPADF_RESULT _result;

  if((XPADF_SOCKET_ADDRESS_FAMILY_UNSPECIFIED >= _eAddressHint) && _szServerAddress && _hIOPoller && _pConnectCallback) {
    size_t _nServerAddress = strlen(_szServerAddress);

    if(_nServerAddress) {
      char *_szAddress = (char *)malloc(_nServerAddress += 8);

      if(_szAddress) {
        do {
          struct addrinfo  _sHints;
          struct addrinfo *_pInfo  = NULL;
          char            *_szPort;

          if(strchr(_szServerAddress, ':'))
            strcpy(_szAddress, _szServerAddress);
          else if(_nServerPort)
            snprintf(_szAddress, _nServerAddress, "%s:%hu", _szServerAddress, _nServerPort);
          else {
            _result = XPADF_ERROR_INVALID_PARAMETERS;

            break;
          }

          *(_szPort = (char *)strchr(_szAddress, ':')) = 0;
          ++_szPort;

          memset(&_sHints, 0, sizeof(_sHints));
          
          switch(_eAddressHint) {
          case XPADF_SOCKET_ADDRESS_FAMILY_IPV4:
            _sHints.ai_family = AF_INET;
            break;
            
          case XPADF_SOCKET_ADDRESS_FAMILY_IPV6:
            _sHints.ai_family = AF_INET6;
            break;
            
          default:
            _sHints.ai_family = AF_UNSPEC;
          }
          
          _sHints.ai_socktype = SOCK_STREAM;
          _sHints.ai_flags    = AI_PASSIVE;

          if(getaddrinfo(_szAddress, _szPort, &_sHints, &_pInfo))
            _result = XPADF_ERROR_RESOLVE_ADDRESS;
          else {
            PXPADF_IO        _pIO;
            struct addrinfo *_i;

            _result = XPADF_ERROR_RESOLVE_ADDRESS;

            for(_i = _pInfo; _i; _i = _i->ai_next) {
              if(XPADF_SUCCEEDED(_result = _xpadf_AllocateSocketObject((PXPADF_OBJECT *)&_pIO, AF_INET, SOCK_STREAM, _pContext,
                                                                       _pCleanupCallback, _xpadf_TCPSocketConnect, NULL, NULL, NULL))) {
                _pIO->m_sWrite.m_pInternalCallback = _xpadf_TCPSocketConnect;

                if(XPADF_SUCCEEDED(_result = _xpadf_RegisterIOWithIOPoller(_hIOPoller, (XPADF_HANDLE)_pIO, XPADF_IO_OPERATION_WRITE))) {
                  socklen_t _nAddressSize;
                  
                  if(AF_INET == _i->ai_family)
                    _nAddressSize = sizeof(struct sockaddr_in);
                  else
                    _nAddressSize = sizeof(struct sockaddr_in6);
                  
                  if((connect(_pIO->m_hFD, _i->ai_addr, _nAddressSize) >= 0) ||
                     (EINPROGRESS == errno)) {
                    freeaddrinfo(_pInfo);
                    free(_szAddress);

                    _pIO->m_sWrite.m_pContext                 = _pContext;
                    _pIO->m_sWrite.m_pConnectCompleteCallback = _pConnectCallback;

                    return XPADF_OK;
                  } else _result = XPADF_ERROR_CONNECT_SOCKET;
                }

                close(_pIO->m_hFD);
                free(_pIO);
              }
            }

            freeaddrinfo(_pInfo);
          }
        } while(0);

        free(_szAddress);
      } else _result = XPADF_ERROR_OUT_OF_MEMORY;
    } else _result = XPADF_ERROR_INVALID_PARAMETERS;
  } else _result = XPADF_ERROR_INVALID_PARAMETERS;

  if(_pCleanupCallback)
    _pCleanupCallback(NULL, _pContext, _result);

  return _result;
}
