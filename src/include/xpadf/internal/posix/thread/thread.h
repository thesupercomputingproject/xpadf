#if !defined(__B35D6A7FB19DD47B159A0DA8541__)
# define __B35D6A7FB19DD47B159A0DA8541__

# include <xpadf/thread/thread.h>

# include <xpadf/internal/common/objects.h>

XPADF_DECLARE_STRUCT(XPADF_THREAD_OBJECT);

XPADF_CALLBACK(void *, PXPADFPThreadCallback, (void *_pContext));

struct XPADF_THREAD_OBJECT {
  XPADF_OBJECT                m_sObject;

  XPADF_PVOID                 m_pContext;
  PXPADFThreadCallback        m_pStopCallback;
};

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_AllocateThreadObject, (XPADF_OUT    PXPADF_OBJECT               *_ppObject,
                                                                    XPADF_IN     XPADF_OBJECT_TYPE            _eType,
                                                                    XPADF_IN     PXPADFCleanupObjectCallback  _pCleanupObjectCallback,
                                                                    XPADF_IN     XPADF_SIZE                   _nSize,
                                                                    XPADF_IN_OPT XPADF_PVOID                  _pContext,
                                                                    XPADF_IN_OPT PXPADFThreadCallback         _pStopCallback));

#endif /* !__B35D6A7FB19DD47B159A0DA8541__ */
