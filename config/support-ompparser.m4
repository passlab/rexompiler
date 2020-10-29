dnl Tests for OpenMPIR.h and libompparser

AC_DEFUN([ROSE_SUPPORT_LIBOMPPARSER],[

    ROSE_CONFIGURE_SECTION([OMPPARSER Library])

    dnl Parse configure command-line switches for libompparser and/or obtain the value from the cache.
    AC_ARG_WITH([ompparser],
                [AC_HELP_STRING([[[[--with-ompparser[=PREFIX]]]]], dnl yes, we really need 4 quotes (autoconf 2.6.1)!
		                [Use libompparser available from https://github.com/passlab/ompparser.
				 The PREFIX, if specified, should be the prefix used to install libompparser, 
				 such as "/usr/local".  The default is the empty prefix, in which case the headers 
				 and library must be installed in a place where they will be found. 
				 Saying "no" for the prefix is the same as saying "--without-ompparser".])],
                [ac_cv_use_ompparser=$withval],
		[ac_cv_use_ompparser=no])
    AC_CACHE_CHECK([whether to use ompparser], [ac_cv_use_ompparser], [ac_cv_use_ompparser=no])

    dnl Find the ompparser library
    ROSE_HAVE_LIBOMPPARSER=
    if test $ac_cv_use_ompparser = yes; then
        ROSE_LIBOMPPARSER_PREFIX=
	AC_CHECK_LIB(ompparser, parseOpenMP,
		     [AC_DEFINE(ROSE_HAVE_LIBOMPPARSER, [], [Defined when libompparser is available.])
		     ROSE_HAVE_LIBOMPPARSER=yes])
    elif test -n "$ac_cv_use_ompparser" -a "$ac_cv_use_ompparser" != no; then
        ROSE_LIBOMPPARSER_PREFIX="$ac_cv_use_ompparser"
	old_LDFLAGS="$LDFLAGS"
    ls $ROSE_LIBOMPPARSER_PREFIX
	LDFLAGS="$LDFLAGS -L$ROSE_LIBOMPPARSER_PREFIX/lib"
	AC_CHECK_LIB(ompparser, parseOpenMP,
	             [AC_DEFINE(ROSE_HAVE_LIBOMPPARSER, [], [Defined when libompparser is available.])
		      ROSE_HAVE_LIBOMPPARSER=yes])
        LDFLAGS="$old_LDFLAGS"
    fi

    dnl Sanity check: if the user told us to use libompparser then we must find the library
    if test "$ac_cv_use_ompparser" != no -a -z "$ROSE_HAVE_LIBOMPPARSER"; then
        AC_MSG_ERROR([did not find libompparser library but --with-ompparser was specified])
    fi

    dnl Results
    dnl   ROSE_LIBOMPPARSER_PREFIX -- name of the directory where libompparser is installed
    dnl   ROSE_HAVE_LIBOMPPARSER   -- defined if the libompparser library is available
    AC_SUBST(ROSE_LIBOMPPARSER_PREFIX)
    AM_CONDITIONAL(ROSE_HAVE_LIBOMPPARSER, [test -n "$ROSE_HAVE_LIBOMPPARSER"])
])
