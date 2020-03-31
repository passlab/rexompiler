include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# This directory support the separation of the generated files used for the ROSE IR
# into smaller files (to avoid a few large 300K line files).
SUBDIRS = 

AM_CPPFLAGS = $(ROSE_INCLUDES)

EXTRA_DIST = CMakeLists.txt 

# Use a makefile include meachanism.  However I can't make this work!
# include $(top_builddir)/src/frontend/SageIII/GENERATED_CODE_DIRECTORY_Cxx_Grammar/generatedFileList.inc
# GENERATED_SOURCE = SgAddOp.C

if ROSE_USE_SMALLER_GENERATED_FILES

haveRosettaGeneratedSource:
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)
	touch haveRosettaGeneratedSource

# Force ROSETTA to be run to generate the ROSE IR source code.
force-haveRosettaGeneratedSource:
	-rm *.h *.C
	$(MAKE) haveRosettaGeneratedSource

$(GENERATED_SOURCE): haveRosettaGeneratedSource

noinst_LTLIBRARIES = libgeneratedIR.la
nodist_libgeneratedIR_la_SOURCES = $(GENERATED_SOURCE)
# nodist_libgeneratedIR_la_SOURCES = $(GENERATED_SOURCE)
endif

# DQ (10/18/2007): Added AST_FILE_IO.h StorageClasses.h
# Removed sla.h (redundant with location in src/util/commandlineProcessing/sla.h)
# include_HEADERS = *.h

test:
	echo "Force ROSETTA to regenerate the source code for the ROSE IR."
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)

CLEANFILES = haveRosettaGeneratedSource

clean-local:
#	rm -rf libgeneratedIR.la
	rm -rf *.h *.C *.loT

printFiles:
	echo "GENERATED_SOURCE = $(GENERATED_SOURCE)"

printObjFiles:
	echo "GENERATED_OBJ = $(GENERATED_OBJ)"

distclean-local:


# DQ (12/29/2009): It seems that I am forced to express the list of source files explicitly.
# So it seems that this list will have to be modified if any new IR nodes are added.
SHORT_GENERATED_SOURCE = \
SgAccessModifier.C

