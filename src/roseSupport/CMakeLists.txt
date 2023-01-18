########### build library ###############
add_definitions(-DROSE_DLL_EXPORTS)
add_library(roseSupport OBJECT
  transformationSupport.C optionDeclaration.C #Combinatorics.C
  utility_functions.C IncludeDirective.C
  stringify.C)
add_dependencies(roseSupport rosetta_generated)
if(READLINE_FOUND)
  target_include_directories(roseSupport PRIVATE ${READLINE_INCLUDE_DIR})
  target_link_libraries(roseSupport ${READLINE_LIBRARY})
endif()

########### install files ###############

install(FILES  transformationSupport.h
               optionDeclaration.h
               utility_functions.h
               IncludeDirective.h
               utility_functionsImpl.C
               stringify.h
	       Combinatorics.h
	DESTINATION ${INCLUDE_INSTALL_DIR}
  )
