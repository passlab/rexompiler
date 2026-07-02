add_executable(pdfGenerator pdfGenerator.C)
add_dependencies(pdfGenerator ROSE_DLL)
target_link_libraries(pdfGenerator ROSE_DLL)
install(TARGETS pdfGenerator DESTINATION bin)
