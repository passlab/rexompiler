include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastDump.la

# DQ (2/26/2009): Added copyGraph.[hC], also note that copyGraph.C has
# a dependence on a TCLSH script that is located in the scripts directory.
# libastDump_la_SOURCES = astGraph.C AstAttributeDOT.C wholeAST.C copyGraph.C walrus.C intermediateRepresentationNodeGraphs.C
libastDump_la_SOURCES = astGraph.C AstAttributeDOT.C wholeAST.C copyGraph.C walrus.C intermediateRepresentationNodeGraphs.C includeGraph.C

#   colorFilesTraversal.C  colorMemoryPoolTraversal.C \
#   customAstDOTGeneration.C  customAstDOTGenerationData.C \
#   customMemoryPoolDOTGeneration.C  customMemoryPoolDOTGenerationData.C

libastDump_la_LIBADD       =
libastDump_la_DEPENDENCIES =

pkginclude_HEADERS = AstAttributeDOT.h astGraph.h astGraphTemplateImpl.h wholeAST.h wholeAST_API.h copyGraph.h walrus.h intermediateRepresentationNodeGraphs.h includeGraph.h

EXTRA_DIST = CMakeLists.txt
