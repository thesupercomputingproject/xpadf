#include <string.h>
#include <malloc.h>

#include <xpadf/io/stdio.h>
#include <xpadf/io/iopoller.h>
#include <xpadf/io/tsocket.h>

#include <xpadf/thread/tpool.h>
#include <xpadf/thread/looper.h>

#include <xpadf/sync/locks.h>

#include "protocol.h"

#define MINIMUM_WORKERS 4
#define MAXIMUM_WORKERS 8

XPADF_DECLARE_STRUCT(SERVER_CONTEXT);
XPADF_DECLARE_STRUCT(CLIENT_CONNECTION);

struct CLIENT_CONNECTION {
  PCLIENT_CONNECTION m_pPrevious;
  PCLIENT_CONNECTION m_pNext;

  PSERVER_CONTEXT    m_pContext;
  XPADF_HANDLE       m_hIO;
};

struct SERVER_CONTEXT {
  XPADF_HANDLE       m_hLock;
  XPADF_HANDLE       m_hIOPoller;
  XPADF_HANDLE       m_hThreadPool;
  XPADF_HANDLE       m_hLooper;
  XPADF_HANDLE       m_hServerSocketIPv6;
  XPADF_HANDLE       m_hServerSocketIPv4;

  PCLIENT_CONNECTION m_pConnectionListHead;
};

XPADF_STATIC_FUNCTION(XPADF_RESULT, _readProtocolHeader, (XPADF_IN     XPADF_HANDLE       _hIO,
                                                          XPADF_IN_OPT PCLIENT_CONNECTION _pConnection,
                                                          XPADF_IN_OPT PXPADF_BUFFER64    _pBuffer));

XPADF_STATIC_FUNCTION(XPADF_RESULT, _pollSockets, (XPADF_IN     XPADF_HANDLE    _hLooper,
                                                   XPADF_IN_OPT PSERVER_CONTEXT _pContext)) {
  return xpadf_PollIO(_pContext->m_hIOPoller, _pContext->m_hThreadPool);
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _stopLooper, (XPADF_IN     XPADF_HANDLE    _hLooper,
                                                  XPADF_IN_OPT PSERVER_CONTEXT _pContext)) {
  return xpadf_StopIOPoller(_pContext->m_hIOPoller);
}

XPADF_STATIC_FUNCTION(void, _cleanupConnection, (XPADF_IN     XPADF_HANDLE       _hIO,
                                                 XPADF_IN_OPT PCLIENT_CONNECTION _pConnection,
                                                 XPADF_IN     XPADF_RESULT       _nStatus)) {
  PSERVER_CONTEXT _pContext = _pConnection->m_pContext;

  if(XPADF_SUCCEEDED(xpadf_Lock(_pContext->m_hLock))) {
    if(_pConnection->m_pNext)
      _pConnection->m_pNext->m_pPrevious = _pConnection->m_pPrevious;

    if(_pConnection->m_pPrevious)
      _pConnection->m_pPrevious->m_pNext = _pConnection->m_pNext;
    else
      _pContext->m_pConnectionListHead = _pConnection->m_pNext;

    xpadf_UnLock(_pContext->m_hLock);

    free(_pConnection);
  }
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _dummyWrite, (XPADF_IN     XPADF_HANDLE _hIO,
                                                  XPADF_IN_OPT XPADF_PVOID  _pContext)) {
  return XPADF_OK;
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _readMessage, (XPADF_IN     XPADF_HANDLE       _hIO,
                                                   XPADF_IN_OPT PCLIENT_CONNECTION _pConnection,
                                                   XPADF_IN_OPT PXPADF_BUFFER64    _pBuffer)) {
  PSERVER_CONTEXT _pContext = _pConnection->m_pContext;

  if(XPADF_SUCCEEDED(xpadf_Lock(_pContext->m_hLock))) {
    PCLIENT_CONNECTION _i       = _pContext->m_pConnectionListHead;
    PROTOCOL_HEADER    _sHeader = {_pBuffer->m_nSize};
    XPADF_BUFFER64     _sBuffer = {&_sHeader, sizeof(_sHeader), sizeof(_sHeader)};

    while(_i) {
      PCLIENT_CONNECTION _pNext = _i->m_pNext;

      if(_i != _pConnection) {
        XPADF_HANDLE _hPeer = _i->m_hIO;
        
        xpadf_UnLock(_pContext->m_hLock);
        
        if(XPADF_SUCCEEDED(xpadf_IOWrite(_hPeer, &_sBuffer, NULL, _dummyWrite)))
          xpadf_IOWrite(_hPeer, _pBuffer, NULL, _dummyWrite);
        
        xpadf_Lock(_pContext->m_hLock);
      }
        
      _i = _pNext;
    }

    xpadf_UnLock(_pContext->m_hLock);
  }

  return xpadf_IORead(_hIO, sizeof(PROTOCOL_HEADER), _pConnection, (PXPADFIOReadCallback)_readProtocolHeader);
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _readProtocolHeader, (XPADF_IN     XPADF_HANDLE       _hIO,
                                                          XPADF_IN_OPT PCLIENT_CONNECTION _pConnection,
                                                          XPADF_IN_OPT PXPADF_BUFFER64    _pBuffer)) {
  PPROTOCOL_HEADER _pHeader = (PPROTOCOL_HEADER)_pBuffer->m_pBuffer;

  return xpadf_IORead(_hIO, _pHeader->m_nPacketSize, _pConnection, (PXPADFIOReadCallback)_readMessage);
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _acceptConnection, (XPADF_IN     XPADF_HANDLE    _hServerSocket,
                                                        XPADF_IN_OPT PSERVER_CONTEXT _pContext,
                                                        XPADF_IN     XPADF_HANDLE    _hNewConnection,
                                                        XPADF_IN     XPADF_PVOID     _pAddress,
                                                        XPADF_IN     XPADF_SIZE      _nAddress)) {
  if(XPADF_SUCCEEDED(xpadf_Lock(_pContext->m_hLock))) {
    PCLIENT_CONNECTION _pConnection = (PCLIENT_CONNECTION)malloc(sizeof(CLIENT_CONNECTION));

    if(_pConnection) {
      _pConnection->m_pPrevious = NULL;

      if((_pConnection->m_pNext = _pContext->m_pConnectionListHead))
        _pConnection->m_pNext->m_pPrevious = _pConnection;

      _pContext->m_pConnectionListHead = _pConnection;
      _pConnection->m_pContext = _pContext;
      _pConnection->m_hIO      = _hNewConnection;

      if(XPADF_SUCCEEDED(xpadf_UpdateIOCleanupCallback(_hNewConnection, _pConnection, (PXPADFIOCleanupCallback)_cleanupConnection))) {
        if(XPADF_SUCCEEDED(xpadf_RegisterIOWithIOPoller(_pContext->m_hIOPoller, _hNewConnection))) {
          if(XPADF_SUCCEEDED(xpadf_IORead(_hNewConnection, sizeof(PROTOCOL_HEADER), _pConnection, (PXPADFIOReadCallback)_readProtocolHeader))) {
            xpadf_UnLock(_pContext->m_hLock);
            
            return xpadf_DestroyObject(_hNewConnection);
          }
        }
      }

      if((_pContext->m_pConnectionListHead = _pConnection->m_pNext))
        _pConnection->m_pNext->m_pPrevious = NULL;

      free(_pConnection);
    }

    xpadf_UnLock(_pContext->m_hLock);
  }

  return xpadf_DestroyObject(_hNewConnection);
}

