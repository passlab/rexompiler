include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =
bin_SCRIPTS =
pkgdata_DATA =
MOSTLYCLEANFILES =

SUBDIRS = util ROSETTA frontend midend backend \
          3rdPartyLibraries

# This lists a large number of directories.  If "make install"
# is run then we only need a single directory
AM_CPPFLAGS = -Winvalid-pch $(ROSE_INCLUDES)


# DQ (7/23/2008): New handling of EDG builds a single library instead of two.
# Binary distributions of ROSE store the binary for EDG related code in the source tree.
# JJW (8/3/2008): See src/frontend/CxxFrontend/Makefile.am.forBinaryEDG for
# more information on this process.
# TV (08/2012): changed 'libroseEDGlibraries' to 'libroseCxxlibraries' while adding Clang support to ROSE
if ROSE_USE_CLANG_FRONTEND
   libroseCxxlibraries = frontend/CxxFrontend/Clang/libroseClangFrontend.la
else
   libroseCxxlibraries = frontend/CxxFrontend/EDG/libroseEDG.la
endif

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   libroseOpenFortranParserlibraries = frontend/OpenFortranParser_SAGE_Connection/libroseFortran.la
   pkgdata_DATA += $(top_srcdir)/src/3rdPartyLibraries/fortran-parser/iso_c_binding.rmod
   pkgdata_DATA += $(top_srcdir)/src/3rdPartyLibraries/fortran-parser/openacc.rmod
   pkgdata_DATA += $(top_srcdir)/src/3rdPartyLibraries/fortran-parser/omp_lib.rmod
   pkgdata_DATA += $(top_srcdir)/src/3rdPartyLibraries/fortran-parser/omp_lib_kinds.rmod
else
   libroseOpenFortranParserlibraries =
endif

# TOO (11/22/2011): flattening midend Makefiles => TODO: move conditional compilation to Makefile.
libroseLibraries = \
   $(top_builddir)/src/backend/libbackend.la \
   $(top_builddir)/src/frontend/SageIII/libsage3.la \
   $(libroseCxxlibraries) \
   $(libroseOpenFortranParserlibraries) \
   $(top_builddir)/src/midend/programAnalysis/libprogramAnalysis.la \
   $(top_builddir)/src/midend/programTransformation/loopProcessing/libLoopProcessing.la \
   $(top_builddir)/src/midend/programTransformation/extractFunctionArgumentsNormalization/libExtractFunctionArgumentsNormalization.la \
   $(top_builddir)/src/midend/programTransformation/singleStatementToBlockNormalization/libSingleStatementToBlockNormalization.la \
   $(top_builddir)/src/midend/libmidend.la \
   $(top_builddir)/src/util/libroseutil.la

if ROSE_WITH_LIBHARU
libroseLibraries += \
   $(top_builddir)/src/3rdPartyLibraries/libharu-2.1.0/src/libhpdf.la
endif

EXTRA_DIST = CMakeLists.txt extra.docs

# Header files that need to be included in the distribution
pkginclude_HEADERS = \
     featureTests.h \
     rose.h \
     roseInternal.h \
     rose_msvc.h \
     msvc_stdint.h \
     frontend/SageIII/sage3basic.h \
     frontend/SageIII/sage3basic.hhh \
     midend/astDiagnostics/AstConsistencyTests.h

nobase_pkginclude_HEADERS =

########################################################################################################################

# DQ (9/22/2006): Because we now use libtool, the shared library is a reference to the separate
# dynamic libraries in each directory and so testMultiRoseLib is redundant with testSharedRoseLib.
# This is true only if we have configured --with-development-libraries, else we have static
# in each directory and we will link with the collection of all static libraries.

all-local:

# PC (9/15/2006): force the use of the C++ linker
LINK = $(CXXLINK)

lib_LTLIBRARIES = librose.la

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_USE_INTERNAL_JAVA_SUPPORT
   JAVA_JVM_FLAGS = $(JAVA_JVM_LINK)

# OpenFortranParser-${OFP_VERSION}.jar:
#	cd ${top_builddir}/src/3rdPartyLibraries/fortran-parser; $(MAKE) get_jar_file
endif

librose_la_SOURCES = dummyCppFileForLibrose.C

librose_la_LIBADD = $(libroseLibraries) $(JAVA_JVM_FLAGS) -lm $(RT_LIBS)

if ROSE_USE_GCC_OMP
   librose_la_LIBADD += lgomp
endif

# librose_la_LDFLAGS = -no-undefined --rpath $(INTEL_PIN_PATH)/intel64/lib-ext
librose_la_LDFLAGS = -no-undefined $(ROSE_RPATHS)

# DQ (4/12/2016): This is required for use with clang (not clear how to compile ROSE with out it).
if USING_CLANG_COMPILER
librose_la_LIBADD += -lstdc++
endif
librose_la_LDFLAGS += -fno-rtti
librose_la_DEPENDENCIES = $(libroseLibraries)

if OS_MACOSX
  librose_la_LDFLAGS += $(ROSE_LINK_RPATHS)
endif

# Build list of all required libraries so that they can be built in parallel
REQUIRED_LIBS = librose.la

