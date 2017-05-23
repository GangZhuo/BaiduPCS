#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([baidupcs], [0.2.6], [https://github.com/GangZhuo/BaiduPCS])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([arg.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS

dnl Check for lib
AC_ENABLE_STATIC
AC_ENABLE_SHARED
LT_INIT

AM_INIT_AUTOMAKE([subdir-objects foreign])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
dnl pthread first
ACX_PTHREAD
AX_CHECK_OPENSSL
AX_LIB_CRYPTO([auto])
LIBS="$OPENSSL_LIBS $PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

dnl Add library linking for mingw
case $host in
  *-mingw*)
    LIBS="$LIBS -lshlwapi"
    ;;
  *-cygwin*)
    LIBS="$LIBS -largp"
esac

dnl Check libcurl
AC_CHECK_LIB(
  [curl],
  [curl_version],
  [LIBS="-lcurl $LIBS"],
  [AC_MSG_FAILURE([Could not find libcurl.])]
  )

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([float.h inttypes.h limits.h locale.h malloc.h stddef.h stdint.h stdlib.h string.h termios.h unistd.h utime.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_FSEEKO
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRNLEN
AC_CHECK_FUNCS([floor gettimeofday memmove memset mkdir pow rmdir setlocale strchr strstr strtol utime])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
