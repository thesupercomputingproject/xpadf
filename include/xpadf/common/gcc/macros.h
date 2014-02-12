#if !defined(__99FE0D4B3D7FED85966557542C5__)
# define __99FE0D4B3D7FED85966557542C5__

# define XPADF_CALLBACK(__result__, __callback_name__, __parameters__)  \
  typedef __result__ (*__callback_name__) __parameters__

# define XPADF_CDECL_CALLBACK(__result__, __callback_name__, __parameters__)  \
  typedef __result__ (*__callback_name__) __parameters__

# define XPADF_INTERNAL_FUNCTION(__result__, __function_name__, __parameters__) \
  __result__ __function_name__ __parameters__

# define XPADF_INTERNAL_CDECL_FUNCTION(__result__, __function_name__, __parameters__) \
  __result__ __function_name__ __parameters__

# define XPADF_FUNCTION(__result__, __function_name__, __parameters__)  \
  extern __result__ __function_name__ __parameters__

# define XPADF_CDECL_FUNCTION(__result__, __function_name__, __parameters__)  \
  extern __result__ __function_name__ __parameters__

#endif /* !__99FE0D4B3D7FED85966557542C5__ */
