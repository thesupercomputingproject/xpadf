#if !defined(__32E9AB2346908AABF43BF842837__)
# define __32E9AB2346908AABF43BF842837__

# if defined(__cplusplus)
#  define XPADF_DEFINE_DATA_TYPE(__base__, __name__)                    \
  typedef __base__ __name__, *P##__name__, &R##__name__;                \
  typedef const __base__ C##__name__, *PC##__name__, &RC##__name__
# else
#  define XPADF_DEFINE_DATA_TYPE(__base__, __name__)                    \
  typedef __base__ __name__, *P##__name__;                              \
  typedef const __base__ C##__name__, *PC##__name__
# endif /* __cplusplus */

# define XPADF_DECLARE_DATA_TYPE(__type__, __name__)    \
  __type__ __name__;                                    \
  XPADF_DEFINE_DATA_TYPE(__type__ __name__, __name__)

# define XPADF_DECLARE_ENUM(__name__)   XPADF_DEFINE_DATA_TYPE(enum __name__, __name__)
# define XPADF_DECLARE_STRUCT(__name__) XPADF_DECLARE_DATA_TYPE(struct __name__, __name__)

# if defined(__GNUC__)
#  include <xpadf/common/gcc/macros.h>
# else
#  error ERROR: Unrecognized compiler
# endif /* Compiler Type */

# define XPADF_STATIC_FUNCTION(__result__, __function_name__, __parameters__) \
  static XPADF_INTERNAL_FUNCTION(__result__, __function_name__, __parameters__)

# define XPADF_STATIC_CDECL_FUNCTION(__result__, __function_name__, __parameters__) \
  static XPADF_INTERNAL_CDECL_FUNCTION(__result__, __function_name__, __parameters__)

#endif /* !__32E9AB2346908AABF43BF842837__ */
