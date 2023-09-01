add_library( astDump OBJECT
  astGraph.C
  AstAttributeDOT.C
  wholeAST.C
  copyGraph.C
  walrus.C
  includeGraph.C
  AstDOTGeneration.C
  AstPDFGeneration.C
  PDFGeneration.C)
add_dependencies(astDump rosetta_generated)


########### install files ###############

install(FILES  AstAttributeDOT.h astGraph.h astGraphTemplateImpl.h wholeAST.h
               wholeAST_API.h copyGraph.h walrus.h
               intermediateRepresentationNodeGraphs.h
               includeGraph.h
               AstDOTGeneration.h AstPDFGeneration.h
	       DOTGeneration.h DOTGenerationImpl.h DOTGraphInterface.h
               DOTRepresentation.h DOTRepresentationImpl.h DOTSubgraphRepresentation.h
               DOTSubgraphRepresentationImpl.h PDFGeneration.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
