include_directories( ${CMAKE_SOURCE_DIR}/src/frontend/Experimental_General_Language_Support )

add_library(sageInterface OBJECT
  sageInterface.C
  sageInterface_type.C
  generateUniqueName.C
  sageBuilder.C
  sageBuilder_fortran.C
  abiStuff.C)

add_dependencies(sageInterface rosetta_generated)

install(
  FILES  sageInterface.h sageBuilder.h abiStuff.h
         sageFunctors.h sageGeneric.h
  DESTINATION ${INCLUDE_INSTALL_DIR}
  )
