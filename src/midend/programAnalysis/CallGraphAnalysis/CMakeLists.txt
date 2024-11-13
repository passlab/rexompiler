
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  CallGraph.h ClassHierarchyGraph.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#
#
#INCLUDES = $(ROSE_INCLUDES)
#libCallGraphSources =  CallGraph.C ClassHierarchyGraph.C
#
#noinst_LTLIBRARIES = libCallGraph.la
#libCallGraph_la_SOURCES = $(libCallGraphSources)
#
#clean-local:
#	rm -rf ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS = CallGraph.h ClassHierarchyGraph.h
#
#
