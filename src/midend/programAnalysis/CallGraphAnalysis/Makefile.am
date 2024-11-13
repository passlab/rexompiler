include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs



AM_CPPFLAGS = $(ROSE_INCLUDES)
libCallGraphSources =  newCallGraph.C CallGraph.C ClassHierarchyGraph.C

noinst_LTLIBRARIES = libCallGraph.la
libCallGraph_la_SOURCES = $(libCallGraphSources)

clean-local:
#	rm -rf ./ii_files ./ti_files ./cxx_templates

distclean-local:
#	rm -rf ./Templates.DB

pkginclude_HEADERS = newCallGraph.h CallGraph.h ClassHierarchyGraph.h



EXTRA_DIST = CMakeLists.txt
