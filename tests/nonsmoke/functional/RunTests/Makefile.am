include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
SUBDIRS = . AstDeleteTests
else
SUBDIRS =
endif

AM_CPPFLAGS =  \
        $(ROSE_INCLUDES) 
#	        -I$(top_srcdir)/src/frontend/SageIII/sageInterface
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD =  $(ROSE_LIBS)

noinst_PROGRAMS = \
  traverseCommonBlock

traverseCommonBlock_SOURCES = traverseCommonBlock.C  

generatedCodeExamples = 

# turn off edg warnings such as "variable x declared but never referenced" for test input files
TEST_CXXFLAGS = -rose:verbose 0 --edg:no_warnings -g3

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
SUBDIRS += FortranTests

generatedCodeExamples += \
	  rose_inputtraverseCommonBlock.f

rose_inputtraverseCommonBlock.f:traverseCommonBlock
	./traverseCommonBlock -c $(TEST_CXXFLAGS) $(srcdir)/inputtraverseCommonBlock.f
endif	  

EXTRA_DIST = CMakeLists.txt inputtraverseCommonBlock.f
check-local:
	$(MAKE) $(generatedCodeExamples)
clean-local:
	rm -f *.o test*.C rose_*.c rose_*.C rose_*.f rose_*.cpp *.C.pdf *.c.pdf testfile525.cpp testfile626.cpp rose_*.upc abiStuffTestDone *.dot rose_*
