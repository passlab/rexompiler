# This should not depend on ROSE in any way so I don't think we need this
# include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

# MS (1/25/21): include json-related code
SUBDIRS += json

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   SUBDIRS += fortran-parser
endif

SUBDIRS += antlr-jars
if ROSE_WITH_LIBHARU
SUBDIRS += libharu-2.1.0 
endif

EXTRA_DIST = CMakeLists.txt
