
#-----------------------------------------------------------------------------
AC_DEFUN([ROSE_SUPPORT_ROSE_PART_1],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_1.

# *********************************************************************
# This macro encapsulates the complexity of the tests required for ROSE
# to understnd the machine environment and the configure command line.
# It is represented a s single macro so that we can simplify the ROSE
# configure.in and permit other external project to call this macro as
# a way to set up there environment and define the many macros that an
# application using ROSE might require.
# *********************************************************************

# DQ (2/11/2010): Jeremiah reported this as bad syntax, I think he is correct.
# I'm not sure how this made it into this file.
# AMTAR ?= $(TAR)
AMTAR="$TAR"

# DQ (9/9/2009): Added test.
if test "$am__tar" = "false"; then
   AC_MSG_FAILURE([am__tar set to false])
fi

# DQ (9/9/2009): Added test.
if test "$am__untar" = "false"; then
   AC_MSG_FAILURE([am__untar set to false])
fi

# DQ (3/20/2009): Trying to get information about what system we are on so that I
# can detect Cygwin and OSX (and other operating systems in the future).
AC_CANONICAL_BUILD
# AC_CANONICAL_HOST
# AC_CANONICAL_TARGET

AC_MSG_CHECKING([machine hardware cpu])
AC_MSG_RESULT([$build_cpu])

AC_MSG_CHECKING([operating system vendor])
AC_MSG_RESULT([$build_vendor])

AC_MSG_CHECKING([operating system])
AC_MSG_RESULT([$build_os])

DETERMINE_OS

# DQ (3/20/2009): The default is to assume Linux, so skip supporting this test.
# AM_CONDITIONAL(ROSE_BUILD_OS_IS_LINUX,  [test "x$build_os" = xlinux-gnu])
AM_CONDITIONAL(ROSE_BUILD_OS_IS_OSX,    [test "x$build_vendor" = xapple])
AM_CONDITIONAL(ROSE_BUILD_OS_IS_CYGWIN, [test "x$build_os" = xcygwin])

# DQ (9/10/2009): A more agressive attempt to identify the OS vendor
# This sets up automake conditional variables for each OS vendor name.
DETERMINE_OS_VENDOR

configure_date=`date '+%A %B %e %H:%M:%S %Y'`
AC_SUBST(configure_date)
# echo "In ROSE/con figure: configure_date = $configure_date"

# DQ (1/27/2008): Added based on suggestion by Andreas.  This allows
# the binary analysis to have more specific information. However, it
# appears that it requires version 2.61 of autoconf and we are using 2.59.
# echo "$host_cpu"
# echo "host_cpu = $host_cpu"
# echo "host_vendor = $host_vendor"
# echo "ac_cv_host = $ac_cv_host"
# echo "host = $host"
# This does not currently work - I don't know why!
# AC_DEFINE([ROSE_HOST_CPU],$host_cpu,[Machine CPU Name where ROSE was configured.])

# DQ (9/7/2006): Allow the default prefix to be the current build tree
# This does not appear to work properly
# AC_PREFIX_DEFAULT(`pwd`)

# echo "In configure: prefix = $prefix"
# echo "In configure: pwd = $PWD"

if test "$prefix" = NONE; then
   AC_MSG_NOTICE([setting prefix to default: "$PWD"])
   prefix="$PWD"
fi

# Call supporting macro for the Java path required by the Open Fortran Parser (for Fortran 2003 support)
# Use our classpath in case the user's is messed up
AS_SET_CATFILE([ABSOLUTE_SRCDIR], [`pwd`], [${srcdir}])

ROSE_CONFIGURE_SECTION([Checking GNU Fortran])
# DQ (10/18/2010): Check for gfortran (required for syntax checking and semantic analysis of input Fortran codes)
AC_REQUIRE([AC_PROG_F77])
AC_REQUIRE([AC_PROG_FC])
AX_WITH_PROG(GFORTRAN_PATH, [gfortran], [])
AC_SUBST(GFORTRAN_PATH)

# DQ (11/17/2016): We need to make sure that --without-gfortran does not set USE_GFORTRAN_IN_ROSE to true.
# if test "x$GFORTRAN_PATH" != "x"; then
if test "x$GFORTRAN_PATH" != "x" -a "$GFORTRAN_PATH" != "no"; then
   AC_DEFINE([USE_GFORTRAN_IN_ROSE], [1], [Mark that GFORTRAN is available])
else
   AC_DEFINE([USE_GFORTRAN_IN_ROSE], [0], [Mark that GFORTRAN is not available])
fi

AC_MSG_NOTICE([GFORTRAN_PATH = "$GFORTRAN_PATH"])

#########################################################################################
##

  ROSE_SUPPORT_LANGUAGE_CONFIG_OPTIONS

  ROSE_CONFIGURE_SECTION([])
  AC_CHECK_LIB([curl], [Curl_connect], [HAVE_CURL=yes], [HAVE_CURL=no])
  AM_CONDITIONAL([HAS_LIBRARY_CURL], [test "x$HAVE_CURL" = "xyes"])

# Rasmussen (01/13/2021): Moved checking for Java until after language configuration
# options are set.  Otherwise configure fails if jdk libraries aren't found even if not used.
  ROSE_SUPPORT_JAVA # This macro uses JAVA_HOME

# ****************************************************
# ROSE/tests directory compilation & testing
# ****************************************************
AC_MSG_CHECKING([if we should build & test the ROSE/tests directory])
AC_ARG_ENABLE([tests-directory],AS_HELP_STRING([--disable-tests-directory],[Disable compilation and testing of the ROSE/tests directory]),[],[enableval=yes])
support_tests_directory=yes
if test "x$enableval" = "xyes"; then
   support_tests_directory=yes
   AC_MSG_RESULT(enabled)
   AC_DEFINE([ROSE_BUILD_TESTS_DIRECTORY_SUPPORT], [], [Build ROSE tests directory])
else
   support_tests_directory=no
   AC_MSG_RESULT(disabled)
fi
AM_CONDITIONAL(ROSE_BUILD_TESTS_DIRECTORY_SUPPORT, [test "x$support_tests_directory" = xyes])
# *******************************************************
# ROSE/tutorial directory compilation & testing
# *******************************************************
AC_MSG_CHECKING([if we should build & test the ROSE/tutorial directory])
AC_ARG_ENABLE([tutorial-directory],AS_HELP_STRING([--disable-tutorial-directory],[Disable compilation and testing of the ROSE/tutorial directory]),[],[enableval=yes])
support_tutorial_directory=yes
if test "x$enableval" = "xyes"; then
   support_tutorial_directory=yes
   AC_MSG_RESULT(enabled)
   AC_DEFINE([ROSE_BUILD_TUTORIAL_DIRECTORY_SUPPORT], [], [Build ROSE tutorial directory])
else
   support_tutorial_directory=no
   AC_MSG_RESULT(disabled)
fi
AM_CONDITIONAL(ROSE_BUILD_TUTORIAL_DIRECTORY_SUPPORT, [test "x$support_tutorial_directory" = xyes])

# ************************************************************
# Option to turn on a special mode of memory pools: no reuse of deleted memory.
# This is useful to track AST nodes during transformation, otherwise the same memory may be reused
# by multiple different AST nodes.
# Liao 8/13/2014
# ************************************************************

AC_ARG_ENABLE(memoryPoolNoReuse, AS_HELP_STRING([--enable-memory-pool-no-reuse], [Enable special memory pool model: no reuse of deleted memory (default is to reuse memory)]))
AM_CONDITIONAL(ROSE_USE_MEMORY_POOL_NO_REUSE, [test "x$enable_memory_pool_no_reuse" = xyes])
if test "x$enable_memory_pool_no_reuse" = "xyes"; then
  AC_MSG_WARN([turn on a special mode in memory pools: no reuse of deleted memory blocks])
  AC_DEFINE([ROSE_USE_MEMORY_POOL_NO_REUSE], [], [Whether to use a special no-reuse mode of memory pools])
fi


# ************************************************************
# Option to control the size of the generated files by ROSETTA
# ************************************************************

# DQ (12/29/2009): This is part of optional support to reduce the sizes of some of the ROSETTA generated files.
AC_ARG_ENABLE(smallerGeneratedFiles, AS_HELP_STRING([--enable-smaller-generated-files], [ROSETTA generates smaller files (but more of them so it takes longer to compile)]))
AM_CONDITIONAL(ROSE_USE_SMALLER_GENERATED_FILES, [test "x$enable_smaller_generated_files" = xyes])
if test "x$enable_smaller_generated_files" = "xyes"; then
  AC_MSG_WARN([using optional ROSETTA mechanism to generate numerous but smaller files for the ROSE IR])
  AC_DEFINE([ROSE_USE_SMALLER_GENERATED_FILES], [], [Whether to use smaller (but more numerous) generated files for the ROSE IR])
fi

# This is the support for using EDG as the frontend in ROSE.
ROSE_SUPPORT_EDG

# This is the support for using Clang as a frontend in ROSE not the support for Clang as a compiler to compile ROSE source code.
ROSE_SUPPORT_CLANG

# DQ (1/4/2009) Added support for optional GNU language extensions in new EDG/ROSE interface.
# This value will be substituted into EDG/4.0/src/rose_lang_feat.h in the future (not used at present!)
AC_ARG_ENABLE(gnu-extensions, AS_HELP_STRING([--enable-gnu-extensions], [Enable internal support in ROSE for GNU language extensions]))
if test "x$enable_gnu_extensions" = "xyes"; then
  ROSE_SUPPORT_GNU_EXTENSIONS="TRUE"
else
  ROSE_SUPPORT_GNU_EXTENSIONS="FALSE"
fi
AC_SUBST(ROSE_SUPPORT_GNU_EXTENSIONS)

# DQ (1/4/2009) Added support for optional Microsoft language extensions in new EDG/ROSE interface.
# This value will be substituted into EDG/4.0/src/rose_lang_feat.h in the future (not used at present!)
AC_ARG_ENABLE(microsoft-extensions, AS_HELP_STRING([--enable-microsoft-extensions], [Enable internal support in ROSE for Microsoft language extensions]))
if test "x$enable_microsoft_extensions" = "xyes"; then
  ROSE_SUPPORT_MICROSOFT_EXTENSIONS="TRUE"
  AC_DEFINE([ROSE_USE_MICROSOFT_EXTENSIONS], [], [Controls use of Microsoft MSVC features])
else
  ROSE_SUPPORT_MICROSOFT_EXTENSIONS="FALSE"
fi
AC_SUBST(ROSE_SUPPORT_MICROSOFT_EXTENSIONS)
AM_CONDITIONAL(ROSE_USE_MICROSOFT_EXTENSIONS, [test "x$enable_microsoft_extensions" = xyes])

# TV (12/31/2018): Defining macro to detect the support of __float128 in EDG
#   Only valid if compiling ROSE using GNU compiler (depends on -lquadmath)
AC_LANG(C++)
AX_COMPILER_VENDOR

ac_save_LIBS="$LIBS"
LIBS="$ac_save_LIBS -lquadmath"
AC_LINK_IFELSE([
            AC_LANG_PROGRAM([[#include <quadmath.h>]])],
            [rose_use_edg_quad_float=yes],
            [rose_use_edg_quad_float=no])
LIBS="$ac_save_LIBS"

if test "$ROSE_SUPPORT_MICROSOFT_EXTENSIONS" == "TRUE"; then
  rose_use_edg_quad_float=no
fi

if test "x$rose_use_edg_quad_float" == "xyes"; then
  AC_DEFINE([ROSE_USE_EDG_QUAD_FLOAT], [], [Enables support for __float80 and __float128 in EDG.])
fi
AC_SUBST(ROSE_USE_EDG_QUAD_FLOAT)
AM_CONDITIONAL(ROSE_USE_EDG_QUAD_FLOAT, [ test $rose_use_edg_quad_float == yes ])
unset ax_cv_cxx_compiler_vendor

# DQ (9/16/2012): Added support for debugging output of new EDG/ROSE connection.  More specifically
# if this is not enabled then it skips the use of output spew in the new EDG/ROSE connection code.
AC_ARG_ENABLE(debug_output_for_new_edg_interface,
    AS_HELP_STRING([--enable-debug_output_for_new_edg_interface], [Enable debugging output (spew) of new EDG/ROSE connection]))
AM_CONDITIONAL(ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION, [test "x$enable_debug_output_for_new_edg_interface" = xyes])
if test "x$enable_debug_output_for_new_edg_interface" = "xyes"; then
  AC_MSG_WARN([using this mode causes large volumes of output spew (internal debugging only)!])
  AC_DEFINE([ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION], [], [Controls large volumes of output spew useful for debugging new EDG/ROSE connection code])
fi

# DQ (6/7/2013): Added support for new Fortran front-end development.
AC_ARG_ENABLE(experimental_fortran_frontend,
    AS_HELP_STRING([--enable-experimental_fortran_frontend], [Enable experimental fortran frontend development]))
AM_CONDITIONAL(ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION, [test "x$enable_experimental_fortran_frontend" = xyes])
if test "x$enable_experimental_fortran_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode enables experimental fortran front-end (internal development only)!])
  AC_DEFINE([ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION], [], [Enables development of experimental fortran frontend])
fi

# DQ (6/7/2013): Added support for debugging new Fortran front-end development.
AC_ARG_ENABLE(debug_output_for_experimental_fortran_frontend,
    AS_HELP_STRING([--enable-debug_output_for_experimental_fortran_frontend], [Enable debugging output (spew) of new OFP/ROSE connection]))
AM_CONDITIONAL(ROSE_DEBUG_EXPERIMENTAL_OFP_ROSE_CONNECTION, [test "x$enable_debug_output_for_experimental_fortran_frontend" = xyes])
if test "x$enable_debug_output_for_experimental_fortran_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode causes large volumes of output spew (internal debugging only)!])
  AC_DEFINE([ROSE_DEBUG_EXPERIMENTAL_OFP_ROSE_CONNECTION], [], [Controls large volumes of output spew useful for debugging new OFP/ROSE connection code])
fi

# DQ (8/18/2009): Removed this conditional macro.
# DQ (4/23/2009): Added support for commandline specification of using new graph IR nodes.
# AC_ARG_ENABLE(newGraphNodes, AS_HELP_STRING([--enable-newGraphNodes], [Enable new (experimental) graph IR nodes]))
#AM_CONDITIONAL(ROSE_USE_NEW_GRAPH_NODES, [test "x$enable_newGraphNodes" = xyes])
#if test "x$enable_newGraphNodes" = "xyes"; then
#  AC_MSG_WARN([Using the new graph IR nodes in ROSE (experimental)!])
#  AC_DEFINE([ROSE_USE_NEW_GRAPH_NODES], [], [Whether to use the new graph IR nodes])
#fi

# DQ (5/2/2009): Added support for backward compatability of new IR nodes with older API.
AC_ARG_ENABLE(use_new_graph_node_backward_compatability,
    AS_HELP_STRING([--enable-use_new_graph_node_backward_compatability], [Enable new (experimental) graph IR nodes backward compatability API]))
AM_CONDITIONAL(ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY, [test "x$enable_use_new_graph_node_backward_compatability" = xyes])
if test "x$enable_use_new_graph_node_backward_compatability" = "xyes"; then
  AC_MSG_WARN([using the new graph IR nodes in ROSE (experimental)!])
  AC_DEFINE([ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY], [], [Whether to use the new graph IR nodes compatability option with older API])
fi

# Set the value of srcdir so that it will be an absolute path instead of a relative path
# srcdir=`dirname "$0"`
# echo "In ROSE/con figure: srcdir = $srcdir"
# echo "In ROSE/con figure: $0"
# Record the location of the build tree (so it can be substituted into ROSE/docs/Rose/rose.cfg)
# topSourceDirectory=`dirname "$0"`
# echo "In ROSE/con figure: topSourceDirectory = $topSourceDirectory"
# AC_SUBST(topSourceDirectory)

# echo "Before test for CANONICAL HOST: CC (CC = $CC)"

AC_CANONICAL_HOST

# *****************************************************************

# DQ (12/3/2016): Added support for specification of specific warnings a for those specific warnings to be treated as errors.
# ROSE_SUPPORT_FATAL_WARNINGS

# *****************************************************************

# DQ (3/21/2017): Moved this to here (earlier than where is it used below) so that
# the warnings options can use the compiler vendor instead of the compiler name.
AC_LANG(C++)

# Get frontend compiler vendor
AX_COMPILER_VENDOR
FRONTEND_CXX_COMPILER_VENDOR="$ax_cv_cxx_compiler_vendor"

# DQ (10/27/2020): Fixed to avoid output spew.
AC_MSG_NOTICE([_AC_LANG_ABBREV              = "$_AC_LANG_ABBREV"])
AC_MSG_NOTICE([ax_cv_c_compiler_vendor      = "$ax_cv_c_compiler_vendor"])
AC_MSG_NOTICE([ax_cv_cxx_compiler_vendor    = $ax_cv_cxx_compiler_vendor"])

AC_MSG_NOTICE([FRONTEND_CXX_COMPILER_VENDOR = "$FRONTEND_CXX_COMPILER_VENDOR"])

unset ax_cv_cxx_compiler_vendor

# DQ (9/20/20): Moving the setup of compiler flags to after the macros that define the compiler versions are computed.
# Setup default options for C and C++ compilers compiling ROSE source code.
# ROSE_FLAG_C_OPTIONS
# ROSE_FLAG_CXX_OPTIONS

# echo "Exiting after computing the frontend compiler vendor"
# exit 1

# *****************************************************************

AC_MSG_NOTICE([CFLAGS   = "$CFLAGS"])
AC_MSG_NOTICE([CXXFLAGS = "$CXXFLAGS"])
AC_MSG_NOTICE([CPPFLAGS = "$CPPFLAGS"])

# *****************************************************************
#    Option to define a uniform debug level for ROSE development
# *****************************************************************

# DQ (10/17/2010): This defines an advanced level of uniform support for debugging and compiler warnings in ROSE.
AC_MSG_CHECKING([for enabled advanced warning support])
# Default is that advanced warnings is off, but this can be changed later so that advanced warnings would have to be explicitly turned off.
AC_ARG_ENABLE(advanced_warnings, AS_HELP_STRING([--enable-advanced-warnings], [Support for an advanced uniform warning level for ROSE development]),[enableval=yes],[enableval=no])
AM_CONDITIONAL(ROSE_USE_UNIFORM_ADVANCED_WARNINGS_SUPPORT, [test "x$enable_advanced_warnings" = xyes])
if test "x$enable_advanced_warnings" = "xyes"; then
  AC_MSG_WARN([using an advanced uniform warning level for ROSE development])
  AC_DEFINE([ROSE_USE_UNIFORM_ADVANCED_WARNINGS_SUPPORT], [], [Support for an advanced uniform warning level for ROSE development])

# Suggested C++ specific flags (used to be run before Hudson, but fail currently).
  CXX_ADVANCED_WARNINGS+=" -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_DEBUG"

# Additional flag (suggested by George).
  CXX_ADVANCED_WARNINGS+=" -D_GLIBCXX_DEBUG_PEDANTIC"

# Incrementally add the advanced options
  if test "$CXX_ADVANCED_WARNINGS"; then CXXFLAGS="$CXXFLAGS $CXX_ADVANCED_WARNINGS"; fi
fi
# ROSE_USE_UNIFORM_DEBUG_SUPPORT=7
AC_SUBST(ROSE_USE_UNIFORM_ADVANCED_WARNINGS_SUPPORT)

AC_MSG_NOTICE([after processing --enable-advanced-warnings: CXX_ADVANCED_WARNINGS = "${CXX_ADVANCED_WARNINGS}"])
AC_MSG_NOTICE([after processing --enable-advanced-warnings: CXX_WARNINGS = "${CXX_WARNINGS}"])
AC_MSG_NOTICE([after processing --enable-advanced-warnings: C_WARNINGS   = "${C_WARNINGS}"])

AC_MSG_NOTICE([CFLAGS   = "$CFLAGS"])
AC_MSG_NOTICE([CXXFLAGS = "$CXXFLAGS"])
AC_MSG_NOTICE([CPPFLAGS = "$CPPFLAGS"])

# echo "Exiting in support after enabled advanced warnings"
# exit 1

# *****************************************************************

# DQ: added here to see if it would be defined for the template tests and avoid placing
# a $(CXX_TEMPLATE_REPOSITORY_PATH) directory in the top level build directory (a minor error)
CXX_TEMPLATE_REPOSITORY_PATH='$(top_builddir)/src'

# *****************************************************************

# *****************************************************************

# ********************************************************************************
#    Option support for the Address Sanitizer and other related Sanitizer tools.
# ********************************************************************************

ROSE_SUPPORT_SANITIZER

# *****************************************************************


# ********************************************************************************
#    Option support for the Linux Coverage Test tools.
# ********************************************************************************

ROSE_SUPPORT_LCOV

# *****************************************************************
# ROSE_HOME should be relative to top_srcdir or top_builddir.
ROSE_HOME=.
# ROSE_HOME=`pwd`/$top_srcdir
AC_SUBST(ROSE_HOME)
# echo "In ROSE/configure: ROSE_HOME = $ROSE_HOME"

AC_LANG(C++)

# Rasmussen (12/16/2017): Added test for Bison version (Mac OSX Bison version may be too old)
ROSE_SUPPORT_BISON

# DQ (11/5/2009): Added test for GraphViz's ``dot'' program
ROSE_SUPPORT_GRAPHVIZ

# DQ (9/15/2009): I have moved this to before the backend compiler selection so that
# we can make the backend selection a bit more compiler dependent. Actually we likely
# don't need this!
# DQ (9/17/2006): These should be the same for both C and C++ (else we will need separate macros)
# Setup the -D<xxx> defines required to allow EDG to take the same path through the compiler
# specific and system specific header files as for the backend compiler.  These depend
# upon the selection of the back-end compiler.
# GET_COMPILER_SPECIFIC_DEFINES

# Test this macro here at the start to avoid long processing times (before it fails)
CHOOSE_BACKEND_COMPILER

# *****************************************************************

# echo "DQ (7/26/2020): Exiting after CHOOSE_BACKEND_COMPILER"
# exit 1

# *****************************************************************

# Calling available macro from Autoconf (test by optionally pushing C language onto the internal autoconf language stack).
# This function must be called from this support-rose file (error in ./build if called from the GET COMPILER SPECIFIC DEFINES macro.
# AC_LANG_PUSH(C)

# Get frontend compiler vendor
AX_COMPILER_VENDOR
FRONTEND_CXX_COMPILER_VENDOR="$ax_cv_cxx_compiler_vendor"
unset ax_cv_cxx_compiler_vendor

# Get backend compiler vendor
  saved_compiler_name=$CXX
  CXX=$BACKEND_CXX_COMPILER
  AC_MSG_NOTICE([after resetting CXX to be the backend compiler: CXX = "$CXX"])

  AX_COMPILER_VENDOR
# returns string ax_cv_cxx_compiler_vendor if this is the C++ compiler else returns
# the vendor for the C compiler in ax_cv_c_compiler_vendor for the C compiler.
# CcompilerVendorName= $ax_cv_c_compiler_vendor
# CxxcompilerVendorName= $ax_cv_cxx_compiler_vendor
# echo "Output the names of the vendor for the C or C++ backend compilers."
# echo "Using back-end C   compiler = \"$BACKEND_CXX_COMPILER\" compiler vendor name = $ax_cv_c_compiler_vendor   for processing of unparsed source files from ROSE preprocessors."
  AC_MSG_NOTICE([using back-end C++ compiler = "$BACKEND_CXX_COMPILER" compiler vendor name = $ax_cv_cxx_compiler_vendor for processing of unparsed source files from ROSE preprocessors])
  BACKEND_CXX_COMPILER_VENDOR="$ax_cv_cxx_compiler_vendor"

  CXX=$saved_compiler_name
  AC_MSG_NOTICE([after resetting CXX to be the saved name of the original compiler: CXX = "$CXX"])

AC_MSG_NOTICE([FRONTEND_CXX_COMPILER_VENDOR = "$FRONTEND_CXX_COMPILER_VENDOR"])

# echo "Exiting after computing the backend compiler vendor"
# exit 1

# *****************************************************************

# DQ (2/27/2016): Added version 4.9.x to supported compilers.
AC_MSG_CHECKING([whether your compiler is a GNU compiler and the version that is supported by ROSE (4.0.x - 6.3.x)])
AC_ARG_ENABLE([gcc-version-check],AS_HELP_STRING([--disable-gcc-version-check],[Disable GCC version 4.0.x - 6.3.x verification check]),,[enableval=yes])
if test "x$FRONTEND_CXX_COMPILER_VENDOR" = "xgnu" ; then
if test "x$enableval" = "xyes" ; then
      AC_LANG_PUSH([C])
      # http://www.gnu.org/s/hello/manual/autoconf/Running-the-Compiler.html
      AC_COMPILE_IFELSE([
        AC_LANG_SOURCE([[
          #if (__GNUC__ >= 4 && __GNUC_MINOR__ <= 9)
            int rose_supported_gcc;
          #else
            not gcc, or gcc version is not supported by rose
          #endif
        ]])
       ],
       [AC_MSG_RESULT([done])],
       gcc_version=`gcc -dumpversion`
       [AC_MSG_FAILURE([your GCC $gcc_version version is currently NOT supported by ROSE; GCC 4.0.x to 4.8.x is supported now])])
      AC_LANG_POP([C])
else
    AC_MSG_RESULT([skipping])
fi
else
    AC_MSG_RESULT([not a GNU compiler])
fi

# *****************************************************************

# DQ (2/7/17): This is a problem reported by Robb (sometimes gcc is not installed).
# This is used in EDG (host_envir.h)  Test by building a bad version of gcc
# use shell script called gcc with "exit 1" inside.
if test "x$FRONTEND_CXX_COMPILER_VENDOR" = "xgnu" ; then
   GCC_VERSION=`gcc -dumpversion | cut -d\. -f1`
   GCC_MINOR_VERSION=`gcc -dumpversion | cut -d\. -f2`

   AC_MSG_NOTICE([initial compiler version test: GCC_VERSION = "$GCC_VERSION"])
   AC_MSG_NOTICE([initial compiler version test: GCC_MINOR_VERSION = "$GCC_MINOR_VERSION"])

   AC_SUBST(GCC_VERSION)
   AC_SUBST(GCC_MINOR_VERSION)
else
 # DQ (2/8/2017): Default configuration of EDG will behave like GNU 4.8.x (unclear if this is idea).
   GCC_VERSION=4
   GCC_MINOR_VERSION=8
fi

# echo "Exiting after test for GNU compiler and setting the version info for EDG (GCC_VERSION and GCC_MINOR_VERSION)."
# exit 1

# DQ (7/27/2020): debugging info
# echo "After computing GNU version: GCC_VERSION       = $GCC_VERSION"
# echo "After computing GNU version: GCC_MINOR_VERSION = $GCC_MINOR_VERSION"

# *****************************************************************

# DQ (2/7/2017): These macros test for C++11 and C++14 features and
# the default behavior of the CXX compiler.  Unfortunately the also
# modify the CXX value so we have to save it and reset it after the
# macros are called.  We modified the macros as well to save the
# default behavior of the CXX compiler so that we can detect C++11
# mode within the frontend compiler used to compile ROSE.  Thi is used
# mostly so far to just disable some test that are causing GNU g++
# version 4.8.x internal errors (because the C++11 support is new).

AC_MSG_NOTICE([before checking C++11 support: CXX = $CXX CXXCPP = "$CXXCPP"])

AC_MSG_NOTICE([calling AX CXX COMPILE STDCXX 11 macro])
save_CXX="$CXX"
AX_CXX_COMPILE_STDCXX_11(, optional)

AC_MSG_NOTICE([after checking C++11 support: CXX = "$CXX", CXXCPP = "$CXXCPP"])

AC_MSG_NOTICE([rose_frontend_compiler_default_is_cxx11_success = "$rose_frontend_compiler_default_is_cxx11_success"])
AC_MSG_NOTICE([gcc_version_4_8                                 = "$gcc_version_4_8"])

AM_CONDITIONAL(ROSE_USING_GCC_VERSION_4_8_CXX11, [test "x$gcc_version_4_8" = "xyes" && test "x$rose_frontend_compiler_default_is_cxx11_success" = "xyes"])

AC_MSG_NOTICE([calling AX CXX COMPILE STDCXX 14 macro])
AX_CXX_COMPILE_STDCXX_14(, optional)

AC_MSG_NOTICE([after checking C++14 support: CXX = "$CXX", CXXCPP = "$CXXCPP"])
CXX="$save_CXX"

AC_MSG_NOTICE([after restoring the saved value of CXX: CXX = "$CXX", CXXCPP = "$CXXCPP"])

# echo "Exiting in support-rose after computing the C++ mode (c++11, and c++14 modes)"
# exit 1

# *****************************************************************

# DQ (12/7/2016): Added support for specification of specific warnings a for those specific warnings to be treated as errors.
ROSE_SUPPORT_FATAL_WARNINGS

# *****************************************************************

# echo "Exiting in support-rose after computing the compiler vendor name for the C and C++ compilers."
# exit 1

# End macro ROSE_SUPPORT_ROSE_PART_1.
]
)


AC_DEFUN([ROSE_SUPPORT_ROSE_BUILD_INCLUDE_FILES],
[
# Begin macro ROSE_SUPPORT_ROSE_BUILD_INCLUDE_FILES.

AC_MSG_NOTICE([in ROSE SUPPORT ROSE BUILD INCLUDE FILES: Using back-end C++ compiler = "$BACKEND_CXX_COMPILER" compiler vendor name = "$ax_cv_cxx_compiler_vendor" for processing of unparsed source files from ROSE preprocessors])

# DQ (7/26/2020): Spelling it correctly so that we can force the directory of header files to be rebuilt.
# Note that this directory name is not spelled correctly, is this a typo?
# JJW (12/10/2008): We don't preprocess the header files for the new interface
# rm -rf ./include-stagin
# echo "Changes spelling of include-stagin to force the directory of header files to be rebuilt."
rm -rf ./include-stagin

# DQ (7/27/2020): debugging info
# echo "Before processing include files: GCC_VERSION       = $GCC_VERSION"
# echo "Before processing include files: GCC_MINOR_VERSION = $GCC_MINOR_VERSION"

if test x$enable_clang_frontend = xyes; then
  INSTALL_CLANG_SPECIFIC_HEADERS
else

# DQ (7/26/2020): Process this macro only to better support testing.
# GENERATE_BACKEND_CXX_COMPILER_SPECIFIC_HEADERS

  # DQ (11/1/2011): I think that we need these for more complex header file
  # requirements than we have seen in testing C code to date.  Previously
  # in testing C codes with the EDG 4.x we didn't need as many header files.
    GENERATE_BACKEND_C_COMPILER_SPECIFIC_HEADERS
    GENERATE_BACKEND_CXX_COMPILER_SPECIFIC_HEADERS
fi

# End macro ROSE_SUPPORT_ROSE_BUILD_INCLUDE_FILES.

# *****************************************************************
# echo "DQ (7/26/2020): Exiting after ROSE SUPPORT ROSE BUILD INCLUDE FILES (Skipped C header files!)"
# exit 1
# *****************************************************************
]
)

# *****************************************************************
# The exit here does not appear to force an exit.
# echo "DQ (7/26/2020): Exiting after ROSE_SUPPORT_ROSE_BUILD_INCLUDE_FILES"
# exit 1
# *****************************************************************

#-----------------------------------------------------------------------------

AC_DEFUN([ROSE_SUPPORT_ROSE_PART_2],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_2.

# DQ (9/26/2015): Since the config/ltdl.m4 file in regenerated, we can't edit it easily.
# So make this a requirement so that it will not be expanded there.
m4_require([_LT_SYS_DYNAMIC_LINKER])

# AC_REQUIRE([AC_PROG_CXX])
AC_PROG_CXX

AC_MSG_NOTICE([in configure.in ... CXX = "$CXX"])

# DQ (9/17/2006): These should be the same for both C and C++ (else we will need separate macros)
# Setup the -D<xxx> defines required to allow EDG to take the same path through the compiler
# specific and system specific header files as for the backend compiler.  These depend
# upon the selection of the back-end compiler.
GET_COMPILER_SPECIFIC_DEFINES

# DQ (9/20/20): Moving the setup of compiler flags to after the macros that define the compiler versions are computed.
# Setup default options for C and C++ compilers compiling ROSE source code.
ROSE_FLAG_C_OPTIONS
ROSE_FLAG_CXX_OPTIONS
ROSE_FLAG_OPTIONS

# This must go after the setup of the headers options
# Setup the CXX_INCLUDE_STRING to be used by EDG to find the correct headers
# SETUP_BACKEND_COMPILER_SPECIFIC_REFERENCES
# JJW (12/10/2008): We don't preprocess the header files for the new interface,
# but we still need to use the original C++ header directories
ROSE_CONFIGURE_SECTION([Checking backend C/C++ compiler specific references])
SETUP_BACKEND_C_COMPILER_SPECIFIC_REFERENCES
SETUP_BACKEND_CXX_COMPILER_SPECIFIC_REFERENCES

# echo "In configure.in ... CXX = $CXX : exiting after call to setup backend C and C++ compilers specific references."
# exit 1

# DQ (1/15/2007): Check if longer internal make check rule is to be used (default is short tests)
ROSE_SUPPORT_LONG_MAKE_CHECK_RULE

# Make the use of longer test optional where it is used in some ROSE/tests directories
AM_CONDITIONAL(ROSE_USE_LONG_MAKE_CHECK_RULE,test "$with_ROSE_LONG_MAKE_CHECK_RULE" = yes)

# Check for availability of wget (used for downloading the EDG binaries used in ROSE).
AC_CHECK_TOOL(ROSE_WGET_PATH, [wget], [no])
AM_CONDITIONAL(ROSE_USE_WGET, [test "$ROSE_WGET_PATH" != "no"])
if test "$ROSE_WGET_PATH" = "no"; then
   AC_MSG_FAILURE([wget was not found; ROSE requires wget to download EDG binaries automatically])
else
   # Not clear if we really should have ROSE configure automatically do something like this.
   AC_MSG_NOTICE([ROSE might use wget to automatically download EDG binaries as required during the build])
fi
# Check for availability of ps2pdf, part of ghostscript (used for generating pdf files).
AC_CHECK_TOOL(ROSE_PS2PDF_PATH, [ps2pdf], [no])
AM_CONDITIONAL(ROSE_USE_PS2PDF, [test "$ROSE_PS2PDF_PATH" != "no"])
if test "$ROSE_PS2PDF_PATH" = "no"; then
   AC_MSG_FAILURE([ps2pdf was not found; ROSE requires ps2pdf (part of ghostscript) to generate pdf files])
fi

AC_C_BIGENDIAN
AC_CHECK_HEADERS([byteswap.h machine/endian.h])

ROSE_SUPPORT_VALGRIND

AC_ARG_WITH(wave-default, [  --with-wave-default     Use Wave as the default preprocessor],
            [AC_DEFINE([ROSE_WAVE_DEFAULT], true, [Use Wave as default in ROSE])],
            [AC_DEFINE([ROSE_WAVE_DEFAULT], false, [Simple preprocessor as default in ROSE])]
            )

# Figure out what version of lex we have available
# flex works better than lex (this gives a preference to flex (flex is gnu))
AM_PROG_LEX
AC_SUBST(LEX)
AC_PROG_YACC
AC_SUBST(YACC)

# echo "After test for LEX: CC (CC = $CC)"

# DQ (4/1/2001) Need to call this macro to avoid having "MAKE" set to "make" in the
# top level Makefile (this is important to getting gmake to be used in the "make distcheck"
# makefile rule.  (This does not seem to work, since calling "make distcheck" still fails and
# only "gmake distcheck" seems to work.  I don't know why!
AC_PROG_MAKE_SET

# DQ (9/21/2009): Debugging for RH release 5
AC_MSG_NOTICE([testing the value of CC: (CC = "$CC")])
AC_MSG_NOTICE([testing the value of CPPFLAGS: (CPPFLAGS = "$CPPFLAGS")])

ROSE_SUPPORT_VECTORIZATION

ROSE_SUPPORT_LIBHARU
#ASR
ROSE_SUPPORT_LLVM

AM_CONDITIONAL(ROSE_USE_LLVM,test ! "$with_llvm" = no)

# Control use of debugging support to convert most unions in EDG to structs.
ROSE_SUPPORT_EDG_DEBUGGING

# Call supporting macro for Omni OpenMP
#
ROSE_SUPPORT_OMNI_OPENMP

# call supporting macro for GCC 4.4.x gomp OpenMP runtime library
# AM_CONDITIONAL is already included into the macro
ROSE_WITH_GOMP_OPENMP_LIBRARY

# Call supporting macro for GCC OpenMP
ROSE_SUPPORT_GCC_OMP

# Configuration commandline support for OpenMP in ROSE
AM_CONDITIONAL(ROSE_USE_GCC_OMP,test ! "$with_parallel_ast_traversal_omp" = no)

# JJW and TP (3-17-2008) -- added MPI support
AC_ARG_WITH(parallel_ast_traversal_mpi,
[  --with-parallel_ast_traversal_mpi     Enable AST traversal in parallel using MPI.],
[ AC_MSG_NOTICE([setting up optional MPI-based tools])
])
AM_CONDITIONAL(ROSE_MPI,test "$with_parallel_ast_traversal_mpi" = yes)
AC_CHECK_TOOLS(MPICXX, [mpiCC mpic++ mpicxx])


# TPS (2-11-2009) -- added PCH Support
AC_ARG_WITH(pch,
[  --with-pch                    Configure option to have pre-compiled header support enabled.],
[ AC_MSG_NOTICE([enabling precompiled header])
])
AM_CONDITIONAL(ROSE_PCH,test "$with_pch" = yes)
if test "x$with_pch" = xyes; then
  AC_MSG_NOTICE(["PCH enabled: CPPFLAGS = "$CPPFLAGS"])
  if test "x$with_parallel_ast_traversal_mpi" = xyes; then
    AC_MSG_ERROR([PCH support cannot be configured together with MPI support])
  fi
  if test "x$with_parallel_ast_traversal_omp" = xyes; then
    AC_MSG_ERROR([PCH Support cannot be configured together with GCC_OMP support])
  fi
else
  AC_MSG_NOTICE("PCH disabled: no Support for PCH")
fi

# DQ (9/4/2009): Added checking for indent command (common in Linux, but not on some platforms).
# This command is used in the tests/nonsmoke/functional/roseTests/astInterfaceTests/Makefile.am file.
AC_CHECK_PROGS(INDENT, [indent])
AM_CONDITIONAL(ROSE_USE_INDENT, [test "x$INDENT" = "xindent"])
AC_MSG_NOTICE([INDENT = "$INDENT"])

# DQ (9/30/2009): Added checking for tclsh command (common in Linux, but not on some platforms).
AC_CHECK_PROGS(TCLSH, [tclsh])
AM_CONDITIONAL(ROSE_USE_TCLSH, [test "x$TCLSH" = "xtclsh"])
AC_MSG_NOTICE([TCLSH = "$TCLSH"])

# Call supporting macro for OFP
ROSE_SUPPORT_OFP

ROSE_SUPPORT_CUDA

# *****************************************************************
#            Accelerator Support (CUDA, OpenCL)
# *****************************************************************

# Check: --with-cuda-inc, --with-cuda-lib, and  --with-cuda-bin
ROSE_CHECK_CUDA
# Check: --with-opencl-inc, --with-opencl-lib
ROSE_CHECK_OPENCL

# *****************************************************************
#            Option to define DOXYGEN SUPPORT
# *****************************************************************

# allow either user or developer level documentation using Doxygen
ROSE_SUPPORT_DOXYGEN

# DQ (8/25/2004): Disabled fast docs option.
# Setup Automake conditional to allow use of Doxygen Tag file to speedup
# generation of Rose documentation this does not however provide the
# best organized documentation so we use it as an option to speed up
# the development of the documenation and then alternatively build the
# final documentation.
# AM_CONDITIONAL(DOXYGEN_GENERATE_FAST_DOCS,test "$enable_doxygen_generate_fast_docs" = yes)
# echo "In configure.in: enable_doxygen_generate_fast_docs = $enable_doxygen_generate_fast_docs"

# Test for setup of document merge of Sage docs with Rose docs
# Causes document build process to take longer but builds better documentation
if (test "$enable_doxygen_generate_fast_docs" = yes) ; then
   AC_MSG_NOTICE([generate Doxygen documentation faster (using tag file mechanism)])
else
   AC_MSG_NOTICE([generate Doxygen documentation slower (reading all of Sage III and Rose together)])
fi

AC_PROG_CXXCPP
dnl AC_PROG_RANLIB
# echo "In configure.in (before libtool win32 setup): libtool test for 64 bit libs = `/usr/bin/file conftest.o`"
dnl AC_LIBTOOL_WIN32_DLL -- ROSE is probably not set up for this

# echo "In configure.in (before libtool setup): disabling static libraries by default (use --enable-static or --enable-static= to override)"
AC_DISABLE_STATIC

# echo "In configure.in (before libtool setup): libtool test for 64 bit libs = `/usr/bin/file conftest.o`"
LT_AC_PROG_SED dnl This seems to not be called, even though it is needed in the other macros
m4_pattern_allow([LT_LIBEXT])dnl From http://www.mail-archive.com/libtool-commit@gnu.org/msg01369.html

# Liao 8/17/2010. Tried to work around a undefined SED on NERSC hopper.
# But this line is expanded after AC_PROG_LIBTOOL.
# I had to promote it to configure.in, right before calling  ROSE_SUPPORT_ROSE_PART_2
#test -z "$SED" && SED=sed

AC_PROG_LIBTOOL
AC_LIBLTDL_CONVENIENCE dnl We need to use our version because libtool can't handle when we use libtool v2 but the v1 libltdl is installed on a system
AC_SUBST(LTDLINCL)
AC_SUBST(LIBLTDL)
AC_LIBTOOL_DLOPEN
AC_LIB_LTDL(recursive)
dnl AC_LT DL_SHLIBPATH dnl Get the environment variable like LD_LIBRARY_PATH for the Fortran support to use
dnl This seems to be an internal variable, set by different macros in different
dnl Libtool versions, but with the same name
AC_DEFINE_UNQUOTED(ROSE_SHLIBPATH_VAR, ["$shlibpath_var"], [Variable like LD_LIBRARY_PATH])

#echo 'int i;' > conftest.$ac_ext
AC_TRY_EVAL(ac_compile);
# echo "In configure.in (after libtool setup): libtool test for 64 bit libs = `/usr/bin/file conftest.o`"

# Various functions for finding the location of librose.* (used to make the
# ROSE executables relocatable to different locations without recompilation on
# some platforms)
AC_CHECK_HEADERS([dlfcn.h], [have_dladdr=yes], [have_dladdr=no])
if test "x$have_dladdr" = "xyes"; then
  AC_CHECK_LIB([dl], [dladdr], [], [have_dladdr=no])
fi
if test "x$have_dladdr" = "xyes"; then
  AC_DEFINE([HAVE_DLADDR], [], [Whether <dlfcn.h> and -ldl contain dladdr()])
  use_rose_in_build_tree_var=no
else
  AC_MSG_WARN([ROSE cannot find the locations of loaded shared libraries using your dynamic linker.  ROSE can only be used with the given build directory or prefix, and the ROSE_IN_BUILD_TREE environment variable must be used to distinguish the two cases.])
  use_rose_in_build_tree_var=yes
fi
AM_CONDITIONAL(USE_ROSE_IN_BUILD_TREE_VAR, [test "x$use_rose_in_build_tree_var" = "xyes"])

# Figure out what version of lex we have available
# flex works better than lex (this gives a preference to flex (flex is gnu))
dnl AM_PROG_LEX
dnl AC_SUBST(LEX)
# This will work with flex and lex (but flex will not set LEXLIB to -ll unless it finds the gnu
# flex library which is not often installed (and at any rate not installed on our system at CASC)).
# Once the lex file contains its own version of yywrap then we will not need this set explicitly.

# next two lines commented out by BP : 10/29/2001,
# the flex library IS installed on our systems, setting it to -ll causes problems on
# Linux systems
# echo "Setting LEXLIB explicitly to -ll (even if flex is used: remove this once lex file contains it's own version of yywrap)"
# dnl LEXLIB='-ll'
# dnl AC_SUBST(LEXLIB)

# Determine what C++ compiler is being used.
AC_MSG_CHECKING(what the C++ compiler $CXX really is)
BTNG_INFO_CXX_ID
AC_MSG_RESULT($CXX_ID-$CXX_VERSION)

# Define various C++ compiler options.
# echo "Before ROSE_FLAG _ CXX_OPTIONS macro"
# ROSE_FLAG_C_OPTIONS
# ROSE_FLAG_CXX_OPTIONS
# echo "Outside of ROSE_FLAG _ CXX_OPTIONS macro: CXX_DEBUG= $CXX_DEBUG"

# DQ (7/8/2004): Added support for shared libraries using Brian's macros
# ROSE_TEST_LIBS="-L`pwd`/src"

# DQ (9/7/2006): build the directory where libs will be placed.
# mkdir -p $prefix/libs
# echo "Before calling \"mkdir -p $prefix/lib\": prefix = $prefix"
# mkdir -p $prefix/lib

# DQ (1/14/2007): I don't think this is required any more!
# ROSE_TEST_LIBS="-L$prefix/lib"

# Determine how to create C++ libraries.
AC_MSG_CHECKING(how to create C++ libraries)
BTNG_CXX_AR
AC_MSG_RESULT($CXX_STATIC_LIB_UPDATE and $CXX_DYNAMIC_LIB_UPDATE)

# DQ (6/23/2004) Commented out due to warning in running build
# I do not know why in this case, INCLUDES is not generically
# defined and automatically substituted.  It usually is.  BTNG.
# INCLUDES='-I. -I$(srcdir) -I$(top_builddir)'
# AC_SUBST(INCLUDES)

# We don't need to select between SAGE 2 and SAGE 3 anymore (must use SAGE 3)
# SAGE_VAR_INCLUDES_AND_LIBS

# Let user specify where to find A++P++ installation.
# Specify by --with-AxxPxx= or setting AxxPxx_PREFIX.
# Note that the prefix specified should be that specified
# when installing A++P++.  The prefix appendages are also
# added here.
# BTNG.
AC_MSG_CHECKING(for A++P++)
AC_ARG_WITH(AxxPxx,
[  --with-AxxPxx=PATH   Specify the prefix where A++P++ is installed],
,
if test "$AxxPxx_PREFIX" ; then
   with_AxxPxx="$AxxPxx_PREFIX"
else
   with_AxxPxx=no
fi
)
test "$with_AxxPxx" && test "$with_AxxPxx" != no && AxxPxx_PREFIX="$with_AxxPxx"
AC_MSG_RESULT($AxxPxx_PREFIX)
if test "$AxxPxx_PREFIX" ; then
  # Note that the prefix appendages are added to AxxPxx_PREFIX to find A++ and P++.
  AC_MSG_RESULT(using $AxxPxx_PREFIX as path to A++ Library)
  Axx_INCLUDES="-I$AxxPxx_PREFIX/A++/lib/include"
  Axx_LIBS="-L$AxxPxx_PREFIX/A++/lib/lib -lApp -lApp_static -lApp"
  Pxx_INCLUDES="-I$AxxPxx_PREFIX/P++/lib/include"
  Pxx_LIBS="-L$AxxPxx_PREFIX/P++/lib/lib -lApp -lApp_static -lApp"
  # optional_AxxPxxSpecificExample_subdirs="EXAMPLES"
  # we will want to setup subdirectories in the TESTS directory later so set it up now
  # optional_AxxPxxSpecificTest_subdirs="A++Tests"
else
  AC_MSG_RESULT(No path specified for A++ Library)
fi
AC_SUBST(Axx_INCLUDES)
AC_SUBST(Axx_LIBS)
AC_SUBST(Pxx_INCLUDES)
AC_SUBST(Pxx_LIBS)
# AC_SUBST(optional_AxxPxxSpecificExample_subdirs)
# AC_SUBST(optional_AxxPxxSpecificTest_subdirs)
# Do not append to INCLUDES and LIBS because Axx is not needed everywhere.
# It is only needed in EXAMPLES.
# Set up A++/P++ directories that require A++/P++ Libraries (EXAMPLES)
AM_CONDITIONAL(AXXPXX_SPECIFIC_TESTS,test ! "$with_AxxPxx" = no)

# BTNG_CHOOSE_STL defines STL_DIR and STL_INCLUDES
# BTNG_CHOOSE_STL
# echo "STL_INCLUDE = $STL_INCLUDE"
# AC _SUB ST(STL_INCLUDES)
# AC _SUB ST(STL_DIR)

ROSE_CONFIGURE_SECTION([Checking system capabilities])

AC_SEARCH_LIBS(clock_gettime, [rt], [
  RT_LIBS="$LIBS"
  LIBS=""
],[
  RT_LIBS=""
])
AC_SUBST(RT_LIBS)

# DQ (12/16/2009): This option is now removed since the developersScratchSpace has been
# removed from the ROSE's git repository and it is a separate git repository that can be
# checked out internally by ROSE developers.
# Set up for Dan Quinlan's development test directory.
# AC_ARG_ENABLE(dq-developer-tests,
# [--enable-dq-developer-tests   Development option for Dan Quinlan (disregard).],
# [ echo "Setting up optional ROSE/developersScratchSpace/Dan directory"
# if test -d ${srcdir}/developersScratchSpace; then
#   :
# else
#   echo "This is a non-developer version of ROSE (source distributed with EDG binary)"
#   enable_dq_developer_tests=no
# fi
# ])
# AM_CONDITIONAL(DQ_DEVELOPER_TESTS,test "$enable_dq_developer_tests" = yes)

## This should be set after a complex test (turn it on as default)
AC_DEFINE([HAVE_EXPLICIT_TEMPLATE_INSTANTIATION],[],[Use explicit template instantiation.])

# Copied from the P++/configure.in
# Determine how to build a C++ library.
AC_MSG_CHECKING(how to build C++ libraries)
BTNG_CXX_AR
if test "$CXX_ID" = ibm; then
  # IBM does not have a method for supporting shared libraries
  # Here is a kludge.
  CXX_SHARED_LIB_UPDATE="`cd ${srcdir}/../config && pwd`/mklib.aix -o"
  BTNG_AC_LOG(CXX_SHARED_LIB_UPDATE changed to $CXX_SHARED_LIB_UPDATE especially for the IBM)
fi
AC_MSG_RESULT($CXX_STATIC_LIB_UPDATE and $CXX_SHARED_LIB_UPDATE)
AC_SUBST(CXX_STATIC_LIB_UPDATE)
AC_SUBST(CXX_SHARED_LIB_UPDATE)

# The STL tests use the CC command line which specifies -ptr$(CXX_TEMPLATE_REPOSITORY_PATH) but this
# is not defined in the shell so no substitution is done and a directory named
# $(CXX_TEMPLATE_REPOSITORY_PATH) is built in the top level directory.  The least we can do is
# delete it if we can't stop it from being generated.
# AC_MSG_RESULT(deleting temporary template directory built during STL tests.)
# rm -rf '$(CXX_TEMPLATE_REPOSITORY_PATH)'
rm -rf Templates.DB

# End macro ROSE_SUPPORT_ROSE_PART_2.
]
)

#-----------------------------------------------------------------------------


AC_DEFUN([ROSE_SUPPORT_ROSE_PART_3],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_3.

## Setup the EDG specific stuff
SETUP_EDG


ROSE_ARG_ENABLE(
  [alternate-edg-build-cpu],
  [for alternate EDG build cpu],
  [allows you to generate EDG binaries with a different CPU type in the name string]
)

#The build_triplet_without_redhat variable is used only in src/frontend/CxxFrontend/Makefile.am to determine the binary edg name
build_triplet_without_redhat=`${srcdir}/config/cleanConfigGuessOutput "$build" "$build_cpu" "$build_vendor"`
if test "x$CONFIG_HAS_ROSE_ENABLE_ALTERNATE_EDG_BUILD_CPU" = "xyes"; then
  # Manually modify the build CPU <build_cpu>-<build_vendor>-<build>
  build_triplet_without_redhat="$(echo "$build_triplet_without_redhat" | sed 's/^[[^-]]*\(.*\)/'$ROSE_ENABLE_ALTERNATE_EDG_BUILD_CPU'\1/')"
fi
AC_SUBST(build_triplet_without_redhat) dnl This is done even with EDG source, since it is used to determine the binary to make in roseFreshTest

# End macro ROSE_SUPPORT_ROSE_PART_3.
])

#-----------------------------------------------------------------------------

AC_DEFUN([ROSE_SUPPORT_ROSE_PART_4],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_4.

dnl ---------------------------------------------------------------------
dnl (8/29/2007): This was added to provide more portable times upon the
dnl suggestion of Matt Sottile at LANL.
dnl ---------------------------------------------------------------------
AC_C_INLINE
AC_HEADER_TIME
AC_CHECK_HEADERS([sys/time.h c_asm.h intrinsics.h mach/mach_time.h])

AC_CHECK_TYPE([hrtime_t],[AC_DEFINE(HAVE_HRTIME_T, 1, [Define to 1 if hrtime_t is defined in <sys/time.h>])],,[#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif])

AC_CHECK_HEADERS(pthread.h)

AC_CHECK_FUNCS([gethrtime read_real_time time_base_to_time clock_gettime mach_absolute_time])

dnl Cray UNICOS _rtc() (real-time clock) intrinsic
AC_MSG_CHECKING([for _rtc intrinsic])
rtc_ok=yes
AC_TRY_LINK([#ifdef HAVE_INTRINSICS_H
#include <intrinsics.h>
#endif], [_rtc()], [AC_DEFINE(HAVE__RTC,1,[Define if you have the UNICOS _rtc() intrinsic.])], [rtc_ok=no])
AC_MSG_RESULT($rtc_ok)
dnl ---------------------------------------------------------------------


# Record the location of the build tree (so it can be substituted into ROSE/docs/Rose/rose.cfg)
top_pwd=$PWD
AC_SUBST(top_pwd)
# echo "In ROSE/con figure: top_pwd = $top_pwd"

absolute_path_srcdir="`cd $srcdir; pwd`"
AC_SUBST(absolute_path_srcdir)

# Liao 6/20/2011, store source path without symbolic links, used to have consistent source and compile paths for ROSE
# when call graph analysis tests are used.
res_top_src=$(cd "$srcdir" && pwd -P)
AC_DEFINE_UNQUOTED([ROSE_SOURCE_TREE_PATH],"$res_top_src",[Location of ROSE Source Tree.])

# kelly64 (6/26/2013): Compass2 xml configuration files require fully-resolved
#                      absolute paths.
AC_SUBST(res_top_src)

# PC (08/20/2009): Symbolic links need to be resolved for the callgraph analysis tests
res_top_pwd=$(cd "$top_pwd" && pwd -P)

# DQ (11/10/2007): Add paths defined by automake to the generated rose.h.in and rose.h
# header files so that this information will be available at compile time. Unclear
# which syntax is best for the specification of these paths.
AC_DEFINE_UNQUOTED([ROSE_COMPILE_TREE_PATH],"$res_top_pwd",[Location of ROSE Compile Tree.])

# This block turns off features of libharu that don't work with Java
with_png=no
export with_png
with_zlib=no
export with_zlib

# Added support for detection of libnuma, a NUMA aware memory allocation mechanism for many-core optimizations.
AC_CHECK_HEADERS(numa.h, [found_libnuma=yes])

if test "x$found_libnuma" = xyes; then
  AC_DEFINE([HAVE_NUMA_H],[],[Support for libnuma a NUMA memory allocation library for many-core optimizations])
fi

AM_CONDITIONAL(ROSE_USE_LIBNUMA, [test "x$found_libnuma" = xyes])


# PC (7/10/2009): The Haskell build system expects a fully numeric version number.
PACKAGE_VERSION_NUMERIC=`echo $PACKAGE_VERSION | sed -e 's/\([[a-z]]\+\)/\.\1/; y/a-i/1-9/'`
AC_SUBST(PACKAGE_VERSION_NUMERIC)

# This CPP symbol is defined so we can check whether rose_config.h is included into a public header file.  It serves
# no other purpose.  The name must not begin with "ROSE_" but must have a high probability of being globally unique (which
# is why it ends with "_ROSE").
AC_DEFINE(CONFIG_ROSE, 1, [Always defined and used for checking whether global CPP namespace is polluted])

# End macro ROSE_SUPPORT_ROSE_PART_4.
]
)




dnl ---------------------------------------------------------------
dnl CLASSPATH_COND_IF(COND, SHELL-CONDITION, [IF-TRUE], [IF-FALSE])
dnl ---------------------------------------------------------------
dnl Automake 1.11 can emit conditional rules for AC_CONFIG_FILES,
dnl using AM_COND_IF.  This wrapper uses it if it is available,
dnl otherwise falls back to code compatible with Automake 1.9.6.
AC_DEFUN([CLASSPATH_COND_IF],
[m4_ifdef([AM_COND_IF],
  [AM_COND_IF([$1], [$3], [$4])],
  [if $2; then
     m4_default([$3], [:])
   else
     m4_default([$4], [:])
   fi
])])

#-----------------------------------------------------------------------------

AC_DEFUN([ROSE_SUPPORT_ROSE_PART_5],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_5.

# DQ (9/21/2009): Debugging for RH release 5
AC_MSG_NOTICE([CC = "$CC"])
AC_MSG_NOTICE([CPPFLAGS = "$CPPFLAGS"])

AC_MSG_NOTICE([subdirs = "$subdirs"])
AC_CONFIG_SUBDIRS([libltdl])

# This list should be the same as in build (search for Makefile.in)

CLASSPATH_COND_IF([ROSE_HAS_EDG_SOURCE], [test "x$has_edg_source" = "xyes"], [
AC_CONFIG_FILES([
src/frontend/CxxFrontend/EDG/Makefile
src/frontend/CxxFrontend/EDG/EDG_5.0/Makefile
src/frontend/CxxFrontend/EDG/EDG_5.0/misc/Makefile
src/frontend/CxxFrontend/EDG/EDG_5.0/src/Makefile
src/frontend/CxxFrontend/EDG/EDG_5.0/src/disp/Makefile
src/frontend/CxxFrontend/EDG/EDG_5.0/lib/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.0/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.0/misc/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.0/src/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.0/src/disp/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.0/lib/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.3/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.3/misc/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.3/src/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.3/src/disp/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.3/lib/Makefile
src/frontend/CxxFrontend/EDG/edgRose/Makefile
])], [])

# End macro ROSE_SUPPORT_ROSE_PART_5.
]
)


#-----------------------------------------------------------------------------
AC_DEFUN([ROSE_SUPPORT_ROSE_PART_6],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_6.

# RV 9/14/2005: Removed src/3rdPartyLibraries/PDFLibrary/Makefile
# JJW 1/30/2008: Removed rose_paths.h as it is now built by a separate Makefile included from $(top_srcdir)/Makefile.am
AC_CONFIG_FILES([
Makefile
config/Makefile
docs/Makefile
docs/Rose/Makefile
docs/Rose/ROSE_DemoGuide.tex
docs/Rose/ROSE_DeveloperInstructions.tex
docs/Rose/ROSE_Exam.tex
docs/Rose/ROSE_InstallationInstructions.tex
docs/Rose/Tutorial/Makefile
docs/Rose/Tutorial/gettingStarted.tex
docs/Rose/Tutorial/tutorial.tex
docs/Rose/footer.html
docs/Rose/gettingStarted.tex
docs/Rose/leftmenu.html
docs/Rose/manual.tex
docs/Rose/rose-install-demo.cfg
docs/Rose/rose.cfg
docs/Rose/roseQtWidgets.doxygen
docs/Rose/sage.cfg
docs/testDoxygen/Makefile
docs/testDoxygen/test.cfg
exampleTranslators/AstCopyReplTester/Makefile
exampleTranslators/DOTGenerator/Makefile
exampleTranslators/Makefile
exampleTranslators/PDFGenerator/Makefile
exampleTranslators/defaultTranslator/Makefile
exampleTranslators/documentedExamples/Makefile
exampleTranslators/documentedExamples/simpleTranslatorExamples/Makefile
exampleTranslators/documentedExamples/simpleTranslatorExamples/exampleMakefile
LicenseInformation/Makefile
scripts/Makefile
src/3rdPartyLibraries/json/Makefile
src/3rdPartyLibraries/json/nlohmann/Makefile
src/3rdPartyLibraries/Makefile
src/3rdPartyLibraries/antlr-jars/Makefile
src/3rdPartyLibraries/fortran-parser/Makefile
src/Makefile
src/ROSETTA/Makefile
src/ROSETTA/src/Makefile
src/backend/Makefile
src/frontend/CxxFrontend/Clang/Makefile
src/frontend/CxxFrontend/Makefile
src/frontend/Experimental_General_Language_Support/Makefile
src/frontend/Makefile
src/frontend/OpenFortranParser_SAGE_Connection/Makefile
src/frontend/SageIII/GENERATED_CODE_DIRECTORY_Cxx_Grammar/Makefile
src/frontend/SageIII/Makefile
src/frontend/SageIII/astFixup/Makefile
src/frontend/SageIII/astHiddenTypeAndDeclarationLists/Makefile
src/frontend/SageIII/astPostProcessing/Makefile
src/frontend/SageIII/astTokenStream/Makefile
src/frontend/SageIII/includeDirectivesProcessing/Makefile
src/frontend/SageIII/sage.docs
src/frontend/SageIII/sageInterface/Makefile
src/frontend/SageIII/ompparser/Makefile
src/frontend/SageIII/accparser/Makefile
src/frontend/SageIII/virtualCFG/Makefile
src/midend/Makefile
src/midend/programAnalysis/Makefile
src/midend/astDump/Makefile
src/midend/programTransformation/extractFunctionArgumentsNormalization/Makefile
src/midend/programTransformation/loopProcessing/Makefile
src/midend/programTransformation/singleStatementToBlockNormalization/Makefile
src/util/Makefile
src/util/commandlineProcessing/Makefile
src/util/graphs/Makefile
src/util/StringUtility/Makefile
src/util/support/Makefile
stamp-h
tests/Makefile
tests/nonsmoke/ExamplesForTestWriters/Makefile
tests/nonsmoke/Makefile
tests/nonsmoke/acceptance/Makefile
tests/nonsmoke/functional/CompileTests/A++Code/Makefile
tests/nonsmoke/functional/CompileTests/A++Tests/Makefile
tests/nonsmoke/functional/CompileTests/C_tests/Makefile
tests/nonsmoke/functional/CompileTests/C_subset_of_Cxx_tests/Makefile
tests/nonsmoke/functional/CompileTests/C89_std_c89_tests/Makefile
tests/nonsmoke/functional/CompileTests/C99_tests/Makefile
tests/nonsmoke/functional/CompileTests/C11_tests/Makefile
tests/nonsmoke/functional/CompileTests/CudaTests/Makefile
tests/nonsmoke/functional/CompileTests/Cxx_tests/Makefile
tests/nonsmoke/functional/CompileTests/Cxx03_tests/Makefile
tests/nonsmoke/functional/CompileTests/Cxx11_tests/Makefile
tests/nonsmoke/functional/CompileTests/Cxx14_tests/Makefile
tests/nonsmoke/functional/CompileTests/Cxx17_tests/Makefile
tests/nonsmoke/functional/CompileTests/Cxx20_tests/Makefile
tests/nonsmoke/functional/CompileTests/ElsaTestCases/Makefile
tests/nonsmoke/functional/CompileTests/ElsaTestCases/ctests/Makefile
tests/nonsmoke/functional/CompileTests/ElsaTestCases/gnu/Makefile
tests/nonsmoke/functional/CompileTests/ElsaTestCases/kandr/Makefile
tests/nonsmoke/functional/CompileTests/ElsaTestCases/std/Makefile
tests/nonsmoke/functional/CompileTests/ExpressionTemplateExample_tests/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/LANL_POP/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/experimental_frontend_tests/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/gfortran.dg/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/gfortran.fortran-torture/Makefile
tests/nonsmoke/functional/CompileTests/Makefile
tests/nonsmoke/functional/CompileTests/OpenClTests/Makefile
tests/nonsmoke/functional/CompileTests/OpenMP_tests/Makefile
tests/nonsmoke/functional/CompileTests/OpenMP_tests/cvalidation/Makefile
tests/nonsmoke/functional/CompileTests/OpenMP_tests/fortran/Makefile
tests/nonsmoke/functional/CompileTests/OvertureCode/Makefile
tests/nonsmoke/functional/CompileTests/P++Tests/Makefile
tests/nonsmoke/functional/CompileTests/RoseExample_tests/Makefile
tests/nonsmoke/functional/CompileTests/STL_tests/Makefile
tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/Makefile
tests/nonsmoke/functional/CompileTests/UnparseHeadersUsingTokenStream_tests/Makefile
tests/nonsmoke/functional/CompileTests/boost_tests/Makefile
tests/nonsmoke/functional/CompileTests/colorAST_tests/Makefile
tests/nonsmoke/functional/CompileTests/copyAST_tests/Makefile
tests/nonsmoke/functional/CompileTests/frontend_integration/Makefile
tests/nonsmoke/functional/CompileTests/hiddenTypeAndDeclarationListTests/Makefile
tests/nonsmoke/functional/CompileTests/mergeAST_tests/Makefile
tests/nonsmoke/functional/CompileTests/mixLanguage_tests/Makefile
tests/nonsmoke/functional/CompileTests/nameQualificationAndTypeElaboration_tests/Makefile
tests/nonsmoke/functional/CompileTests/sizeofOperation_tests/Makefile
tests/nonsmoke/functional/CompileTests/sourcePosition_tests/Makefile
tests/nonsmoke/functional/CompileTests/staticCFG_tests/Makefile
tests/nonsmoke/functional/CompileTests/systemc_tests/Makefile
tests/nonsmoke/functional/CompileTests/uninitializedField_tests/Makefile
tests/nonsmoke/functional/CompileTests/unparseToString_tests/Makefile
tests/nonsmoke/functional/CompileTests/virtualCFG_tests/Makefile
tests/nonsmoke/functional/CompileTests/unparse_template_from_ast/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/collectAllCommentsAndDirectives_tests/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/preinclude_tests/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testCpreprocessorOption/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testFileNamesAndExtensions/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testForSpuriousOutput/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testGenerateSourceFileNames/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testGnuOptions/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testHeaderFileOutput/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testIncludeOptions/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testOutputFileOption/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testNostdincOption/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testAnsiOption/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testWave/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/tokenStream_tests/Makefile
tests/nonsmoke/functional/Makefile
tests/nonsmoke/functional/moveDeclarationTool/Makefile
tests/nonsmoke/functional/RunTests/A++Tests/Makefile
tests/nonsmoke/functional/RunTests/AstDeleteTests/Makefile
tests/nonsmoke/functional/RunTests/FortranTests/LANL_POP/Makefile
tests/nonsmoke/functional/RunTests/FortranTests/Makefile
tests/nonsmoke/functional/RunTests/Makefile
tests/nonsmoke/functional/UnitTests/Makefile
tests/nonsmoke/functional/UnitTests/Rose/Makefile
tests/nonsmoke/functional/UnitTests/Rose/SageBuilder/Makefile
tests/nonsmoke/functional/Utility/Makefile
tests/nonsmoke/functional/roseTests/Makefile
tests/nonsmoke/functional/roseTests/ROSETTA/Makefile
tests/nonsmoke/functional/roseTests/astInliningTests/Makefile
tests/nonsmoke/functional/roseTests/astInterfaceTests/Makefile
tests/nonsmoke/functional/roseTests/astInterfaceTests/typeEquivalenceTests/Makefile
tests/nonsmoke/functional/roseTests/astInterfaceTests/unitTests/Makefile
tests/nonsmoke/functional/roseTests/astLValueTests/Makefile
tests/nonsmoke/functional/roseTests/astMergeTests/Makefile
tests/nonsmoke/functional/roseTests/astOutliningTests/Makefile
tests/nonsmoke/functional/roseTests/astPerformanceTests/Makefile
tests/nonsmoke/functional/roseTests/astProcessingTests/Makefile
tests/nonsmoke/functional/roseTests/astQueryTests/Makefile
tests/nonsmoke/functional/roseTests/astSymbolTableTests/Makefile
tests/nonsmoke/functional/roseTests/astTokenStreamTests/Makefile
tests/nonsmoke/functional/roseTests/fileLocation_tests/Makefile
tests/nonsmoke/functional/roseTests/loopProcessingTests/Makefile
tests/nonsmoke/functional/roseTests/mergeTraversal_tests/Makefile
tests/nonsmoke/functional/roseTests/ompLoweringTests/Makefile
tests/nonsmoke/functional/roseTests/ompLoweringTests/fortran/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/defUseAnalysisTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/generalDataFlowAnalysisTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/staticInterproceduralSlicingTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/testCallGraphAnalysis/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/typeTraitTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/variableLivenessTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/variableRenamingTests/Makefile
tests/nonsmoke/functional/roseTests/programTransformationTests/Makefile
tests/nonsmoke/functional/roseTests/programTransformationTests/extractFunctionArgumentsTest/Makefile
tests/nonsmoke/functional/roseTests/programTransformationTests/singleStatementToBlockNormalization/Makefile
tests/nonsmoke/functional/roseTests/varDeclNorm/Makefile
tests/nonsmoke/functional/testSupport/Makefile
tests/nonsmoke/functional/testSupport/gtest/Makefile
tests/nonsmoke/functional/translatorTests/Makefile
tests/nonsmoke/specimens/Makefile
tests/nonsmoke/specimens/c++/Makefile
tests/nonsmoke/specimens/c/Makefile
tests/nonsmoke/specimens/fortran/Makefile
tests/nonsmoke/unit/Makefile
tests/nonsmoke/unit/SageInterface/Makefile
tests/roseTests/Makefile
tests/roseTests/ompLoweringTests/Makefile
tests/roseTests/programAnalysisTests/Makefile
tests/roseTests/programAnalysisTests/typeTraitTests/Makefile
tests/smoke/ExamplesForTestWriters/Makefile
tests/smoke/Makefile
tests/smoke/functional/Fortran/Makefile
tests/smoke/functional/Makefile
tests/smoke/specimens/Makefile
tests/smoke/specimens/c++/Makefile
tests/smoke/specimens/c/Makefile
tests/smoke/specimens/fortran/Makefile
tests/smoke/unit/Boost/Makefile
tests/smoke/unit/Makefile
tests/smoke/unit/Utility/Makefile
tools/Makefile
tools/globalVariablesInLambdas/Makefile
tools/classMemberVariablesInLambdas/Makefile
tools/checkFortranInterfaces/Makefile
tools/featureVector/Makefile
tutorial/Makefile
tutorial/exampleMakefile
tutorial/outliner/Makefile
])

# DQ (3/8/2017): Removed these directories from testing (pre-smoke and pre-nonsmoke test directories.
# tests/CompileTests/Makefile
# tests/CompileTests/OpenMP_tests/Makefile

# DQ (10/27/2010): New Fortran tests (from gfortan test suite).
# tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/Makefile
# tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/gfortran.fortran-torture/Makefile
# tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/gfortran.dg/Makefile

# DQ (8/12/2010): We want to get permission to distribute these files as test codes.
# tests/nonsmoke/functional/CompileTests/Fortran_tests/LANL_POP/Makefile

# DQ (12/31/2008): Skip these, since we don't have SPEC and NAS benchmarks setup yet.
# developersScratchSpace/Dan/Fortran_tests/NPB3.2-SER/Makefile
# developersScratchSpace/Dan/Fortran_tests/NPB3.2-SER/BT/Makefile
# developersScratchSpace/Dan/SpecCPU2006/Makefile
# developersScratchSpace/Dan/SpecCPU2006/config/Makefile
# developersScratchSpace/Dan/SpecCPU2006/config/rose.cfg

# DQ (9/12/2008): Removed older version of QRose (now an external project)
# src/roseIndependentSupport/graphicalUserInterface/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/QRTree/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/QRCodeBox/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/QRGui/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/QRGui/icons22/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/QRQueryBox/Makefile
# exampleTranslators/graphicalUserInterfaceExamples/Makefile
# exampleTranslators/graphicalUserInterfaceExamples/slicing/Makefile
# exampleTranslators/graphicalUserInterfaceExamples/attributes/Makefile
# exampleTranslators/graphicalUserInterfaceExamples/query/Makefile
# exampleTranslators/graphicalUserInterfaceExamples/layout/Makefile

# End macro ROSE_SUPPORT_ROSE_PART_6.
]
)


#-----------------------------------------------------------------------------


AC_DEFUN([ROSE_SUPPORT_ROSE_PART_7],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_7.

AC_CONFIG_COMMANDS([default],
    [
     AC_MSG_NOTICE([ensuring Grammar in the compile tree (assuming source tree is not the same as the compile tree)])
     pathToSourceDir="`cd $srcdir && pwd`"
     test -d src/ROSETTA/Grammar || ( rm -rf src/ROSETTA/Grammar && ln -s "$pathToSourceDir/src/ROSETTA/Grammar" src/ROSETTA/Grammar )
    ],
    [])

# Generate rose_paths.C
AC_CONFIG_COMMANDS([rose_paths.C], [
    AC_MSG_NOTICE([building src/util/rose_paths.C])
    make src/util/rose_paths.C
])

# Generate public config file from private config file. The public config file adds "ROSE_" to the beginning of
# certain symbols. See scripts/publicConfiguration.pl for details.
AC_CONFIG_COMMANDS([rosePublicConfig.h],[
    AC_MSG_NOTICE([building rosePublicConfig.h])
    make rosePublicConfig.h
])

# [TOO1, 2014-04-22]
# TODO: Re-enable once we phase out support for older version of Autotools.
#       Specifically, Pontetec is using Autoconf 2.59 and Automake 1.9.6.
# Rewrite the definitions for srcdir, top_srcdir, builddir, and top_builddir so they use the "abs_" versions instead.
#AC_CONFIG_COMMANDS([absoluteNames],
#[[
#       echo "rewriting makefiles to use absolute paths for srcdir, top_srcdir, builddir, and top_builddir..."
#       find . -name Makefile | xargs sed -i~ \
#           -re 's/^(srcdir|top_srcdir|builddir|top_builddir) = \..*/\1 = $(abs_\1)/'
#]])



# End macro ROSE_SUPPORT_ROSE_PART_7.
]
)

