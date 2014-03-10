#if !defined(__9260DEA4A26A82B00D6D95F36D6__)
# define __9260DEA4A26A82B00D6D95F36D6__

# include <pthread.h>

# include <xpadf/sync/locks.h>

# include <xpadf/internal/common/objects.h>

XPADF_DECLARE_STRUCT(XPADF_LOCK);

struct XPADF_LOCK {
  XPADF_OBJECT    m_sObject;

  pthread_mutex_t m_hLock;
};

#endif /* !__9260DEA4A26A82B00D6D95F36D6__ */
