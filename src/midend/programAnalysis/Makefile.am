# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# AUTOMAKE_OPTIONS = subdir-objects

# SUBDIRS +=

###############################################################################
# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles
#
# Caution: include path cannot contain variable,
#>..e.g. include $(cxxCodeGenerationPath)/Makefile_variables
#
include $(srcdir)/CallGraphAnalysis/Makefile_variables
include $(srcdir)/CFG/Makefile_variables
include $(srcdir)/bitvectorDataflow/Makefile_variables
include $(srcdir)/VirtualFunctionAnalysis/Makefile_variables
include $(srcdir)/defUseAnalysis/Makefile_variables
include $(srcdir)/dominanceAnalysis/Makefile_variables
include $(srcdir)/genericDataflow/Makefile_variables
include $(srcdir)/OAWrap/Makefile_variables
include $(srcdir)/pointerAnal/Makefile_variables
include $(srcdir)/staticInterproceduralSlicing/Makefile_variables
include $(srcdir)/valuePropagation/Makefile_variables
include $(srcdir)/variableRenaming/Makefile_variables

mpaCallGraphAnalysisPath=$(srcdir)/CallGraphAnalysis#
mpaCFGPath=$(srcdir)/CFG#
mpaBitvectorDataflowPath=$(srcdir)/bitvectorDataflow#
mpaVirtualFunctionAnalysisPath=$(srcdir)/VirtualFunctionAnalysis#
mpaDefUseAnalysisPath=$(srcdir)/defUseAnalysis#
mpaDominanceAnalysisPath=$(srcdir)/dominanceAnalysis#
mpaGenericDataflowPath=$(srcdir)/genericDataflow#
mpaOAWrapPath=$(srcdir)/OAWrap#
mpaPointerAnalPath=$(srcdir)/pointerAnal#
mpaStaticInterproceduralSlicingPath=$(srcdir)/staticInterproceduralSlicing#
mpaValuePropagationPath=$(srcdir)/valuePropagation#
mpaVariableRenamingPath=$(srcdir)/variableRenaming#

###############################################################################

# Taken from midend/programAnalysis/annotationLanguageParser
AM_CXXFLAGS+=\
	-D__LOCALSTL_H


AM_CPPFLAGS_OMP=\
	$(mpaDefUseAnalysis_includesOmp) \
	$(mpaVariableRenaming_includesOmp)


AM_CPPFLAGS=\
	$(ROSE_INCLUDES) \
	$(mpaCallGraphAnalysis_includes) \
	$(mpaCFG_includes) \
	$(mpaBitvectorDataflow_includes) \
	$(mpaVirtualFunctionAnalysis_includes) \
	$(mpaDefUseAnalysis_includes) \
	$(mpaGenericDataflow_includes) \
	$(mpaOAWrap_includes) \
	$(mpaPointerAnal_includes) \
	$(mpaStaticInterproceduralSlicing_includes) \
	$(mpaValuePropagation_includes) \
	$(mpaVariableRenaming_includes) \
	$(AM_CPPFLAGS_OMP)

BUILT_SOURCES=

noinst_LTLIBRARIES=\
	libprogramAnalysis.la

##################################################################

libprogramAnalysis_la_SOURCES=\
	$(mpaCallGraphAnalysis_la_sources) \
	$(mpaCFG_la_sources) \
	$(mpaBitvectorDataflow_la_sources) \
	$(mpaVirtualFunctionAnalysis_la_sources) \
	$(mpaDefUseAnalysis_la_sources) \
	$(mpaGenericDataflow_la_sources) \
	$(mpaOAWrap_la_sources) \
	$(mpaPointerAnal_la_sources) \
	$(mpaStaticInterproceduralSlicing_la_sources) \
	$(mpaValuePropagation_la_sources) \
	$(mpaVariableRenaming_la_sources)

libprogramAnalysis_la_LIBADD = 

pkginclude_HEADERS=\
	$(mpaCallGraphAnalysis_includeHeaders) \
	$(mpaCFG_includeHeaders) \
	$(mpaBitvectorDataflow_includeHeaders) \
	$(mpaVirtualFunctionAnalysis_includeHeaders) \
	$(mpaDefUseAnalysis_includeHeaders) \
	$(mpaDominanceAnalysis_includeHeaders) \
	$(mpaGenericDataflow_includeHeaders) \
	$(mpaOAWrap_includeHeaders) \
	$(mpaPointerAnal_includeHeaders) \
	$(mpaStaticInterproceduralSlicing_includeHeaders) \
	$(mpaValuePropagation_includeHeaders) \
	$(mpaVariableRenaming_includeHeaders)


noinst_HEADERS=

EXTRA_DIST=\
	CMakeLists.txt \
	OpenAnalysis \
	PolicyControl.txt \
	$(mpaCallGraphAnalysis_extraDist) \
	$(mpaCFG_extraDist) \
	$(mpaBitvectorDataflow_extraDist) \
	$(mpaVirtualFunctionAnalysis_extraDist) \
	$(mpaDefUseAnalysis_extraDist) \
	$(mpaDominanceAnalysis_extraDist) \
	$(mpaGenericDataflow_extraDist) \
	$(mpaOAWrap_extraDist) \
	$(mpaPointerAnal_extraDist) \
	$(mpaStaticInterproceduralSlicing_extraDist) \
	$(mpaValuepropagation_extraDist) \
	$(mpaVariableRenaming_extraDist)


clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
	$(mpaCallGraphAnalysis_cleanLocal)
	$(mpaCFG_cleanLocal)
	$(mpaBitvectorDataflow_cleanLocal)
	$(mpaVirtualFunctionAnalysis_cleanLocal)
	$(mpaDefUseAnalysis_cleanLocal)
	$(mpaDominanceAnalysis_cleanLocal)
	$(mpaGenericDataflow_cleanLocal)
	$(mpaOAWrap_cleanLocal)
	$(mpaPointerAnal_cleanLocal)
	$(mpaStaticInterproceduralSlicing_cleanLocal)
	$(mpaValuePropagation_cleanLocal)
	$(mpaVariableRenaming_cleanLocal)


distclean-local:
#	rm -rf Templates.DB
	$(mpaCallGraphAnalysis_distCleanLocal)
	$(mpaCFG_distCleanLocal)
	$(mpaBitvectorDataflow_distCleanLocal)
	$(mpaVirtualFunctionAnalysis_distCleanLocal)
	$(mpaDefUseAnalysis_distCleanLocal)
	$(mpaDominanceAnalysis_distCleanLocal)
	$(mpaGenericDataflow_distCleanLocal)
	$(mpaOAWrap_distCleanLocal)
	$(mpaPointerAnal_distCleanLocal)
	$(mpaStaticInterproceduralSlicing_distCleanLocal)
	$(mpaValuePropagation_distCleanLocal)
	$(mpaVariableRenaming_distCleanLocal)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .
