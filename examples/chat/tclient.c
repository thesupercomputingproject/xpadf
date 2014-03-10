#include <string.h>

#include <xpadf/io/stdio.h>
#include <xpadf/io/tsocket.h>

#include <xpadf/thread/looper.h>

#include "protocol.h"

XPADF_DECLARE_STRUCT(CLIENT_CONTEXT);

struct CLIENT_CONTEXT {
  XPADF_BOOL   m_bRunning;

  XPADF_HANDLE m_hIOPoller;
  XPADF_HANDLE m_hClientSocket;
  XPADF_HANDLE m_hLooper;
};

XPADF_STATIC_FUNCTION(XPADF_RESULT, _readProtocolHeader, (XPADF_IN     XPADF_HANDLE    _hIO,
                                                          XPADF_IN_OPT PCLIENT_CONTEXT _pContext,
                                                          XPADF_IN_OPT PXPADF_BUFFER64 _pBuffer));

XPADF_STATIC_FUNCTION(XPADF_RESULT, _readMessage, (XPADF_IN     XPADF_HANDLE    _hIO,
                                                   XPADF_IN_OPT PCLIENT_CONTEXT _pContext,
                                                   XPADF_IN_OPT PXPADF_BUFFER64 _pBuffer)) {
  const char *_szMessage = (const char *)_pBuffer->m_pBuffer;

  XPADF_FPRINTF(stdout, ">> %s\n >> ", _szMessage);

  return xpadf_IORead(_hIO, sizeof(PROTOCOL_HEADER), _pContext, (PXPADFIOReadCallback)_readProtocolHeader);
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _readProtocolHeader, (XPADF_IN     XPADF_HANDLE    _hIO,
                                                          XPADF_IN_OPT PCLIENT_CONTEXT _pContext,
                                                          XPADF_IN_OPT PXPADF_BUFFER64 _pBuffer)) {
  PPROTOCOL_HEADER _pHeader = (PPROTOCOL_HEADER)_pBuffer->m_pBuffer;

  return xpadf_IORead(_hIO, _pHeader->m_nPacketSize, _pContext, (PXPADFIOReadCallback)_readMessage);
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _connectCallback, (XPADF_IN     XPADF_HANDLE    _hClientSocket,
                                                       XPADF_IN_OPT PCLIENT_CONTEXT _pContext)) {
  if(XPADF_SUCCEEDED(xpadf_IORead(_hClientSocket, sizeof(PROTOCOL_HEADER), _pContext, (PXPADFIOReadCallback)_readProtocolHeader)))
    _pContext->m_hClientSocket = _hClientSocket;

  return xpadf_DestroyObject(_hClientSocket);
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _pollSocket, (XPADF_IN     XPADF_HANDLE    _hLooper,
                                                  XPADF_IN_OPT PCLIENT_CONTEXT _pContext)) {
  return xpadf_PollIO(_pContext->m_hIOPoller, NULL);
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _stopLooper, (XPADF_IN     XPADF_HANDLE    _hLooper,
                                                  XPADF_IN_OPT PCLIENT_CONTEXT _pContext)) {
  return xpadf_StopIOPoller(_pContext->m_hIOPoller);
}

XPADF_STATIC_FUNCTION(XPADF_RESULT, _dummyWrite, (XPADF_IN     XPADF_HANDLE _hIO,
                                                  XPADF_IN_OPT XPADF_PVOID  _pContext)) {
  return XPADF_OK;
}

XPADF_STATIC_FUNCTION(void, _socketCleanup, (XPADF_IN     XPADF_HANDLE    _hIO,
                                             XPADF_IN_OPT PCLIENT_CONTEXT _pContext,
                                             XPADF_IN     XPADF_RESULT    _nStatus)) {
  if(_pContext->m_bRunning)
    XPADF_FPRINTF(stdout, "INFO: The server closed the connection\n");
}

int main(int _argc, char *_argv[]) {
  CLIENT_CONTEXT _sContext;
  XPADF_RESULT   _result;

  memset(&_sContext, 0, sizeof(_sContext));

  if(XPADF_SUCCEEDED(_result = xpadf_CreateIOPoller(&_sContext.m_hIOPoller))) {
    if(XPADF_SUCCEEDED(_result = xpadf_CreateTCPClientSocket(XPADF_SOCKET_ADDRESS_FAMILY_IPV4, 0, _argv[1], _sContext.m_hIOPoller, &_sContext,
                                                             (PXPADFConnectSocketCallback)_connectCallback, (PXPADFIOCleanupCallback)_socketCleanup))) {
      if(XPADF_SUCCEEDED(_result = xpadf_CreateLooper(&_sContext.m_hLooper, &_sContext, NULL, (PXPADFThreadCallback)_pollSocket,
                                                      (PXPADFThreadCallback)_stopLooper, NULL))) {
        unsigned char     _aBuffer[sizeof(PROTOCOL_HEADER) + 512];
        PPROTOCOL_HEADER  _pProtocolHeader = (PPROTOCOL_HEADER)_aBuffer;
        char             *_szMessage       = (char *)(_pProtocolHeader + 1);
        int               _nMessage        = sizeof(_aBuffer) - sizeof(PROTOCOL_HEADER);
        XPADF_BUFFER64    _sBuffer         = {_aBuffer};

        _sContext.m_bRunning = XPADF_TRUE;

        for(;;) {
          char         *_pDelimiter;
          XPADF_RESULT  _result;

          XPADF_FPRINTF(stdout, " >> ");

          fgets(_szMessage, _nMessage, stdin);

          if((_pDelimiter = strchr(_szMessage, '\n')))
            *_pDelimiter = 0;

          if(strcasecmp(_szMessage, "exit")) {
            _pProtocolHeader->m_nPacketSize = (XPADF_SIZE)strlen(_szMessage) + 1;

            _sBuffer.m_nCapacity =
              _sBuffer.m_nSize   = _pProtocolHeader->m_nPacketSize + sizeof(PROTOCOL_HEADER);

            if(XPADF_FAILED(_result = xpadf_IOWrite(_sContext.m_hClientSocket, &_sBuffer, NULL, (PXPADFIOWriteCallback)_dummyWrite)))
              XPADF_FPRINTF(stderr, " >> ERROR: Unable to send data to the server [Error: 0x0%08x]\n", _result);
          } else break;
        }

        _sContext.m_bRunning = XPADF_FALSE;

        xpadf_DestroyObject(_sContext.m_hLooper);
      }

      if(_sContext.m_hClientSocket)
        xpadf_DestroyObject(_sContext.m_hClientSocket);
    }

    if(_sContext.m_hIOPoller)
      xpadf_DestroyObject(_sContext.m_hIOPoller);
  }

  return (int)_result;
}
