
SET(TESTCODES
    findMain.C
)

set(TESTCODE_INCLUDES
    
    -I${CMAKE_SOURCE_DIR}/src
    -I${CMAKE_SOURCE_DIR}/src/util
    -I${CMAKE_SOURCE_DIR}/src/util/StringUtility
    -I${CMAKE_SOURCE_DIR}/src/util/commandlineProcessing
    -I${CMAKE_SOURCE_DIR}/src/frontend
    -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII
    -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII/virtualCFG
    -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII/sage_support
    -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII/sageInterface
    -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII/astPostProcessing
    -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII/astFixup
    -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII/astVisualization
    -I${CMAKE_SOURCE_DIR}/src/midend/astProcessing
    -I${CMAKE_SOURCE_DIR}/src/midend/astQuery
    -I${CMAKE_SOURCE_DIR}/src/midend/astDiagnostics
    -I${CMAKE_SOURCE_DIR}/src/midend/programTransformation/ompLowering
    -I${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astInlining
    -I${CMAKE_SOURCE_DIR}/src/midend/programTransformation/constantFolding
    -I${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis
    -I${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis
    -I${CMAKE_SOURCE_DIR}/src/backend/unparser
    -I${CMAKE_SOURCE_DIR}/src/backend/unparser/languageIndependenceSupport
    -I${CMAKE_SOURCE_DIR}/src/backend/unparser/formatSupport
    -I${CMAKE_SOURCE_DIR}/src/backend/unparser/CxxCodeGeneration
    -I${CMAKE_SOURCE_DIR}/src/backend/unparser/FortranCodeGeneration
    -I${CMAKE_SOURCE_DIR}/src/3rdPartyLibraries/MSTL
    -I${CMAKE_BINARY_DIR}
    -I${CMAKE_BINARY_DIR}/src/frontend/SageIII
)

set(TESTCODE_LINK -L${CMAKE_BINARY_DIR}/lib -lrose)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME ASTINFTEST_${file_to_test}
    COMMAND g++ ${TESTCODE_INCLUDES}
        ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test} -o ${file_to_test}.exe
        ${TESTCODE_LINK}
  )
  #set_tests_properties(OMPTEST_${file_to_test} PROPERTIES LABELS OMPTEST)
endforeach()

