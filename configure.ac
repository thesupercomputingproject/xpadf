AC_INIT([xpadf], [0.1.0.0], [bugs@xpadf.thesupercomputingproject.org], [xpadf.tar.xz], [https://xpadf.thesupercomputingproject.org])
AC_PREREQ([2.69])
AC_COPYRIGHT([(c) 2014, The SuperComputing Project])

AM_INIT_AUTOMAKE

LT_INIT

AC_LANG([C])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CC_C89
AC_PROG_CC_C99

AC_PROG_LIBTOOL

AC_C_BACKSLASH_A
AC_C_BIGENDIAN
AC_C_CONST
AC_C_RESTRICT
AC_C_VOLATILE
AC_C_INLINE
AC_C_CHAR_UNSIGNED
AC_C_STRINGIZE
AC_C_FLEXIBLE_ARRAY_MEMBER
AC_C_VARARRAYS
AC_C_TYPEOF
AC_C_PROTOTYPES

AS_IF([test "x$ac_cv_c_compiler_gnu" = "xyes"], [
  AC_MSG_CHECKING([for GCC atomic builtins])
  AC_LANG_PUSH([C])
    AC_LINK_IFELSE([AC_LANG_SOURCE([[
      int main() {
        volatile unsigned long _val = 1;

        __sync_add_and_fetch(&_val, 1);
        __sync_sub_and_fetch(&_val, 1);

        return 0;
      }
    ]])], [
      AC_MSG_RESULT([yes])
      AC_DEFINE([HAVE_ATOMIC_BUILTINS], [1], [Define to 1 if gcc has support for atomic built-ins])
    ], [AC_MSG_RESULT([no])])
  AC_LANG_POP([C])
])

AX_PTHREAD([
  AC_CHECK_FUNCS([pthread_create], [], [AC_CHECK_LIB([pthread], [pthread_create])])
  AC_CHECK_FUNCS([pthread_equal])
  AC_CHECK_FUNCS([pthread_self])
  AC_CHECK_FUNCS([pthread_join])
  AC_CHECK_FUNCS([pthread_attr_init])
  AC_CHECK_FUNCS([pthread_attr_setdetachstate])
  AC_CHECK_FUNCS([pthread_attr_destroy])
  AC_CHECK_FUNCS([pthread_mutex_destroy])
  AC_CHECK_FUNCS([pthread_mutex_lock])
  AC_CHECK_FUNCS([pthread_mutex_unlock])
  AC_CHECK_FUNCS([pthread_mutex_init])

  AC_CHECK_TYPE([pthread_attr_t])
  AC_CHECK_TYPE([pthread_t])
  AC_CHECK_TYPE([pthread_mutex_t])

  AC_CHECK_DECLS([PTHREAD_CREATE_JOINABLE], [], [], [[#include <pthread.h>]])
])

AC_CHECK_HEADER([semaphore.h], [
  AC_CHECK_FUNCS([sem_post])
  AC_CHECK_FUNCS([sem_wait])
  AC_CHECK_FUNCS([sem_destroy])
  AC_CHECK_FUNCS([sem_init])

  AC_CHECK_TYPE([sem_t], [], [], [[#include <semaphore.h>]])
])

AC_CONFIG_HEADER([include/xpadf/common/config.h:config.h.in])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
