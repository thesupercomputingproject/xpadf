#if !defined(__52342E196EA8F7D9846AD6044E0__)
# define __52342E196EA8F7D9846AD6044E0__

# include <xpadf/io/io.h>
# include <xpadf/io/socket.h>

# include <xpadf/internal/common/objects.h>

# if !defined(XPADF_MAXIMUM_IO_EVENTS)
#  define XPADF_MAXIMUM_IO_EVENTS 64
# endif /* !XPADF_MAXIMUM_IO_EVENTS */

# define XPADF_IO_OPERATION_READ  0x000000001
# define XPADF_IO_OPERATION_WRITE 0x000000002

XPADF_DECLARE_STRUCT(XPADF_IO);
XPADF_DECLARE_STRUCT(XPADF_IO_OPERATION);

XPADF_CALLBACK(XPADF_RESULT, PXPADFIOCallback, (XPADF_IN PXPADF_IO _pIO));

struct XPADF_IO_OPERATION {
  XPADF_PVOID                   m_pContext;

  XPADF_BUFFER64                m_sBuffer;

  PXPADF_BYTE                   m_pIterator;
  PXPADF_BYTE                   m_pLimit;

  union {
    PXPADFIOReadCallback        m_pReadCompleteCallback;
    PXPADFIOWriteCallback       m_pWriteCompleteCallback;
    PXPADFAcceptSocketCallback  m_pAcceptCompleteCallback;
    PXPADFConnectSocketCallback m_pConnectCompleteCallback;
  };

  PXPADFIOCallback              m_pExternalCallback;
  PXPADFIOCallback              m_pInternalCallback;
};

struct XPADF_IO {
  XPADF_OBJECT            m_sObject;

  PXPADF_IO               m_pPrevious;
  PXPADF_IO               m_pNext;

  XPADF_HANDLE            m_hIOPoller;

  int                     m_hFD;

  XPADF_ULONG             m_nOperations;

  XPADF_IO_OPERATION      m_sRead;
  XPADF_IO_OPERATION      m_sWrite;

  XPADF_PVOID             m_pContext;
  PXPADFIOCleanupCallback m_pCleanupCallback;
};

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_AllocateIOObject, (XPADF_OUT    PXPADF_OBJECT               *_ppObject,
                                                                XPADF_IN     PXPADFCleanupObjectCallback  _pCleanupObjectCallback,
                                                                XPADF_IN     XPADF_SIZE                   _nSize,
                                                                XPADF_IN_OPT XPADF_PVOID                  _pContext,
                                                                XPADF_IN_OPT PXPADFIOCleanupCallback      _pCleanupCallback,
                                                                XPADF_IN     PXPADFIOCallback             _pInternalReadCallback,
                                                                XPADF_IN     PXPADFIOCallback             _pInternalWriteCallback,
                                                                XPADF_IN_OPT PXPADFIOCallback             _pExternalReadCallback,
                                                                XPADF_IN_OPT PXPADFIOCallback             _pExternalWriteCallback));
XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_InitializeIOBuffer, (XPADF_IN PXPADF_IO_OPERATION _pOperation,
                                                                  XPADF_IN XPADF_SIZE64        _nSize));
XPADF_INTERNAL_FUNCTION(void, _xpadf_SetIOOperation, (XPADF_INOUT PXPADF_IO   _pIO,
                                                      XPADF_IN    XPADF_ULONG _nOperation));

#endif /* !__52342E196EA8F7D9846AD6044E0__ */