########################################################################################################################
# This tool spits out information that tells users how to compile and link programs that use ROSE. Therefore, we have to
# build a configuration file and compile its installed name into the rose-config tool. It's also especially important
# that this tool uses rpaths to find its dynamically-linked libraries, otherwise the user will be tempted to use
# LD_LIBRARY_PATH and complain when the tool segfaults, not realizing that the fault is because they told the tool to
# use a library that is incompatible with the one it was compiled to use.
bin_PROGRAMS += rose-config
rose_config_SOURCES = rose-config.C
rose_config_LDFLAGS = $(ROSE_RPATHS)
rose_config_DEPENDENCIES = $(REQUIRED_LIBS)
rose_config_CPPFLAGS = -DLIBDIR='"$(libdir)"'
rose_config_LDADD = $(ROSE_LIBS)

if OS_MACOSX
  rose_config_LDFLAGS += $(ROSE_LINK_RPATHS)
endif

# DQ (4/12/2016): This is required for use with clang (not clear how to compile ROSE with out it).
if USING_CLANG_COMPILER
rose_config_LDADD += -lstdc++
endif
rose_config_LDFLAGS += -fno-rtti

rose_config_installed_cppflags =		\
	-I@includedir@/rose			\
	$(ROSE_PCH_INCLUDE)

rose_config_installed_ldflags =			\
	-L@libdir@ -lrose			\
	$(JAVA_JVM_LINK)			\
	$(RT_LIBS)				\
	$(ROSE_INTEL_COMPILER_MATH_LIBS)	\
	-lm
if ROSE_USE_EDG_QUAD_FLOAT
  rose_config_installed_ldflags += -lquadmath
endif
rose_config_cfgdir = $(libdir)
rose_config_cfg_DATA = rose-config.cfg

# Rasmussen (3/23/2018): Added ROSE_RPATHS and ROSE_LINK_RPATHS to enable
# linking on Mac OSX from a ROSE installations.
rose-config.cfg:
	$(AM_V_GEN) (								\
		echo "# Things that the 'rose-config' tool can report";		\
		echo "ROSE_CC       = $(CC)";					\
		echo "ROSE_CXX      = $(CXX)";					\
		echo "ROSE_CPPFLAGS = $(rose_config_installed_cppflags)";	\
		echo "ROSE_CFLAGS   = $(CFLAGS)";				\
		echo "ROSE_CXXFLAGS = $(CXXFLAGS)";				\
		echo "ROSE_LDFLAGS  = $(rose_config_installed_ldflags)";	\
		echo "ROSE_LIBDIRS  = $(filter-out -R, $(ROSE_RPATHS))";	\
		echo "ROSE_PREFIX   = $(prefix)";				\
		echo "# Additional items useful to include in Makefiles";	\
		echo "ROSE_RPATHS       = $(ROSE_RPATHS)";			\
		echo "ROSE_LINK_RPATHS  = $(ROSE_LINK_RPATHS)";			\
	) >$@

MOSTLYCLEANFILES += rose-config.cfg

bin_PROGRAMS += rose-compiler
bin_PROGRAMS += rose-cc rose-c++
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  bin_PROGRAMS += rose-fortran
endif
# The -std= option in ROSE only support c, c++, fortran

LDADD_ROSEBIN        = $(ROSE_LIBS)
LDFLAGS_ROSEBIN      = $(ROSE_RPATHS)
DEPENDENCIES_ROSEBIN = $(REQUIRED_LIBS)
if USING_CLANG_COMPILER
  LDADD_ROSEBIN   += -lstdc++
endif
LDFLAGS_ROSEBIN += -fno-rtti

if ROSE_USE_INTERNAL_JAVA_SUPPORT
if OS_MACOSX
  LDFLAGS_ROSEBIN += $(ROSE_LINK_RPATHS)
endif
endif

rose_compiler_SOURCES      = rose-compiler.C
rose_compiler_LDFLAGS      = $(LDFLAGS_ROSEBIN)
rose_compiler_DEPENDENCIES = $(REQUIRED_LIBS)
rose_compiler_CPPFLAGS     =
rose_compiler_LDADD        = $(LDADD_ROSEBIN)

rose_cc_SOURCES      = rose-compiler.C
rose_cc_LDFLAGS      = $(LDFLAGS_ROSEBIN)
rose_cc_DEPENDENCIES = $(REQUIRED_LIBS)
rose_cc_CPPFLAGS     = -DROSE_COMPILER_FOR_LANGUAGE="\"c\""
rose_cc_LDADD        = $(LDADD_ROSEBIN)

rose_c___SOURCES      = rose-compiler.C
rose_c___LDFLAGS      = $(LDFLAGS_ROSEBIN)
rose_c___DEPENDENCIES = $(REQUIRED_LIBS)
rose_c___CPPFLAGS     = -DROSE_COMPILER_FOR_LANGUAGE="\"c++\""
rose_c___LDADD        = $(LDADD_ROSEBIN)

