# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Subdirectories
# ------------------------------------------------------------------------------

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

include $(srcdir)/abstractLayer/Makefile_variables
include $(srcdir)/astDiagnostics/Makefile_variables
include $(srcdir)/astProcessing/Makefile_variables
include $(srcdir)/astQuery/Makefile_variables
include $(srcdir)/astUtil/Makefile_variables
include $(srcdir)/astUtil/annotation/Makefile_variables
include $(srcdir)/astUtil/astInterface/Makefile_variables
include $(srcdir)/astUtil/astSupport/Makefile_variables
include $(srcdir)/astUtil/symbolicVal/Makefile_variables
include $(srcdir)/programTransformation/Makefile_variables
include $(srcdir)/programTransformation/partialRedundancyElimination/Makefile_variables
include $(srcdir)/programTransformation/finiteDifferencing/Makefile_variables
include $(srcdir)/programTransformation/functionCallNormalization/Makefile_variables
include $(srcdir)/programTransformation/constantFolding/Makefile_variables
include $(srcdir)/programTransformation/implicitCodeGeneration/Makefile_variables
include $(srcdir)/programTransformation/astInlining/Makefile_variables
include $(srcdir)/programTransformation/astOutlining/Makefile_variables
include $(srcdir)/programTransformation/ompLowering/Makefile_variables
include $(srcdir)/programTransformation/transformationTracking/Makefile_variables

#
# Don't use $(srcdir) in conjunction with [subdir-objects], otherwise
# object files will be built in the source tree. Therefore, we need to
# use relative paths.
#
#mptPartialRedundancyEliminationPath=$(srcdir)/partialRedundancyElimination#
mAbstractLayerPath=abstractLayer#
mAstDiagnosticsPath=astDiagnostics#
mAstProcessingPath=astProcessing#
mAstQueryPath=astQuery#
mAstUtilPath=astUtil#
mProgramTransformationPath=programTransformation#

SUBDIRS = astDump \
	programTransformation/extractFunctionArgumentsNormalization \
	programTransformation/singleStatementToBlockNormalization

# DQ (12/2/2016): These directories have poorly formated Makefile.am files have
# and are a source of portability problem on Ubuntu systems (because they overload 
# the CXXFLAGS makefile variable).

SUBDIRS += \
	programAnalysis \
	programTransformation/loopProcessing

# ------------------------------------------------------------------------------
#  Library: programTransformation.la
# ------------------------------------------------------------------------------

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

AM_CPPFLAGS=\
	$(ROSE_INCLUDES) \
	-I${top_srcdir}/src/midend/programAnalysis/CFG \
	-I$(mAstUtilPath) \
	-I$(mAstUtilPath)/astInterface \
	-I$(mAstUtilPath)/symbolicVal \
	-I$(mAstUtilPath)/astSupport \
	-I$(top_srcdir)/src/util \
	-I$(top_srcdir)/src/util/support

noinst_LTLIBRARIES=\
	libmidend.la

lib_LTLIBRARIES=\
	$(mProgramTransformation_lib_ltlibraries)

libmidend_la_SOURCES=\
	$(mAbstractLayer_la_sources) \
	$(mAstDiagnostics_la_sources) \
	$(mAstProcessing_la_sources) \
	$(mAstQuery_la_sources) \
	$(mAstSnippet_la_sources) \
	$(mAstUtil_la_sources) \
	$(mProgramTransformation_la_sources)

libmidend_la_DEPENDENCIES=\
	$(mAstQuery_la_dependencies)

libmidend_la_LIBADD=\
	astDump/libastDump.la

pkginclude_HEADERS = \
	$(mAbstractLayer_includeHeaders) \
	$(mAstDiagnostics_includeHeaders) \
	$(mAstProcessing_includeHeaders) \
	$(mAstQuery_includeHeaders) \
	$(mAstSnippet_includeHeaders) \
	$(mAstUtil_includeHeaders) \
	$(mProgramTransformation_includeHeaders)

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

EXTRA_DIST = \
	CMakeLists.txt \
	$(mAbstractLayer_extraDist) \
	$(mAstDiagnostics_extraDist) \
	$(mAstProcessing_extraDist) \
	$(mAstQuery_extraDist) \
	$(mAstUtil_extraDist) \
	$(mProgramTransformation_extraDist)

clean-local:
	$(mAbstractLayer_cleanLocal)
	$(mAstDiagnostics_cleanLocal)
	$(mAstProcessing_cleanLocal)
	$(mAstQuery_cleanLocal)
	$(mAstUtil_cleanLocal)
	$(mProgramTransformation_cleanLocal)

distclean-local:
	$(mAstProcessing_distCleanLocal)
	$(mAstQuery_distCleanLocal)
	$(mAstUtil_distCleanLocal)
