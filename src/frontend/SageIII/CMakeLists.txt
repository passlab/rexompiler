# tps (04/01/2010) : Added  GENERATED_CODE_DIRECTORY_Cxx_Grammar subdirectory
add_subdirectory(GENERATED_CODE_DIRECTORY_Cxx_Grammar)
add_subdirectory(astFixup)
add_subdirectory(astPostProcessing)
add_subdirectory(sageInterface)
add_subdirectory(virtualCFG)
add_subdirectory(ompparser)
set(USE_REX true)
set(USE_OMPPARSER true)
add_subdirectory(accparser)

if(NOT enable-internalFrontendDevelopment)
  ##AS Commented out for simpler build
  add_subdirectory(astHiddenTypeAndDeclarationLists)
  add_subdirectory(astTokenStream)
  add_subdirectory(includeDirectivesProcessing)
endif()

set(SAGE3_SOURCES
  accAstConstruction.cpp
  rose_attributes_list.C
  attachPreprocessingInfo.C
  attachPreprocessingInfoTraversal.C
  attributeListMap.C
  manglingSupport.C
  sage_support/sage_support.cpp
  sage_support/cmdline.cpp
  sage_support/keep_going.cpp
  sage_support/utility_functions.C
  fixupCopy_scopes.C
  fixupCopy_symbols.C
  fixupCopy_references.C
  IncludeDirective.C
  rtiHelpers.C
  rose_graph_support.C
  Utf8.C)

# GENERATING THE ROSE PREPROCESSOR
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/lex.yy.C
  COMMAND ${FLEX_EXECUTABLE} -t
    ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/preproc-c.ll > ${CMAKE_BINARY_DIR}/src/frontend/SageIII/lex.yy.C
  DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/preproc-c.ll)

add_custom_target( ROSE_PREPROCESSOR ALL DEPENDS
  ${CMAKE_BINARY_DIR}/src/frontend/SageIII/lex.yy.C COMMENT "Generating files for preprocessor" )

#GENERATING the OpenMP parser
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp_exprparser_lexer.cc
  COMMAND ${FLEX_EXECUTABLE} -t
    ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/omp_exprparser_lexer.ll > ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp_exprparser_lexer.cc
  DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/omp_exprparser_lexer.ll)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp_exprparser_parser.cc
  COMMAND ${BISON_EXECUTABLE}
    ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/omp_exprparser_parser.yy -o
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp_exprparser_parser.cc
    #    && mv omp_exprparser_parser.H omp_exprparser_parser.hh
  DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/omp_exprparser_parser.yy)

#GENERATING the license_string.h file
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/license_string.h
  COMMAND ${PERL_EXECUTABLE} -t
    ${ROSE_TOP_SRC_DIR}/scripts/to-c-string.pl < ${ROSE_TOP_SRC_DIR}/LicenseInformation/ROSE_BSD_License.txt > ${CMAKE_BINARY_DIR}/src/frontend/SageIII/license_string.h
  DEPENDS  ${ROSE_TOP_SRC_DIR}/LicenseInformation/ROSE_BSD_License.txt)

set(SAGE3_SOURCES ${SAGE3_SOURCES}
  ompSupport.C
  ompAstConstruction.cpp ompFortranParser.C
        omp_exprparser_lexer.ll omp_exprparser_parser.yy
  ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.C
  ${CMAKE_CURRENT_BINARY_DIR}/omp_exprparser_parser.cc
  ${CMAKE_CURRENT_BINARY_DIR}/omp_exprparser_lexer.cc
  ${CMAKE_CURRENT_BINARY_DIR}/license_string.h)

########### next target ###############
add_library(sage3 OBJECT ${SAGE3_SOURCES})
add_dependencies(sage3 rosetta_generated)

########### install files ###############

install(
  FILES
    sage3.h sage3basic.h rose_attributes_list.h attachPreprocessingInfo.h
    attachPreprocessingInfoTraversal.h attach_all_info.h manglingSupport.h
    C++_include_files.h fixupCopy.h general_token_defs.h rtiHelpers.h
    ompAstConstruction.h omp.h ompSupport.h
    omp_lib_kinds.h omp_lib.h rosedll.h fileoffsetbits.h rosedefs.h
    sage3basic.hhh sage_support/cmdline.h sage_support/sage_support.h
    sage_support/utility_functions.h sage_support/utility_functionsImpl.C
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_Grammar.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarTreeTraversalAccessEnums.h
    ${CMAKE_CURRENT_BINARY_DIR}/AstQueryMemoryPool.h
  DESTINATION ${INCLUDE_INSTALL_DIR})