GENERATED_SOURCE = \
SgAccessModifier.C                     SgCatchOptionStmt.C                   SgFunctionParameterList.C               SgOmpClause.C                                SgTemplateInstantiationMemberFunctionDecl.C \
SgActualArgumentExpression.C           SgCatchStatementSeq.C                 SgFunctionParameterTypeList.C           SgOmpCollapseClause.C                        SgTemplateParameter.C \
SgAddOp.C                              SgCharVal.C                           SgFunctionRefExp.C                      SgOmpCopyinClause.C                          SgTemplateParameterList.C \
SgAddressOfOp.C                        SgClassDeclaration.C                  SgFunctionSymbol.C                      SgOmpCopyprivateClause.C                     SgTemplateSymbol.C \
SgAggregateInitializer.C               SgClassDecl_attr.C                    SgFunctionType.C                        SgOmpCriticalStatement.C                     SgTemplateType.C \
SgAliasSymbol.C                        SgClassDefinition.C                   SgFunctionTypeSymbol.C                  SgOmpDefaultClause.C                         SgThisExp.C \
SgAllocateStatement.C                  SgClassNameRefExp.C                   SgFunctionTypeTable.C                   SgOmpDoStatement.C                           SgThrowOp.C \
SgAndAssignOp.C                        SgClassSymbol.C                       SgGlobal.C                              SgOmpExpressionClause.C                      SgToken.C \
SgAndOp.C                              SgClassType.C                         SgGotoStatement.C                       SgOmpFirstprivateClause.C              SgOmpNontemporalClause.C       SgTryStmt.C \
SgOmpInclusiveClause.C               SgOmpExclusiveClause.C                  SgOmpIsDevicePtrClause.C\
SgArithmeticIfStatement.C              SgClinkageDeclarationStatement.C      SgGraph.C                               SgOmpFlushStatement.C                        SgTypeBool.C \
SgArrayType.C                          SgClinkageEndStatement.C              SgGraphEdge.C                           SgOmpForStatement.C                          SgType.C \
SgArrowExp.C                           SgClinkageStartStatement.C            SgGraphEdgeList.C                       SgOmpIfClause.C                              SgTypeChar.C \
SgArrowStarOp.C                        SgCloseStatement.C                    SgGraphNode.C                           SgOmpLastprivateClause.C                     SgTypeComplex.C                  SgOmpMasterStatement.C\
SgColonShapeExp.C                     SgGraphNodeList.C                       SgOmpTaskyieldStatement.C                       SgTypeDefault.C \
SgCommaOpExp.C                        SgGreaterOrEqualOp.C                    SgOmpNowaitClause.C                          SgTypedefDeclaration.C \
SgOmpReverseOffloadClause.C          SgOmpUnifiedAddressClause.C           SgOmpUnifiedSharedMemoryClause.C \
SgOmpDynamicAllocatorsClause.C        SgOmpAtomicDefaultMemOrderClause.C     SgOmpExtImplementationDefinedRequirementClause.C \                     
SgAsmOp.C 			      SgCommonBlock.C                       SgGreaterThanOp.C                       SgOmpNumThreadsClause.C                      SgTypedefSeq.C                  SgOmpNumTeamsClause.C                         SgOmpOrderClause.C  SgOmpBindClause.C               SgOmpThreadLimitClause.C \
SgOmpNumParallelClause.C                 SgOmpNumSectionsClause.C                   SgOmpNumForClause.C                        SgOmpNumTaskgroupClause.C \
SgCommonBlockObject.C                 SgIdentDirectiveStatement.C             SgOmpOrderedClause.C                         SgTypedefSymbol.C \
SgCommonSymbol.C                      SgIfdefDirectiveStatement.C             SgOmpOrderedStatement.C                      SgTypedefType.C \
SgIfDirectiveStatement.C      SgOmpMetadirectiveStatement.C          SgOmpParallelStatement.C      SgOmpTeamsStatement.C    SgOmpLoopStatement.C       SgTypeDouble.C                       SgOmpRequiresStatement.C                    SgOmpScanStatement.C         SgOmpDistributeStatement.C   SgOmpTaskgroupStatement.C             SgOmpCancellationPointStatement.C \
SgComputedGotoStatement.C             SgIfndefDirectiveStatement.C            SgOmpPrivateClause.C                         SgTypeEllipse.C \
SgConcatenationOp.C                   SgIfStmt.C                              SgOmpReductionClause.C                       SgOmpAllocateClause.C \
SgOmpWhenClause.C                   SgTypeFloat.C                             SgOmpDistScheduleClause.C                    SgOmpInReductionClause.C \
SgOmpDefaultmapClause.C               SgOmpTaskReductionClause.C \
SgConditionalExp.C                    SgImagPartOp.C                          SgOmpScheduleClause.C                        SgTypeGlobalVoid.C \
SgConjugateOp.C                       SgImplicitStatement.C                   SgOmpSectionsStatement.C                     SgTypeIdOp.C \
SgConstructorInitializer.C            SgImpliedDo.C                           SgOmpSectionStatement.C                      SgTypeImaginary.C \
SgConstVolatileModifier.C             SgImportStatement.C                     SgOmpSharedClause.C                          SgTypeInt.C \
SgContainsStatement.C                 SgIncidenceDirectedGraph.C              SgOmpSingleStatement.C                       SgTypeLong.C \
SgContinueStmt.C                      SgIncidenceUndirectedGraph.C            SgOmpTaskStatement.C                         SgTypeLongDouble.C \
SgC_PreprocessorDirectiveStatement.C  SgIncludeDirectiveStatement.C           SgOmpTaskwaitStatement.C                     SgTypeLongLong.C \
SgCtorInitializerList.C               SgIncludeNextDirectiveStatement.C       SgOmpThreadprivateStatement.C                SgTypeModifier.C \
SgDataStatementGroup.C                SgInitializedName.C                     SgOmpUntiedClause.C                          SgTypeShort.C \
SgDataStatementObject.C               SgInitializer.C                         SgOmpVariablesClause.C                       SgTypeSignedChar.C \
SgDataStatementValue.C                SgInquireStatement.C                    SgOmpWorkshareStatement.C                    SgTypeSignedInt.C \
SgDeadIfDirectiveStatement.C          SgIntegerDivideOp.C                     SgOpenStatement.C                            SgTypeSignedLong.C \
SgDeallocateStatement.C               SgInterfaceBody.C                       SgOptions.C                                  SgTypeSignedLongLong.C \
SgDeclarationModifier.C               SgInterfaceStatement.C                  SgOrOp.C                                     SgTypeSignedShort.C \
SgDeclarationStatement.C              SgInterfaceSymbol.C                     SgParameterStatement.C                       SgTypeString.C \
SgDefaultOptionStmt.C                 SgIntKeyedBidirectionalGraph.C          SgPartialFunctionModifierType.C              SgTypeUnknown.C \
SgDefaultSymbol.C                     SgIntrinsicSymbol.C                     SgPartialFunctionType.C                      SgTypeUnsignedChar.C \
SgDefineDirectiveStatement.C          SgIntVal.C                              SgPlusAssignOp.C                             SgTypeUnsignedInt.C \
SgDeleteExp.C                         SgIOItemExpression.C                    SgPlusPlusOp.C                               SgTypeUnsignedLong.C \
SgDerivedTypeStatement.C              SgIorAssignOp.C                         SgPntrArrRefExp.C                            SgTypeUnsignedLongLong.C \
SgDesignatedInitializer.C             SgIOStatement.C                         SgPointerAssignOp.C                          SgTypeUnsignedShort.C \
SgDimensionObject.C                   SgLabelRefExp.C                         SgPointerDerefExp.C                          SgTypeVoid.C \
SgAsmStmt.C			      SgDirectedGraphEdge.C                 SgLabelStatement.C                      SgPointerMemberType.C                        SgTypeWchar.C \
SgDirectory.C                         SgLabelSymbol.C                         SgPointerType.C                              SgUnaryAddOp.C \
SgDirectoryList.C                     SgLessOrEqualOp.C                       SgPragma.C                                   SgUnaryOp.C \
SgDivAssignOp.C                       SgLessThanOp.C                          SgPragmaDeclaration.C                        SgUndefDirectiveStatement.C \
SgDivideOp.C                          SgLineDirectiveStatement.C              SgPrintStatement.C                           SgUndirectedGraphEdge.C \
SgDotExp.C                            SgLinemarkerDirectiveStatement.C        SgProcedureHeaderStatement.C                 SgUnknownArrayOrFunctionReference.C \
SgDotStarOp.C                         SgLinkageModifier.C                     SgProgramHeaderStatement.C                   SgUnknownFile.C \
SgLocatedNode.C                         SgProject.C                                  SgUnparse_Info.C \
SgDoWhileStmt.C                       SgLocatedNodeSupport.C                  SgPseudoDestructorRefExp.C                   SgUnsignedCharVal.C \
SgElaboratedTypeModifier.C            SgLongDoubleVal.C                       SgQualifiedName.C                            SgUnsignedIntVal.C \
SgElseDirectiveStatement.C            SgLongIntVal.C                          SgQualifiedNameType.C                        SgUnsignedLongLongIntVal.C \
SgElseifDirectiveStatement.C          SgLongLongIntVal.C                      SgReadStatement.C                            SgUnsignedLongVal.C \
SgElseWhereStatement.C                SgLshiftAssignOp.C                      SgRealPartOp.C                               SgUnsignedShortVal.C \
SgEmptyDirectiveStatement.C           SgLshiftOp.C                            SgReferenceType.C                            SgUPC_AccessModifier.C \
SgEndfileStatement.C                  SgMemberFunctionDeclaration.C           SgRefExp.C                                   SgUpcBarrierStatement.C \
SgEndifDirectiveStatement.C           SgMemberFunctionRefExp.C                SgRenamePair.C                               SgUpcBlocksizeof.C \
SgEntryStatement.C                    SgMemberFunctionSymbol.C                SgRenameSymbol.C                             SgUpcElemsizeof.C \
SgEnumDeclaration.C                   SgMemberFunctionType.C                  SgReturnStmt.C                               SgUpcFenceStatement.C \
SgEnumFieldSymbol.C                   SgMinusAssignOp.C                       SgRewindStatement.C                          SgUpcForAllStatement.C \
SgEnumSymbol.C                        SgMinusMinusOp.C                        SgRshiftAssignOp.C                           SgUpcLocalsizeof.C \
SgEnumType.C                          SgMinusOp.C                             SgRshiftOp.C                                 SgUpcMythread.C \
SgEnumVal.C                           SgModAssignOp.C                         SgScopeOp.C                                  SgUpcNotifyStatement.C \
SgEqualityOp.C                        SgModifier.C                            SgScopeStatement.C                           SgUpcThreads.C \
SgEquivalenceStatement.C              SgModifierNodes.C                       SgSequenceStatement.C                        SgUpcWaitStatement.C \
SgAssignedGotoStatement.C                  SgErrorDirectiveStatement.C           SgModifierType.C                        SgShortVal.C                                 SgUserDefinedBinaryOp.C \
SgAssignInitializer.C                      SgExponentiationOp.C                  SgModOp.C                               SgSizeOfOp.C                                 SgUserDefinedUnaryOp.C \
SgAssignOp.C                               SgExpression.C                        SgModuleStatement.C                     SgSourceFile.C                               SgUseStatement.C \
SgAssignStatement.C                        SgExpressionRoot.C                    SgModuleSymbol.C                        SgSpawnStmt.C                                SgUsingDeclarationStatement.C \
SgAssociateStatement.C                     SgExprListExp.C                       SgMultAssignOp.C                        SgSpecialFunctionModifier.C                  SgUsingDirectiveStatement.C \
SgAsteriskShapeExp.C                       SgExprStatement.C                     SgMultiplyOp.C                          SgStatement.C                                SgValueExp.C \
SgAttribute.C                              SgFile.C                              SgName.C                                SgStatementExpression.C                      SgVarArgCopyOp.C \
SgAttributeSpecificationStatement.C        Sg_File_Info.C                        SgNamedType.C                           SgStatementFunctionStatement.C               SgVarArgEndOp.C \
SgBackspaceStatement.C                     SgFileList.C                          SgNameGroup.C                           SgStopOrPauseStatement.C                     SgVarArgOp.C \
SgBaseClass.C                              SgFloatVal.C                          SgNamelistStatement.C                   SgStorageModifier.C                          SgVarArgStartOneOperandOp.C \
SgBaseClassModifier.C                      SgFlushStatement.C                    SgNamespaceAliasDeclarationStatement.C  SgStringKeyedBidirectionalGraph.C            SgVarArgStartOp.C \
SgBasicBlock.C                             SgForAllStatement.C                   SgNamespaceDeclarationStatement.C       SgStringVal.C                                SgVariableDeclaration.C \
SgBidirectionalGraph.C                     SgForInitStatement.C                  SgNamespaceDefinitionStatement.C        SgSubscriptExpression.C                      SgVariableDefinition.C \
SgBinaryComposite.C                        SgFormatItem.C                        SgNamespaceSymbol.C                     SgSubtractOp.C                               SgVariableSymbol.C \
SgBinaryOp.C                               SgFormatItemList.C                    SgNewExp.C                              SgSupport.C                                  SgVariantExpression.C \
SgBitAndOp.C                               SgFormatStatement.C                   SgNode.C                                SgSwitchStatement.C                          SgVariantStatement.C \
SgBitAttribute.C                           SgForStatement.C                      SgNotEqualOp.C                          SgSymbol.C                                   SgVarRefExp.C \
SgBitComplementOp.C                        SgFortranDo.C                         SgNotOp.C                               SgSymbolTable.C                              SgWaitStatement.C \
SgBitOrOp.C                                SgFortranIncludeLine.C                SgNullExpression.C                      SgTemplateArgument.C                         SgWarningDirectiveStatement.C \
SgBitXorOp.C                               SgFortranNonblockedDo.C               SgNullifyStatement.C                    SgTemplateArgumentList.C                     SgWcharVal.C \
SgBlockDataStatement.C                     SgFuncDecl_attr.C                     SgNullStatement.C                       SgTemplateDeclaration.C                      SgWhereStatement.C \
SgBoolValExp.C                             SgFunctionCallExp.C                   SgOmpAtomicStatement.C                  SgTemplateInstantiationDecl.C                SgWhileStmt.C \
SgBreakStmt.C                              SgFunctionDeclaration.C               SgOmpBarrierStatement.C                 SgTemplateInstantiationDefn.C                SgWriteStatement.C \
SgCaseOptionStmt.C                         SgFunctionDefinition.C                SgOmpBodyStatement.C                    SgTemplateInstantiationDirectiveStatement.C  SgXorAssignOp.C \
SgCastExp.C                                SgFunctionModifier.C                  SgOmpClauseBodyStatement.C              SgTemplateInstantiationFunctionDecl.C        SgHeaderFileBody.C






