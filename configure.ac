
AC_PREREQ([2.69])
AC_INIT([libamathus],[0.1-0])
AC_CONFIG_SRCDIR([src/core.cpp])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
AC_PROG_INSTALL
dnl AC_PROG_RANLIB

dnl AC_CHECK_HEADER([event2/event.h], , [AC_MSG_ERROR([Required event2/event.h])] )
dnl AC_CHECK_LIB([event], [event_init], , [AC_MSG_ERROR([Required libevent])] )

dnl AC_CHECK_HEADER([openssl/bio.h], , [AC_MSG_ERROR([Required OpenSSL dev])] )
dnl AC_CHECK_LIB([ssl], [SSL_library_init], , [AC_MSG_ERROR([Required Open SSL])] )

dnl AC_CHECK_HEADER([openssl/md5.h], , [AC_MSG_ERROR([Required openssl/md5.h])] )
dnl AC_CHECK_LIB([crypto], [MD5_Final], , [AC_MSG_ERROR([Required libcrypto])] )

AC_CHECK_HEADER([libwebsockets.h], , [AC_MSG_ERROR([Required libwebsockets.h])] )
AC_CHECK_LIB([websockets], [lws_context_destroy], , [AC_MSG_ERROR([Required libwebsockets])] )


AC_LANG(C++)
AC_MSG_CHECKING([for g++11 without opt])
AC_RUN_IFELSE( [AC_LANG_PROGRAM([[#include <stdlib.h>]],
                    [[ int b[16]; for(auto &x: b)x=0; ]])],
                    [
                        AC_MSG_RESULT([yes])
                        M11=1
dnl                        AC_DEFINE_UNQUOTED([HAVE_BUILTIN_FFSL], 1, [Define to 1 if you have the `__builtin_ffsl'.])
                    ],
                    [
                        AC_MSG_RESULT([no])
dnl                     AC_DEFINE_UNQUOTED([HAVE_SYNC_LOCK_TEST_AND_SET], 0, [Define to 1 if you have the `__sync_lock_test_and_set' function.])
                    ]
)

dnl AC_LANG_CPLUSPLUS
dnl AC_CHECK_LIB([amqpcpp], [main], , [AC_MSG_ERROR([Required libamqpcpp])] )


if [[ ! -z "$M11" ]]
then
    CXXFL="-Wall -Wextra -pthread"
else
    CXXFL="-Wall -Wextra -std=c++14 -pthread"
fi

AC_SUBST([AM_CXXFLAGS], $CXXFL)

AC_CONFIG_FILES(
    Makefile
    src/Makefile
    )

AC_OUTPUT
