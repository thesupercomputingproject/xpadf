#if !defined(__52342E196EA8F7D9846AD6044E0__)
# define __52342E196EA8F7D9846AD6044E0__

# include <xpadf/io/io.h>

# include <xpadf/internal/common/objects.h>

# if !defined(XPADF_MAXIMUM_IO_EVENTS)
#  define XPADF_MAXIMUM_IO_EVENTS 64
# endif /* !XPADF_MAXIMUM_IO_EVENTS */

# define XPADF_IO_OPERATION_READ  0x000000001
# define XPADF_IO_OPERATION_WRITE 0x000000002

XPADF_DECLARE_STRUCT(XPADF_IO);
XPADF_DECLARE_STRUCT(XPADF_IO_OPERATION);

XPADF_CALLBACK(XPADF_RESULT, PXPADFInternalIOCallback, (XPADF_IN PXPADF_IO _pIO));

struct XPADF_IO_OPERATION {
  PXPADFInternalIOCallback m_pCallback;
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

#endif /* !__52342E196EA8F7D9846AD6044E0__ */
