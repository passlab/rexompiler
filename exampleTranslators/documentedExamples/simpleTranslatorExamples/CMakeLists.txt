add_executable(identityTranslator identityTranslator.cpp)
target_link_libraries(identityTranslator ROSE_DLL)
  install(TARGETS identityTranslator DESTINATION bin)
