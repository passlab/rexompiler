add_library( astDump OBJECT
  astGraph.C
  AstAttributeDOT.C
  wholeAST.C
  copyGraph.C
  walrus.C
  includeGraph.C)
add_dependencies(astDump rosetta_generated)


########### install files ###############

install(FILES  AstAttributeDOT.h astGraph.h astGraphTemplateImpl.h wholeAST.h
               wholeAST_API.h copyGraph.h walrus.h
               intermediateRepresentationNodeGraphs.h
               includeGraph.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
