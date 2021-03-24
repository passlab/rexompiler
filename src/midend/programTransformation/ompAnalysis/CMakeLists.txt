add_library(omp_analysis OBJECT ompAnalysis.cpp)
add_dependencies(omp_analysis rosetta_generated)
install(FILES ompAnalysis.h DESTINATION ${INCLUDE_INSTALL_DIR})

