#if !defined(__302B00736D18A25B4BBBD28F37A__)
# define __302B00736D18A25B4BBBD28F37A__

# include <xpadf/common/objects.h>

XPADF_DECLARE_STRUCT(XPADF_OBJECT);

XPADF_CALLBACK(XPADF_RESULT, PXPADFCleanupObjectCallback, (XPADF_IN PXPADF_OBJECT _pObject));

enum XPADF_OBJECT_TYPE {
  XPADF_OBJECT_TYPE_LOOPER      = 0,
  XPADF_OBJECT_TYPE_THREAD_POOL = 1,
  XPADF_OBJECT_TYPE_INVALID
};

XPADF_DECLARE_ENUM(XPADF_OBJECT_TYPE);

struct XPADF_OBJECT {
  XPADF_OBJECT_TYPE           m_eType;

  XPADF_ULONG                 m_nReferenceCount;

  PXPADFCleanupObjectCallback m_pCleanupObjectCallback;
};

XPADF_INTERNAL_FUNCTION(XPADF_RESULT, _xpadf_AllocateObject, (XPADF_OUT PXPADF_OBJECT               *_ppObject,
                                                              XPADF_IN  XPADF_OBJECT_TYPE            _eType,
                                                              XPADF_IN  PXPADFCleanupObjectCallback  _pCleanupObjectCallback,
                                                              XPADF_IN  XPADF_SIZE                   _nSize));
XPADF_INTERNAL_FUNCTION(void, _xpadf_ReferenceObject, (XPADF_INOUT PXPADF_OBJECT _pObject));
XPADF_INTERNAL_FUNCTION(void, _xpadf_DereferenceObject, (XPADF_INOUT PXPADF_OBJECT _pObject));

#endif /* !__302B00736D18A25B4BBBD28F37A__ */
