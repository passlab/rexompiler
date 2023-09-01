# Build the ROSE-based tools
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = globalVariablesInLambdas classMemberVariablesInLambdas checkFortranInterfaces featureVector

EXTRA_DIST = 

# install the tools in 'bin'
bin_SCRIPTS = 

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

EXTRA_DIST += tests

#-------------- only for C/C++
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT

bin_PROGRAMS = moveDeclarationToInnermostScope

moveDeclarationToInnermostScope_SOURCES = moveDeclarationToInnermostScope.C

clean-local:
	rm -rf rose_*.C rose_*.cpp rose*.cxx *.diff *.output *.txt *.o *.log
endif	
