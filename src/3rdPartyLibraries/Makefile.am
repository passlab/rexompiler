# This should not depend on ROSE in any way so I don't think we need this
# include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# SUBDIRS = MSTL libharu-2.1.0 fortran-parser java-parser antlr-jars qrose
# checkPointLibrary

SUBDIRS =

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   SUBDIRS += fortran-parser
endif

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
SUBDIRS += MSTL
else
SUBDIRS += MSTL antlr-jars
if ROSE_WITH_LIBHARU
SUBDIRS += libharu-2.1.0 
endif
endif

EXTRA_DIST = CMakeLists.txt
