#include <string.h>
#include <malloc.h>

#include <xpadf/internal/posix/io/io.h>

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_AllocateIOObject, (XPADF_OUT    PXPADF_OBJECT               *_ppObject,
                                                                XPADF_IN     PXPADFCleanupObjectCallback  _pCleanupObjectCallback,
                                                                XPADF_IN     XPADF_SIZE                   _nSize,
                                                                XPADF_IN_OPT XPADF_PVOID                  _pContext,
                                                                XPADF_IN_OPT PXPADFIOCleanupCallback      _pCleanupCallback,
                                                                XPADF_IN     PXPADFIOCallback             _pInternalReadCallback,
                                                                XPADF_IN     PXPADFIOCallback             _pInternalWriteCallback,
                                                                XPADF_IN_OPT PXPADFIOCallback             _pExternalReadCallback,
                                                                XPADF_IN_OPT PXPADFIOCallback             _pExternalWriteCallback)) {
  if(sizeof(XPADF_IO) <= _nSize) {
    XPADF_RESULT _result = _xpadf_AllocateObject(_ppObject, XPADF_OBJECT_TYPE_IO, _pCleanupObjectCallback, _nSize);

    if(XPADF_SUCCEEDED(_result)) {
      PXPADF_IO _pIO = (PXPADF_IO)(*_ppObject);

      _pIO->m_sRead.m_pInternalCallback  = _pInternalReadCallback;
      _pIO->m_sWrite.m_pInternalCallback = _pInternalWriteCallback;
      _pIO->m_sRead.m_pExternalCallback  = _pExternalReadCallback;
      _pIO->m_sWrite.m_pExternalCallback = _pExternalWriteCallback;
      _pIO->m_pContext                   = _pContext;
      _pIO->m_pCleanupCallback           = _pCleanupCallback;
    }

    return _result;
  } return XPADF_ERROR_INVALID_PARAMETERS;
}

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_InitializeIOBuffer, (XPADF_IN PXPADF_IO_OPERATION _pOperation,
                                                                  XPADF_IN XPADF_SIZE64        _nSize)) {
  if(_pOperation->m_sBuffer.m_nCapacity < _nSize)
    _pOperation->m_sBuffer.m_pBuffer = realloc(_pOperation->m_sBuffer.m_pBuffer, _pOperation->m_sBuffer.m_nCapacity = _nSize);

  if(_pOperation->m_sBuffer.m_pBuffer) {
    _pOperation->m_sBuffer.m_nSize = _nSize;

    _pOperation->m_pLimit = (_pOperation->m_pIterator = (PXPADF_BYTE)_pOperation->m_sBuffer.m_pBuffer) + _nSize;

    return XPADF_OK;
  } return XPADF_ERROR_OUT_OF_MEMORY;
}

XPADF_INTERNAL_FUNCTION(void, _xpadf_SetIOOperation, (XPADF_INOUT PXPADF_IO   _pIO,
                                                      XPADF_IN    XPADF_ULONG _nOperation)) {
  _pIO->m_nOperations |= _nOperation;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_UpdateIOCleanupCallback, (XPADF_IN     XPADF_HANDLE            _hIO,
                                                             XPADF_IN_OPT XPADF_PVOID             _pContext,
                                                             XPADF_IN_OPT PXPADFIOCleanupCallback _pCleanupCallback)) {
  if(_hIO) {
    PXPADF_IO _pIO = (PXPADF_IO)_hIO;

    if(XPADF_OBJECT_TYPE_IO == _pIO->m_sObject.m_eType) {
      _pIO->m_pContext         = _pContext;
      _pIO->m_pCleanupCallback = _pCleanupCallback;

      return XPADF_OK;
    }
  } return XPADF_ERROR_INVALID_PARAMETERS;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_IORead, (XPADF_IN     XPADF_HANDLE         _hIO,
                                            XPADF_IN_OPT XPADF_SIZE           _nSize,
                                            XPADF_IN_OPT XPADF_PVOID          _pContext,
                                            XPADF_IN     PXPADFIOReadCallback _pReadCompleteCallback)) {
  XPADF_RESULT _result = XPADF_ERROR_INVALID_PARAMETERS;

  if(_hIO && _pReadCompleteCallback) {
    PXPADF_IO _pIO = (PXPADF_IO)_hIO;

    if(XPADF_OBJECT_TYPE_IO == _pIO->m_sObject.m_eType) {
      if(_pIO->m_sRead.m_pExternalCallback) {
        if(XPADF_SUCCEEDED(_result = _xpadf_InitializeIOBuffer(&_pIO->m_sRead, _nSize))) {
          _pIO->m_sRead.m_pContext              = _pContext;
          _pIO->m_sRead.m_pReadCompleteCallback = _pReadCompleteCallback;
          
          return _pIO->m_sRead.m_pExternalCallback(_hIO);
        }
      } else _result = XPADF_ERROR_INVALID_OPERATION;
    }
  }

  return _result;
}

XPADF_FUNCTION(XPADF_RESULT, xpadf_IOWrite, (XPADF_IN     XPADF_HANDLE          _hIO,
                                             XPADF_IN     PXPADF_BUFFER64       _pBuffer,
                                             XPADF_IN_OPT XPADF_PVOID           _pContext,
                                             XPADF_IN     PXPADFIOWriteCallback _pWriteCompleteCallback)) {
  XPADF_RESULT _result = XPADF_ERROR_INVALID_PARAMETERS;

  if(_hIO && _pBuffer && _pBuffer->m_nSize && (_pBuffer->m_nSize <= _pBuffer->m_nCapacity) && _pWriteCompleteCallback) {
    PXPADF_IO _pIO = (PXPADF_IO)_hIO;

    if(XPADF_OBJECT_TYPE_IO == _pIO->m_sObject.m_eType) {
      if(_pIO->m_sWrite.m_pExternalCallback) {
        if(XPADF_SUCCEEDED(_result = _xpadf_InitializeIOBuffer(&_pIO->m_sWrite, _pBuffer->m_nSize))) {
          memcpy(_pIO->m_sWrite.m_sBuffer.m_pBuffer, _pBuffer->m_pBuffer, _pBuffer->m_nSize);

          _pIO->m_sWrite.m_pContext               = _pContext;
          _pIO->m_sWrite.m_pWriteCompleteCallback = _pWriteCompleteCallback;

          return _pIO->m_sWrite.m_pExternalCallback(_hIO);
        }
      } else _result = XPADF_ERROR_INVALID_OPERATION;
    }
  }

  return _result;
}
