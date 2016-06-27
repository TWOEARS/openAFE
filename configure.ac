# Initialize AC: ACINIT( package_name, version, bug_report_address)
AC_INIT([openAFE], [1.0], [musabini@isir.upmc.fr])
# Initialize Automake
AM_INIT_AUTOMAKE
# AM stands for Automake commands, AC stands for Autoconf commands
# We use libraries
AC_CONFIG_MACRO_DIR([m4])
# AC_PROG_RANLIB # libtoolize: `AC_PROG_RANLIB' is rendered obsolete by `LT_INIT'
# Let's check for a C++ compiler
AC_LANG(C++)
# Let's set a C++ compiler
LT_INIT
AC_PROG_CXX
# AC_SUBST([AM_CXXFLAGS], )
CXXFLAGS='-Wall -g -O -std=c++11 -lpthread -D_REENTRANT -DHAVE_INLINE'
# This suppresses the error we get for also having the cc version of helper library.
AC_SUBST([CC])
# Let's specify where the Makefiles should be produced.
# These are the same locations as your Makefile.in's, but named as Makefile only
# We need to do this because both Autoconf and Automake read this file and
# produce Makefiles from this list.
AC_CONFIG_FILES([Makefile my_inc/Makefile src/Makefile src/tools/Makefile src/Signals/Makefile src/Filters/Makefile src/Processors/Makefile examples/Makefile])
# Finally produce "configure" script
AC_OUTPUT
