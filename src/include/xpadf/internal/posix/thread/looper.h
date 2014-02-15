#if !defined(__E497E65AD09106167E2A2FF3DA6__)
# define __E497E65AD09106167E2A2FF3DA6__

# include <pthread.h>

# include <xpadf/thread/looper.h>

# include <xpadf/internal/posix/thread/thread.h>

XPADF_DECLARE_STRUCT(XPADF_LOOPER);

struct XPADF_LOOPER {
  XPADF_THREAD_OBJECT         m_sObject;

  pthread_t                   m_hThread;

  XPADF_BOOL                  m_bRunning;
  PXPADFThreadCallback        m_pInitializeCallback;
  PXPADFThreadCallback        m_pExecuteCallback;
  PXPADFThreadCleanupCallback m_pCleanupCallback;
};

#endif /* !__E497E65AD09106167E2A2FF3DA6__ */
