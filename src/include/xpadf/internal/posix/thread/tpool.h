#if !defined(__049D664256EEB0E21CEA74D59AB__)
# define __049D664256EEB0E21CEA74D59AB__

# include <pthread.h>
# include <semaphore.h>

# include <xpadf/thread/tpool.h>

# include <xpadf/internal/posix/thread/thread.h>

XPADF_DECLARE_STRUCT(XPADF_THREAD_POOL);
XPADF_DECLARE_STRUCT(XPADF_THREAD_POOL_WORK);

struct XPADF_THREAD_POOL_WORK {
  PXPADF_THREAD_POOL_WORK     m_pNext;

  XPADF_BOOL                  m_bPermanent;
  XPADF_PVOID                 m_pContext;
  PXPADFThreadCallback        m_pWorkCallback;
  PXPADFThreadCleanupCallback m_pCleanupCallback;
};

struct XPADF_THREAD_POOL {
  XPADF_THREAD_OBJECT     m_sObject;

  XPADF_SIZE              m_nMinimumWorkers;
  XPADF_SIZE              m_nMaximumWorkers;
  XPADF_SIZE              m_nActiveWorkers;

  XPADF_BOOL              m_bRunning;

  sem_t                   m_hShutdownEvent;
  sem_t                   m_hJobEvent;

  pthread_mutex_t         m_hLock;

  PXPADF_THREAD_POOL_WORK m_pFreeWorkListHead;
  PXPADF_THREAD_POOL_WORK m_pPendingWorkListHead;
  PXPADF_THREAD_POOL_WORK m_pPendingWorkListTail;
};

#endif /* !__049D664256EEB0E21CEA74D59AB__ */
