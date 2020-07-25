AC_DEFUN([ROSE_SUPPORT_LANGUAGE_CONFIG_OPTIONS],
[
#  TOO (3/10/2011):
#
#	1. Manage language support command-line options
#	2. Set flags to indicate which languages to support 
#	3. Output language support (debugging)
#	4. Enabled only one language
#	5. Set the automake conditional macros that will be used in Makefiles
#
#  DQ (4/15/2010): Added support to specify selected languages to support in ROSE.
#########################################################################################
ROSE_CONFIGURE_SECTION([Checking analyzable languages])

#########################################################################################
#
#  Manage language support command-line options:
#
#	--enable-languages(=args)
#	--enable-c
#	--enable-cxx
#	--enable-cuda
#	--enable-fortran
#	--enable-opencl
#
#########################################################################################

#########################################################################################
#
##
  ALL_SUPPORTED_LANGUAGES="c c++ cuda fortran opencl"
##
#
#########################################################################################

AC_ARG_ENABLE([languages],
               AS_HELP_STRING([--enable-languages=LIST],[Build specific languages: all,none,c,c++,cuda,fortran,opencl (default=all)]),,
               [enableval=all])

	       # Default support for all languages
	       case "$enableval" in
 	         all|yes)
		 	LANGUAGES_TO_SUPPORT="$ALL_SUPPORTED_LANGUAGES"
		 	;;
                 *)
                 	LANGUAGES_TO_SUPPORT="$enableval"
                 	;;
	       esac

