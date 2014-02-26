#if !defined(__99FE0D4B3D7FED85966557542C5__)
# define __99FE0D4B3D7FED85966557542C5__

# define XPADF_IN
# define XPADF_IN_OPT

# define XPADF_OUT

# define XPADF_INOUT

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

# if defined(HAVE_ATOMIC_BUILTINS)
#  define XPADF_ATOMIC_INCREMENT(__pointer__) __sync_add_and_fetch(__pointer__, 1)
#  define XPADF_ATOMIC_DECREMENT(__pointer__) __sync_sub_and_fetch(__pointer__, 1)

#  define XPADF_ATOMIC_POP_SLIST_HEAD(__entry__, __head__, __next__)    \
  for(;;) {                                                             \
    if((__entry__ = __head__)) {                                        \
      if(__sync_bool_compare_and_swap(&__head__, __entry__, __entry__->__next__)) \
        break;                                                          \
    } else break;                                                       \
  }

#  define XPADF_ATOMIC_PUSH_SLIST_HEAD(__entry__, __head__, __next__)   \
  for(;;) {                                                             \
    __entry__->__next__ = __head__;                                     \
                                                                        \
    if(__sync_bool_compare_and_swap(&__head__, __entry__->__next__, __entry__)) \
      break;                                                            \
  }
# else
#  error ERROR: Atomic built-ins not found
# endif /* HAVE_ATOMIC_BUILTINS */

#endif /* !__99FE0D4B3D7FED85966557542C5__ */
