AC_INIT([openAFE], [1.0], [musabini@isir.upmc.fr])
AM_INIT_AUTOMAKE([subdir-objects])
AC_LANG(C++)
AC_CONFIG_MACRO_DIR([m4])
have_pthreads=no
AC_SEARCH_LIBS([pthread_create], [pthread], [have_pthreads=yes])
AC_CHECK_HEADERS([string.h memory.h stdlib.h])

if test "x${have_pthreads}" = "xyes"; then
  AC_CHECK_HEADERS([pthread.h], [], [have_pthreads=no])
fi

LT_INIT
AC_PROG_CPP
CPPFLAGS='-Wall -Wsign-compare -std=c++11 -pthread -D_REENTRANT -DHAVE_INLINE'
AC_SUBST([CC])
AC_CONFIG_FILES([Makefile
		src/Makefile
		src/tools/Makefile
		src/Signals/Makefile
		src/Filters/Makefile
		src/Processors/Makefile
		examples/Makefile])
AC_OUTPUT