# Convert support-language-list to a space-separated list, stripping
# leading and trailing whitespace
LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed -e 's/,/ /g;s/^[ \t]*//;s/[ \t]*$//'`" 
#DEBUG#echo "LANGUAGES_TO_SUPPORT='$LANGUAGES_TO_SUPPORT'"

AC_ARG_ENABLE([c],
               AS_HELP_STRING([--enable-c],[Enable C language support in ROSE (default=yes). Note:  C++ support must currently be simultaneously enabled/disabled]),
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "\bc\b"
                if test $? = 0 ; then 
                  list_has_c=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_c" != "xyes" ; then
                          # --enable-languages does not include C, but --enable-c=yes
                  	  LANGUAGES_TO_SUPPORT+=" c"
                        fi
                  	;;
                  [no)]
                        list_has_cxx="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i == "c++") { printf "yes"; } } }']`"
                        if test "x$list_has_cxx" = "xyes" && test "x$enable_cxx" != "xno" ; then
                  	  [AC_MSG_FAILURE([cannot disable 'C' language support because 'C++' language support is enabled -- currently both are required to be supported together. If you really don't want 'C' language support, please also disable 'C++' language support (see ./configure --help)])]
                        else
                          # remove 'C' from support languages list
                          # TOO (3/11/2011): couldn't find a nice way to handle with sed, cases: "c", "c c++", ...
                  	  LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i != "c") { printf "%s ",$i; } } }']`" 
                        fi
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-c="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,)
AC_ARG_ENABLE([cxx],
               AS_HELP_STRING([--enable-cxx],[Enable C++ language support in ROSE (default=yes). Note: C support must currently be simultaneously enabled/disabled]),
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_cxx" != "xyes" ; then
                          # --enable-languages does not include C++, but --enable-cxx=yes
                  	  LANGUAGES_TO_SUPPORT+=" c++"
                        fi
                  	;;
                  [no)]
                        list_has_c="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i == "c") { printf "yes"; } } }']`"
                        if test "x$list_has_c" = "xyes" && test "x$enable_c" != "xno" ; then
                  	  [AC_MSG_FAILURE([cannot disable C++ language support because C language support is enabled -- currently both are required to be supported together. If you really don't want C++ language support, please also disable C language support (see ./configure --help)])]
                        else
                          # remove 'C++' from support languages list
                  	  LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/c++//g'`"
                        fi
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-cxx="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,)
AC_ARG_ENABLE([cuda],
               AS_HELP_STRING([--enable-cuda],[Enable Cuda language support in ROSE (default=yes)]),
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "cuda"
                if test $? = 0 ; then 
                  list_has_cuda=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_cuda" != "xyes" ; then
                          # --enable-languages does not include Cuda, but --enable-cuda=yes
                  	  LANGUAGES_TO_SUPPORT+=" cuda"
                        fi
                  	;;
                  [no)]
                        # remove 'Cuda' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/cuda//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-cuda="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,)
AC_ARG_ENABLE([fortran],
               AS_HELP_STRING([--enable-fortran],[Enable Fortran language support in ROSE (default=yes)]),
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "fortran"
                if test $? = 0 ; then 
                  list_has_fortran=yes
                fi
                case "$enableval" in
                  [yes)]
                        if test "x$with_java" = "xno" ; then
                          [AC_MSG_FAILURE([[[Fortran Support]] you specified conflicting configure flags: --enable-fortran="$enableval" enables Fortran-language support, which requires Java, and --with-java="$with_java" disables Java])]
                        fi
                        if test "x$USE_JAVA" = "x0" ; then
                          [AC_MSG_FAILURE([[[Fortran Support]] you requested to build Fortran language support with --enable-fortran="$enableval", which requires Java, but Java was not found. Do you need to explicitly specify your Java using the --with-java configure-switch? (See ./configure --help)])]
                        fi

                  	if test "x$list_has_fortran" != "xyes" ; then
                          # --enable-languages does not include Fortran, but --enable-fortran=yes
                  	  LANGUAGES_TO_SUPPORT+=" fortran"
                        fi
                  	;;
                  [no)]
                        # remove 'Fortran' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/fortran//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-fortran="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,
                if test "x$with_java" = "xno" ; then
		  if test "$enable_fortran" != no; then
                    enable_fortran=no
                    LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/fortran//g'`"
                    [echo "[[Fortran support]] disabling Fortran language support, which requires Java, because you specified --with-java='$with_java'"]
		  fi
                fi)
AC_ARG_ENABLE([opencl],
               AS_HELP_STRING([--enable-opencl],[Enable OpenCL language support in ROSE (default=yes)]),
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "opencl"
                if test $? = 0 ; then 
                  list_has_opencl=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_opencl" != "xyes" ; then
                          # --enable-languages does not include OpenCL, but --enable-opencl=yes
                  	  LANGUAGES_TO_SUPPORT+=" opencl"
                        fi
                  	;;
                  [no)]
                        # remove 'OpenCL' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/opencl//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-opencl="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,)

#
# C and C++ are currently required to be supported simultaneously 
#
list_has_c="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i == "c") { printf "yes"; } } }']`"
list_has_cxx="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i == "c++") { printf "yes"; } } }']`"

if test "x$list_has_c" = "xyes" && test "x$list_has_cxx" != "xyes"; then
  LANGUAGES_TO_SUPPORT+=" c++"
  echo "[[C language support:warning]] turning on C++ support (currently required)"
fi

if test "x$list_has_cxx" = "xyes" && test "x$list_has_c" != "xyes"; then
  LANGUAGES_TO_SUPPORT+=" c"
  echo "[[C++-only support:warning]] turning on C support (currently required)"
fi

#########################################################################################
#
#  Set flags to indicate which languages to support according to the
#  user specified command-line options; including macros that will be
#  contained in BUILD_TREE/rose_config.h 
#
#########################################################################################
if test "x$LANGUAGES_TO_SUPPORT" = "x" ; then
  LANGUAGES_TO_SUPPORT=none
fi
count_of_languages_to_support=0
for a_language in $LANGUAGES_TO_SUPPORT ; do

count_of_languages_to_support=`expr $count_of_languages_to_support + 1`
case "$a_language" in 

none|no)
	support_c_frontend=no
	support_cxx_frontend=no
	support_cuda_frontend=no
	support_fortran_frontend=no
	support_opencl_frontend=no
        enable_tutorial_directory=no
	AC_MSG_WARN([you did not enable any language support])
	;;
c)
	support_c_frontend=yes
	AC_DEFINE([ROSE_BUILD_C_LANGUAGE_SUPPORT], [], [Build ROSE to support the C langauge])
	;;
c++)
	support_cxx_frontend=yes
	AC_DEFINE([ROSE_BUILD_CXX_LANGUAGE_SUPPORT], [], [Build ROSE to support the C++ langauge])
	;;
cuda)
	support_cuda_frontend=yes
	AC_DEFINE([ROSE_BUILD_CUDA_LANGUAGE_SUPPORT], [], [Build ROSE to support the CUDA langauge])
	;;
