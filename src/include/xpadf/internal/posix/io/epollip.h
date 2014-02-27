#if !defined(__90C31F604E24381F4E9BC43892A__)
# define __90C31F604E24381F4E9BC43892A__

# include <pthread.h>

# include <xpadf/io/iopoller.h>

# include <xpadf/internal/common/objects.h>

# include <xpadf/internal/posix/io/io.h>

XPADF_DECLARE_STRUCT(XPADF_IO_POLLER);
XPADF_DECLARE_STRUCT(XPADF_IO_POLLER_EVENT);

struct XPADF_IO_POLLER_EVENT {
  PXPADF_IO_POLLER_EVENT m_pNext;

  XPADF_ULONG            m_nOperations;

  PXPADF_IO              m_pIO;
};

struct XPADF_IO_POLLER {
  XPADF_OBJECT           m_sObject;

  XPADF_BOOL             m_bRunning;

  pthread_mutex_t        m_hLock;

  int                    m_hEPoll;

  PXPADF_IO              m_pIOListHead;

  PXPADF_IO_POLLER_EVENT m_pFreeEventListHead;
};

#endif /* !__90C31F604E24381F4E9BC43892A__ */
