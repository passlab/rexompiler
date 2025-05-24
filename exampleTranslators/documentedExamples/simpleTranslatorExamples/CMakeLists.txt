add_executable(identityTranslator identityTranslator.cpp)
target_link_libraries(identityTranslator ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS identityTranslator DESTINATION bin)
