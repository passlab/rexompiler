include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

#
# Don't use $(srcdir) in conjunction with [subdir-objects], otherwise
# object files will be built in the source tree. Therefore, we need to
# use relative paths.
#
#mptPartialRedundancyEliminationPath=$(srcdir)/partialRedundancyElimination#
fSageSupportPath=sage_support#
include $(srcdir)/sage_support/Makefile_variables
# license_string.h is included by sage_support/cmdline.cpp
license_string.h: $(top_srcdir)/LicenseInformation/ROSE_BSD_License.txt
	perl $(top_srcdir)/scripts/to-c-string.pl < $< > $@

# if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
#   SUBDIRS = astFixup astPostProcessing sageInterface virtualCFG includeDirectivesProcessing
#else
   SUBDIRS = astFixup astPostProcessing sageInterface virtualCFG astTokenStream astHiddenTypeAndDeclarationLists includeDirectivesProcessing ompparser accparser
#endif

# DQ (12/29/2009): This is only used to handle generated code as smaller files.
SUBDIRS += GENERATED_CODE_DIRECTORY_Cxx_Grammar

# DQ (5/2/2009): This supports the optimization of the new 
# Graph IR nodes (makes about 25% improvement, I think).
# ROSE_OPTIMIZATION_LEVEL = -O3
ROSE_OPTIMIZATION_LEVEL =
# ROSE_VERBOSE = -rose:verbose 2

CXXFLAGS += $(ROSE_OPTIMIZATION_LEVEL) $(ROSE_VERBOSE)

omp_exprparser_lexer.lo: omp_exprparser_parser.hh omp_exprparser_lexer.cc
omp_exprparser_parser.hh: omp_exprparser_parser.cc

# Force sage.docs to be make by default
# (if sage.docs.in is modified)
all-local: roseh sage.docs 

# AM_CPPFLAGS = $(ROSE_INCLUDES)
# AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src $(WAVE_INCLUDES)
# AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src
# JJW 6-2-2008: We shouldn't be using anything from EDG outside these two directories
# AM_CPPFLAGS = $(ROSE_INCLUDES) # -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/SageIII/ompparser/src
AM_CPPFLAGS += -I$(top_srcdir)/src/frontend/SageIII/accparser/src

# DQ (3/13/2010): sage_support.cpp requires the OFP fortran source files to be made available.
# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   AM_CPPFLAGS += -I${top_builddir}/src/3rdPartyLibraries/fortran-parser
endif

###Rasmussen:FIXME************************************
   AM_CPPFLAGS += -I$(top_srcdir)/src/frontend/Experimental_General_Language_Support

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

# Copy the original lex file and the C++ file generated from it to the distribution
# so that users don't have to find an appropriate version of lex (flex).  Flex should
# be used by developers so that ANSI C code is generated for g++.  This is something
# for developers to worry about in the generation of sufficently portable distributions.
# EXTRA_DIST = preproc.lex preproc.C sageClasses.docs sage.docs.in docs
# EXTRA_DIST = preproc.lex preproc.C astGraphTemplate.C docs
# EXTRA_DIST = preproc.ll docs rtiHelpers.h attributeListMap.h
#
EXTRA_DIST = CMakeLists.txt preproc-c.ll docs rtiHelpers.h attributeListMap.h \
             advanced_preprocessing_hooks.h rose_attributes_list.h \
             sage3basic.hhh fileoffsetbits.h rosedll.h PolicyControl.txt Utf8.h  \
             ompAstConstruction.h accAstConstruction.h \
             omp_exprparser_lexer.cc omp_exprparser_parser.cc omp_exprparser_parser.hh
CLEANFILES = haveRosettaGeneratedSource

DISTCLEANFILES = preproc-c.cc omp_exprparser_lexer.cc omp_exprparser_parser.hh omp_exprparser_parser.cc license_string.h

# Some source code is generated (call make in ROSE/src/ROSETTA/src to generate these files)
ROSETTA_GENERATED_SOURCE = \
	Cxx_Grammar.h \
	Cxx_Grammar.C \
	Cxx_GrammarRTI.C \
	AstQueryMemoryPool.h \
	Cxx_GrammarReturnClassHierarchySubTree.C \
	Cxx_GrammarTreeTraversalSuccessorContainer.C \
	Cxx_GrammarTreeTraversalAccessEnums.h \
	Cxx_GrammarVariantEnumNames.C \
	Cxx_GrammarNewAndDeleteOperators.C \
	Cxx_GrammarCopyMemberFunctions.C \
	Cxx_GrammarTraverseMemoryPool.C \
	Cxx_GrammarCheckingIfDataMembersAreInMemoryPool.C \
	Cxx_GrammarReturnDataMemberPointers.C \
	Cxx_GrammarProcessDataMemberReferenceToPointers.C \
	Cxx_GrammarNewConstructors.C \
	Cxx_GrammarGetChildIndex.C

haveRosettaGeneratedSource:
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)
	touch haveRosettaGeneratedSource

force-haveRosettaGeneratedSource:
	-rm Cxx_Grammar.h
	$(MAKE) haveRosettaGeneratedSource

$(ROSETTA_GENERATED_SOURCE): haveRosettaGeneratedSource

libsage3Sources = \
   Utf8.C \
   rose_attributes_list.C \
   attachPreprocessingInfo.C \
   attachPreprocessingInfoTraversal.C \
   attributeListMap.C \
   manglingSupport.C \
   fixupCopy_scopes.C \
   fixupCopy_symbols.C \
   fixupCopy_references.C \
   IncludeDirective.C \
   rtiHelpers.C \
   rose_graph_support.C \
   $(fSageSupport_la_sources) \
   ompSupport.C \
   ompFortranParser.C \
   ompAstConstruction.cpp \
   accAstConstruction.cpp \
   memory-pool-snapshot.C