rose_fortran_SOURCES      = rose-compiler.C
rose_fortran_LDFLAGS      = $(LDFLAGS_ROSEBIN)
rose_fortran_DEPENDENCIES = $(REQUIRED_LIBS)
rose_fortran_CPPFLAGS     = -DROSE_COMPILER_FOR_LANGUAGE="\"fortran\""
rose_fortran_LDADD        = $(LDADD_ROSEBIN)

########################################################################################################################
# Test that a simple ROSE program can run, and as a side effect, print some version numbers.

check-dynload:
	@echo "  TEST     whether a trivial ROSE program runs [rose-compiler]"
	${AM_V_at}if ! ./rose-compiler --version; then                                           \
	  echo "error: You cannot run a program linked with ROSE. Please be sure that your" >&2; \
	  echo "error: LD_LIBRARY_PATH (Linux) or DYLD_LIBRARY_PATH (Mac OSX) contains the" >&2; \
	  echo "error: correct libraries (JVM, Boost, librose.so, etc.)" >&2;                    \
	  exit 1;                                                                                \
	fi

check-config:
	@echo "  TEST     ROSE configuration tool [rose-config]"
	./rose-config --version

if OS_MACOSX
check-hello-c: $(eval SHELL:=/bin/bash)
	@echo "  TEST     ROSE C compiler [rose-cc]"
	${AM_V_at}echo "#include <stdio.h>"                     >  hello-world.c
	${AM_V_at}echo "int main() {"                           >> hello-world.c
	${AM_V_at}echo "   printf(\"Hello World from C\\n\");"  >> hello-world.c
	${AM_V_at}echo "   return 0;"                           >> hello-world.c
	${AM_V_at}echo "}"                                      >> hello-world.c
	${AM_V_at}./rose-cc hello-world.c -o hello-world-c
	${AM_V_at}./hello-world-c
else
check-hello-c:
	@echo "  TEST     ROSE C compiler [rose-cc]"
	${AM_V_at}echo "#include <stdio.h>"                     >  hello-world.c
	${AM_V_at}echo "int main() {"                           >> hello-world.c
	${AM_V_at}echo "   printf(\"Hello World from C\\n\");"  >> hello-world.c
	${AM_V_at}echo "   return 0;"                           >> hello-world.c
	${AM_V_at}echo "}"                                      >> hello-world.c
	${AM_V_at}./rose-cc hello-world.c -o hello-world-c
	${AM_V_at}./hello-world-c
endif
check-hello-cxx:
	@echo "  TEST     ROSE C++ compiler [rose-c++]"
	${AM_V_at}echo "#include <iostream>"                                      >  hello-world.cpp
	${AM_V_at}echo "int main() {"                                             >> hello-world.cpp
	${AM_V_at}echo "    std::cout << \"Hello World from C++!\" << std::endl;" >> hello-world.cpp
	${AM_V_at}echo "    return 0;"                                            >> hello-world.cpp
	${AM_V_at}echo "}"                                                        >> hello-world.cpp
	${AM_V_at}./rose-c++ hello-world.cpp -o hello-world-cpp
	${AM_V_at}./hello-world-cpp
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
check-hello-fortran:
	@echo "  TEST     ROSE Fortran compiler [rose-fortran]"
	${AM_V_at}echo "program hello"                            >  hello-world.f90
	${AM_V_at}echo "  print *, \"Hello World from Fortran!\"" >> hello-world.f90
	${AM_V_at}echo "end program hello"                        >> hello-world.f90
	${AM_V_at}./rose-fortran hello-world.f90 -o hello-world-f90
	${AM_V_at}./hello-world-f90
endif

check-local: $(bin_PROGRAMS)
	${AM_V_at}$(MAKE) check-dynload
	${AM_V_at}$(MAKE) check-config
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	${AM_V_at}$(MAKE) check-hello-c
endif
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
	${AM_V_at}$(MAKE) check-hello-cxx
endif
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
	${AM_V_at}$(MAKE) check-hello-fortran
endif

########################################################################################################################

# Rasmussen (3/23/2018): Added installing libtool to the bin directory to support
# linking an application using ROSE from a ROSE installations.
bin_SCRIPTS += ../libtool


# DQ (1/10/2004): remove the librose.a and libedg.a files so that distclean will work cleanly
# Do not remove libraries indiscriminately with wildcards [Matzke, 2017-06-19]
clean-local:
	rm -rf librose.a librose.so testSharedRoseLib testMultiRoseLib $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -rf $(MOSTLYCLEANFILES)

distclean-local:
	rm -rf librose.a librose.so testSharedRoseLib testMultiRoseLib Templates.DB

cleanTest:
	rm -rf *.lo testSharedRoseLib testMultiRoseLib

# DQ (9/14/2006): These are required!
# Note that uninstall-hook does not work, but uninstall-local does.
uninstall-local:
	rm -f $(DESTDIR)$(libdir)/libedg.so
	rm -f $(DESTDIR)$(libdir)/librose.so
	@echo "************************************************************"
	@echo "**** make uninstall rule complete (terminated normally) ****"
	@echo "************************************************************"

rebuildPrelinker:
	rm -f frontend/CxxFrontend/EDG/EDG_3.3/util/edg_prelink.lo
	rm -f frontend/SageIII/Cxx_Grammar.lo
	$(MAKE)

