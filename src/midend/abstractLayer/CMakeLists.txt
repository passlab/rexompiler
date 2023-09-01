
add_library(abstractLayer OBJECT
  RoseAst.C
  SgNodeHelper.C
  )

add_dependencies(abstractLayer rosetta_generated)

install(FILES
  RoseAst.h
  SgNodeHelper.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