int main(int _argc, char *_argv[]) {
  SERVER_CONTEXT _sContext;
  XPADF_RESULT   _result;

  memset(&_sContext, 0, sizeof(_sContext));

  if(XPADF_SUCCEEDED(_result = xpadf_CreateLock(&_sContext.m_hLock))) {
    if(XPADF_SUCCEEDED(_result = xpadf_CreateThreadPool(&_sContext.m_hThreadPool, MINIMUM_WORKERS, MAXIMUM_WORKERS, &_sContext, NULL))) {
      if(XPADF_SUCCEEDED(_result = xpadf_CreateIOPoller(&_sContext.m_hIOPoller))) {
        if(XPADF_SUCCEEDED(_result = xpadf_CreateLooper(&_sContext.m_hLooper, &_sContext, NULL, (PXPADFThreadCallback)_pollSockets, (PXPADFThreadCallback)_stopLooper, NULL))) {
          if(XPADF_SUCCEEDED(_result = xpadf_CreateTCPServerSocket(&_sContext.m_hServerSocketIPv6, XPADF_SOCKET_ADDRESS_FAMILY_IPV6, 0, _argv[1],
                                                                   _sContext.m_hIOPoller, &_sContext, (PXPADFAcceptSocketCallback)_acceptConnection, NULL))) {
            if(XPADF_SUCCEEDED(_result = xpadf_CreateTCPServerSocket(&_sContext.m_hServerSocketIPv4, XPADF_SOCKET_ADDRESS_FAMILY_IPV4, 0, _argv[1],
                                                                     _sContext.m_hIOPoller, &_sContext, (PXPADFAcceptSocketCallback)_acceptConnection, NULL))) {
              XPADF_FPRINTF(stdout, "Press <return> to stop the server...");
              getchar();
              
              xpadf_DestroyObject(_sContext.m_hServerSocketIPv4);
            }
            
            xpadf_DestroyObject(_sContext.m_hServerSocketIPv6);
          }
          
          xpadf_DestroyObject(_sContext.m_hLooper);
        }
        
        xpadf_DestroyObject(_sContext.m_hIOPoller);
      }
      
      xpadf_DestroyObject(_sContext.m_hThreadPool);
    }

    xpadf_DestroyObject(_sContext.m_hLock);
  }

  return (int)_result;
}