noinst_LTLIBRARIES = libsage3.la

libsage3_la_SOURCES = $(libsage3Sources) preproc-c.ll omp_exprparser_lexer.ll omp_exprparser_parser.yy

# DQ (3/8/2009): I think that the dependence upon $(top_builddir)/rose_config.h 
# is what was forcing a lot of files to be compiled after reruning configure.
# nodist_libsage3_la_SOURCES = $(ROSETTA_GENERATED_SOURCE) $(top_builddir)/rose_config.h
nodist_libsage3_la_SOURCES = $(ROSETTA_GENERATED_SOURCE) license_string.h

#if ROSE_USE_BOOST_WAVE
libsage3_la_LIBADD_not_used= \
  astFixup/libastFixup.la \
  accparser/libaccparser.la \
  ompparser/libompparser.la \
  astPostProcessing/libastPostProcessing.la \
  sageInterface/libsageInterface.la \
  virtualCFG/libvirtualCFG.la \
  astHiddenTypeAndDeclarationLists/libastHiddenTypeAndDeclarationLists.la \
  includeDirectivesProcessing/libincludeDirectivesProcessing.la \
  astTokenStream/libastTokenStream.la
#else
libsage3_la_LIBADD= \
  astFixup/libastFixup.la \
  accparser/libaccparser.la \
  ompparser/libompparser.la \
  astPostProcessing/libastPostProcessing.la \
  sageInterface/libsageInterface.la \
  virtualCFG/libvirtualCFG.la \
  astHiddenTypeAndDeclarationLists/libastHiddenTypeAndDeclarationLists.la \
  includeDirectivesProcessing/libincludeDirectivesProcessing.la
#endif

# DQ (6/25/2011): Readded this directory so that we can support the older behavior as a default
# although this will likely fail at least some of the nearly one hundred new name qualification tests.
# DQ (6/24/2011): Removed now the the name qualification support in a part of src/backend/unparser.
# astHiddenTypeAndDeclarationLists/libastHiddenTypeAndDeclarationLists.la 

if ROSE_USE_SMALLER_GENERATED_FILES
   libsage3_la_LIBADD += GENERATED_CODE_DIRECTORY_Cxx_Grammar/libgeneratedIR.la
endif

rose_attributes_list.lo : rose_attributes_list.h 

if ROSE_PCH
if ROSE_MPI
# TPS (11Feb2009): PCH only works when multithreading is off.
# We should not use PCH and MPI together.
roseh:

else
# DQ (2/11/2009): This is part of the support for Precompiled Headers (PCH) using GNU g++
roseh_v_at_0 = @
roseh_v_at_1 =
roseh_v_at_  = $(roseh_v_at_0)
roseh_v_at   = $(roseh_v_at_$(V))

roseh_v_echo_0 = @echo " COMPILE sage3basic.h.gch"
roseh_v_echo_1 =
roseh_v_echo_  = $(roseh_v_echo_0)
roseh_v_echo   = $(roseh_v_echo_$(V))

roseh:
#	DQ (2/10/2009): Rush for the off switch on precompiled header files, comment out to skip use of PCH.
#       TPS (23 Feb 2009): Hardcoded -fpic and -DPIC for now because 
#       for some reason it is used by most files although not specified on configure
	$(roseh_v_echo)
	$(roseh_v_at)$(CXX) -fPIC -DPIC $(CXXFLAGS) $(CFLAGS) $(CPPFLAGS) -g -o sage3basic.h.gch \
		$(top_srcdir)/src/frontend/SageIII/sage3basic.h -I$(top_builddir) $(ROSE_INCLUDES) 
endif
else
# default, do nothing.
roseh:

endif

test_roseh: roseh

pkginclude_HEADERS = \
   sage3.h sage3basic.h rose_attributes_list.h \
   attachPreprocessingInfo.h \
   attachPreprocessingInfoTraversal.h \
   attach_all_info.h manglingSupport.h C++_include_files.h \
   fixupCopy.h memory-pool-snapshot.h \
   general_token_defs.h rtiHelpers.h \
   ompSupport.h omp.h \
   omp_lib_kinds.h omp_lib.h sage3basic.hhh rosedefs.h  fileoffsetbits.h rosedll.h \
   $(fSageSupport_includeHeaders)

# DQ (4/5/2009): Moved rose_paths.h to src/util (where the source file is located).
nodist_pkginclude_HEADERS = \
   Cxx_Grammar.h \
   Cxx_GrammarTreeTraversalAccessEnums.h \
   AstQueryMemoryPool.h \
   license_string.h

BUILT_SOURCES = $(ROSETTA_GENERATED_SOURCE) omp_exprparser_parser.cc omp_exprparser_parser.hh omp_exprparser_lexer.cc license_string.h

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf Cxx_Grammar* license_string.h
	rm -rf AstQueryMemoryPool.h
	rm -rf generated_abstractcppgrammar.atg
	rm -f sage3basic.h.gch
	rm -f *.binary

distclean-local:
	rm -rf Templates.DB
	rm -f sage3basic.h.gch

removeHashLineDirectivesFromGeneratedCode:
	cp Cxx_Grammar.h  Cxx_Grammar.h.withLineDirectives
	grep -v "#line" Cxx_Grammar.h > Cxx_Grammar.h.withoutLineDirectives
	cp Cxx_Grammar.h.withoutLineDirectives Cxx_Grammar.h