fortran)
	if test "x$USE_JAVA" = x1; then
	  if test "x$GFORTRAN_PATH" = "x" -o "x$GFORTRAN_PATH" = "xno"; then
            AC_MSG_FAILURE([[[Fortran support]] gfortran not found: required for syntax checking and semantic analysis.
                           Do you need to explicitly specify gfortran using the --with-gfortran=path/to/gfortran configure-switch? (See ./configure --help)])
          else
     	    support_fortran_frontend=yes
	    AC_DEFINE([ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT], [], [Build ROSE to support the Fortran langauge])
          fi
        elif test "x$with_java" = "xno" ; then
	  AC_MSG_FAILURE([[[Fortran support]] cannot support the Fortran language because you specified --with-java="$with_java". You can turn off Fortran support with --disable-fortran (See ./configure --help)]) 
	else
	  AC_MSG_FAILURE([[[Fortran support]] Java Virtual Machine (JVM) not found: required by the Open Fortran Parser (OFP).
	                 Do you need to explicitly specify Java using the --with-java configure-switch? (See ./configure --help)])
	fi
	;;
opencl)
	support_opencl_frontend=yes
	AC_DEFINE([ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT], [], [Build ROSE to support the OpenCL langauge])
        GENERATE_OPENCL_SPECIFIC_HEADERS
	;;
*)
	AC_MSG_FAILURE([unrecognized language "$a_language"])
	;;
esac
done


#
# Check Java version for Fortran front-end support
#
# Versions greater than 1.8 may work but haven't been tested [Rasmussen, 2019.02.27]
#
if test "x$support_fortran_frontend" = "xyes" ; then
    if test "x$JAVA_VERSION_MAJOR" != x1; then
	AC_MSG_FAILURE([Java version for Fortran front-end must be 1.8])
    fi
    if test "x$JAVA_VERSION_MINOR" != x8; then
	AC_MSG_FAILURE([Java version for Fortran front-end must be 1.8 (minor version is not 8)])
    fi
fi


#########################################################################################
#
#  Output language support
#
#########################################################################################
#########################################################################################
#
#  Enabled only one language: set specific configurations for minimal build of ROSE 
#
#########################################################################################
if test $count_of_languages_to_support = 2 ; then
  #
  # Only C/C++ (currently required to be supported simultaneously) 
  #
  if test "x$support_c_frontend" = "xyes" && test "x$support_cxx_frontend" = "xyes" ; then
    enable_tutorial_directory=no
  fi
elif test $count_of_languages_to_support = 1 ; then
  support_only_one_language=yes
  AC_MSG_CHECKING([$LANGUAGES_TO_SUPPORT-only specific configurations])
  echo ""

  #
  # Only Fortran
  # requested by Rice University and LANL 
  #
  if test "x$support_fortran_frontend" = "xyes" ; then
    # Scott appears to require CPPFLAGS to be set...
    #debug#echo "Before setting CPPFLAGS: CPPFLAGS = $CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $JAVA_JVM_INCLUDE"
    echo "[[Fortran-only support]] added JAVA_JVM_INCLUDE ($JAVA_JVM_INCLUDE) to CPPFLAGS ($CPPFLAGS)"

    # Allow tests directory to be run so that we can run the Fortran tests.
    # enable_tests_directory=no
    enable_tutorial_directory=no

    # This allows testing this mechanism to set configure options from within the configure script...
    # enable_edg_version=4.5
  fi

  #
  # Only cuda  
  #
  if test "x$support_cuda_frontend" = "xyes" ; then
    enable_tutorial_directory=no
  fi

  #
  # Only opencl  
  #
  if test "x$support_opencl_frontend" = "xyes" ; then
    enable_tutorial_directory=no
  fi
#AC_MSG_RESULT([done])
#end-if $count_of_languages==1 (enable-only-language)

###
  # Output language-only configuration
  #

  #
  # Tutorial/ directory 
  #
  if test "x$enable_tutorial_directory" = "xyes" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support enabling ROSE/tutorial directory])
  elif test "x$enable_tutorial_directory" = "xno" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support disabling ROSE/tutorial directory])
  fi
fi
#########################################################################################
#
# Set the automake conditional macros that will be used in Makefiles.
#
#########################################################################################
AM_CONDITIONAL(ROSE_BUILD_C_LANGUAGE_SUPPORT, [test "x$support_c_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_CXX_LANGUAGE_SUPPORT, [test "x$support_cxx_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT, [test "x$support_fortran_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_CUDA_LANGUAGE_SUPPORT, [test "x$support_cuda_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT, [test "x$support_opencl_frontend" = xyes])

AC_MSG_CHECKING([if the C frontend is enabled])
if test "x$support_c_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the C++ frontend is enabled])
if test "x$support_cxx_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the Cuda frontend is enabled])
if test "x$support_cuda_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the Fortran frontend is enabled])
if test "x$support_fortran_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the OpenCL frontend is enabled])
if test "x$support_opencl_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

# End macro ROSE_SUPPORT_LANGUAGES.
])
