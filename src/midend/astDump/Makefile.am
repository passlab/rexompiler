include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastDump.la

# DQ (2/26/2009): Added copyGraph.[hC], also note that copyGraph.C has
# a dependence on a TCLSH script that is located in the scripts directory.
# libastDump_la_SOURCES = astGraph.C AstAttributeDOT.C wholeAST.C copyGraph.C walrus.C intermediateRepresentationNodeGraphs.C
libastDump_la_SOURCES = astGraph.C AstAttributeDOT.C wholeAST.C copyGraph.C \
                        walrus.C intermediateRepresentationNodeGraphs.C includeGraph.C \
                        AstDOTGeneration.C DOTGraphInterface.C \
                        AstTerm.C

if ROSE_WITH_LIBHARU
libastDump_la_SOURCES += AstPDFGeneration.C PDFGeneration.C
endif

#   colorFilesTraversal.C  colorMemoryPoolTraversal.C \
#   customAstDOTGeneration.C  customAstDOTGenerationData.C \
#   customMemoryPoolDOTGeneration.C  customMemoryPoolDOTGenerationData.C

libastDump_la_LIBADD       =
libastDump_la_DEPENDENCIES =

pkginclude_HEADERS = AstAttributeDOT.h astGraph.h astGraphTemplateImpl.h \
                     wholeAST.h wholeAST_API.h copyGraph.h walrus.h \
                     intermediateRepresentationNodeGraphs.h includeGraph.h \
                     AstDOTGeneration.h \
                     DOTGeneration.h DOTRepresentation.h \
                     DOTSubgraphRepresentation.h DOTGraphInterface.h \
                     DOTRepresentationImpl.h DOTGenerationImpl.h \
                     DOTSubgraphRepresentationImpl.h AstTerm.h

if ROSE_WITH_LIBHARU
pkginclude_HEADERS += AstPDFGeneration.h PDFGeneration.h
endif

EXTRA_DIST = CMakeLists.txt MSTL.docs
