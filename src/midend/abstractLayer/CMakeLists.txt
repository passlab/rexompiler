
add_library(abstractLayer OBJECT
  AstAnnotator.C
  AstTerm.C
  RoseAst.C
  Label.C
  Labeler.C
  SgNodeHelper.C
  VariableIdMapping.C
  )

add_dependencies(abstractLayer rosetta_generated)

install(FILES
  AstTerm.h
  RoseAst.h
  AstAnnotator.h
  Label.h
  Labeler.h
  SgNodeHelper.h
  VariableIdMapping.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
