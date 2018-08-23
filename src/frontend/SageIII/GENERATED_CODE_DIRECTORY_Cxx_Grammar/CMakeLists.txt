
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

# Use this as a way to turn off this optional use to build smaller files
# so that we test this unter Linux before worring about use within Windows.
# tps (01/04/10) : Added name for if condition
if(VERBOSE)
  message (STATUS "smallerGeneratedFiles : " ${enable-smaller-generated-files})
endif()

<<<<<<< 466ca7fa3c1f48510e41c1653e984e108ba16098
if(enable-smaller-generated-files)
=======
########### next target ###############
set( libgeneratedIR_SRC
SgAccessModifier.C                     SgAsmDwarfRestrictType.C            SgAsmLESection.C                           SgCatchOptionStmt.C                   SgFunctionParameterList.C               SgOmpClause.C                                SgTemplateInstantiationMemberFunctionDecl.C 
SgActualArgumentExpression.C           SgAsmDwarfSetType.C                 SgAsmLESectionTable.C                      SgCatchStatementSeq.C                 SgFunctionParameterTypeList.C           SgOmpCollapseClause.C                        SgTemplateParameter.C 
SgAddOp.C                              SgAsmDwarfSharedType.C              SgAsmLESectionTableEntry.C                 SgCharVal.C                           SgFunctionRefExp.C                      SgOmpCopyinClause.C                          SgTemplateParameterList.C 
SgAddressOfOp.C                        SgAsmDwarfStringType.C              SgAsmMemoryReferenceExpression.C           SgClassDeclaration.C                  SgFunctionSymbol.C                      SgOmpCopyprivateClause.C                     SgTemplateSymbol.C 
SgAggregateInitializer.C               SgAsmDwarfStructureType.C           SgAsmNEEntryPoint.C                        SgClassDecl_attr.C                    SgFunctionType.C                        SgOmpCriticalStatement.C                     SgTemplateType.C 
SgAliasSymbol.C                        SgAsmDwarfSubprogram.C              SgAsmNEEntryTable.C                        SgClassDefinition.C                   SgFunctionTypeSymbol.C                  SgOmpDefaultClause.C                         SgThisExp.C 
SgAllocateStatement.C                  SgAsmDwarfSubrangeType.C            SgAsmNEFileHeader.C                        SgClassNameRefExp.C                   SgFunctionTypeTable.C                   SgOmpDoStatement.C                           SgThrowOp.C 
SgAndAssignOp.C                        SgAsmDwarfSubroutineType.C          SgAsmNEModuleTable.C                       SgClassSymbol.C                       SgGlobal.C                              SgOmpExpressionClause.C                      SgToken.C 
SgAndOp.C                              SgAsmDwarfTemplateTypeParameter.C   SgAsmNENameTable.C                         SgClassType.C                         SgGotoStatement.C                       SgOmpFirstprivateClause.C                    SgTryStmt.C 
SgArithmeticIfStatement.C              SgAsmDwarfTemplateValueParameter.C  SgAsmNERelocEntry.C                        SgClinkageDeclarationStatement.C      SgGraph.C                               SgOmpFlushStatement.C                        SgTypeBool.C 
SgArrayType.C                          SgAsmDwarfThrownType.C              SgAsmNERelocTable.C                        SgClinkageEndStatement.C              SgGraphEdge.C                           SgOmpForStatement.C                          SgType.C 
SgArrowExp.C                           SgAsmDwarfTryBlock.C                SgAsmNESection.C                           SgClinkageStartStatement.C            SgGraphEdgeList.C                       SgOmpIfClause.C                              SgTypeChar.C 
SgArrowStarOp.C                        SgAsmDwarfTypedef.C                 SgAsmNESectionTable.C                      SgCloseStatement.C                    SgGraphNode.C                           SgOmpLastprivateClause.C                     SgTypeComplex.C 
SgAsmArmInstruction.C                  SgAsmDwarfUnionType.C               SgAsmNESectionTableEntry.C                 SgColonShapeExp.C                     SgGraphNodeList.C                       SgOmpMasterStatement.C                       SgTypeDefault.C 
SgAsmDwarfUnknownConstruct.C        SgAsmNEStringTable.C                       SgCommaOpExp.C                        SgGreaterOrEqualOp.C                    SgOmpNowaitClause.C                          SgTypedefDeclaration.C 
SgAsmBasicString.C                     SgAsmDwarfUnspecifiedParameters.C   SgAsmNode.C                                SgCommonBlock.C                       SgGreaterThanOp.C                       SgOmpNumThreadsClause.C                      SgTypedefSeq.C 
SgAsmBinaryAdd.C                       SgAsmDwarfUnspecifiedType.C         SgAsmOp.C                                  SgCommonBlockObject.C                 SgIdentDirectiveStatement.C             SgOmpOrderedClause.C                         SgTypedefSymbol.C 
SgAsmBinaryAddPostupdate.C             SgAsmDwarfUpcRelaxedType.C          SgAsmOperandList.C                         SgCommonSymbol.C                      SgIfdefDirectiveStatement.C             SgOmpOrderedStatement.C                      SgTypedefType.C 
SgAsmBinaryAddPreupdate.C              SgAsmDwarfUpcSharedType.C           SgAsmPEExportDirectory.C                   SgComplexVal.C                        SgIfDirectiveStatement.C                SgOmpParallelStatement.C                     SgTypeDouble.C 
SgAsmBinaryAsr.C                       SgAsmDwarfUpcStrictType.C           SgAsmPEExportEntry.C                       SgComputedGotoStatement.C             SgIfndefDirectiveStatement.C            SgOmpPrivateClause.C                         SgTypeEllipse.C 
SgAsmBinaryDivide.C                    SgAsmDwarfVariable.C                SgAsmPEExportEntryList.C                   SgConcatenationOp.C                   SgIfStmt.C                              SgOmpReductionClause.C   SgOmpAllocateClause.C                    SgTypeFloat.C 
SgAsmBinaryExpression.C                SgAsmDwarfVariant.C                 SgAsmPEExportSection.C                     SgConditionalExp.C                    SgImagPartOp.C                          SgOmpScheduleClause.C                        SgTypeGlobalVoid.C 
SgAsmBinaryLsl.C                       SgAsmDwarfVariantPart.C             SgAsmPEFileHeader.C                        SgConjugateOp.C                       SgImplicitStatement.C                   SgOmpSectionsStatement.C                     SgTypeIdOp.C 
SgAsmBinaryLsr.C                       SgAsmDwarfVolatileType.C            SgAsmPEImportDirectory.C                   SgConstructorInitializer.C            SgImpliedDo.C                           SgOmpSectionStatement.C                      SgTypeImaginary.C 
SgAsmBinaryMod.C                       SgAsmDwarfWithStmt.C                SgAsmPEImportDirectoryList.C               SgConstVolatileModifier.C             SgImportStatement.C                     SgOmpSharedClause.C                          SgTypeInt.C 
SgAsmBinaryMultiply.C                  SgAsmElfDynamicEntry.C              SgAsmPEImportItem.C                    SgContainsStatement.C                 SgIncidenceDirectedGraph.C              SgOmpSingleStatement.C                       SgTypeLong.C 
SgAsmBinaryRor.C                       SgAsmElfDynamicEntryList.C          SgAsmPEImportItemList.C                SgContinueStmt.C                      SgIncidenceUndirectedGraph.C            SgOmpTaskStatement.C                         SgTypeLongDouble.C 
SgAsmBinarySubtract.C                  SgAsmElfDynamicSection.C            SgC_PreprocessorDirectiveStatement.C  SgIncludeDirectiveStatement.C           SgOmpTaskwaitStatement.C                     SgTypeLongLong.C 
SgAsmBinarySubtractPostupdate.C        SgAsmElfEHFrameEntryCI.C            SgCtorInitializerList.C               SgIncludeNextDirectiveStatement.C       SgOmpThreadprivateStatement.C                SgTypeModifier.C 
SgAsmBinarySubtractPreupdate.C         SgAsmElfEHFrameEntryCIList.C        SgDataStatementGroup.C                SgInitializedName.C                     SgOmpUntiedClause.C                          SgTypeShort.C 
SgAsmBlock.C                           SgAsmElfEHFrameEntryFD.C            SgAsmPEImportSection.C                     SgDataStatementObject.C               SgInitializer.C                         SgOmpVariablesClause.C                       SgTypeSignedChar.C 
SgAsmIntegerValueExpression.C             SgAsmElfEHFrameEntryFDList.C        SgAsmPERVASizePair.C                       SgDataStatementValue.C                SgInquireStatement.C                    SgOmpWorkshareStatement.C                    SgTypeSignedInt.C 
SgAsmCoffStrtab.C                      SgAsmElfEHFrameSection.C            SgAsmPERVASizePairList.C                   SgDeadIfDirectiveStatement.C          SgIntegerDivideOp.C                     SgOpenStatement.C                            SgTypeSignedLong.C 
SgAsmCoffSymbol.C                      SgAsmElfFileHeader.C                SgAsmPESection.C                           SgDeallocateStatement.C               SgInterfaceBody.C                       SgOptions.C                                  SgTypeSignedLongLong.C 
SgAsmCoffSymbolList.C                  SgAsmElfNoteEntry.C                 SgAsmPESectionTable.C                      SgDeclarationModifier.C               SgInterfaceStatement.C                  SgOrOp.C                                     SgTypeSignedShort.C 
SgAsmCoffSymbolTable.C                 SgAsmElfNoteEntryList.C             SgAsmPESectionTableEntry.C                 SgDeclarationStatement.C              SgInterfaceSymbol.C                     SgParameterStatement.C                       SgTypeString.C 
SgAsmCommonSubExpression.C             SgAsmElfNoteSection.C               SgAsmPEStringSection.C                     SgDefaultOptionStmt.C                 SgIntKeyedBidirectionalGraph.C          SgPartialFunctionModifierType.C              SgTypeUnknown.C 
SgAsmControlFlagsExpression.C          SgAsmElfRelocEntry.C                SgAsmPowerpcInstruction.C                  SgDefaultSymbol.C                     SgIntrinsicSymbol.C                     SgPartialFunctionType.C                      SgTypeUnsignedChar.C 
SgAsmDataStructureDeclaration.C        SgAsmElfRelocEntryList.C            SgDefineDirectiveStatement.C          SgIntVal.C                              SgPlusAssignOp.C                             SgTypeUnsignedInt.C 
SgAsmElfRelocSection.C              SgDeleteExp.C                         SgIOItemExpression.C                    SgPlusPlusOp.C                               SgTypeUnsignedLong.C 
SgAsmDOSExtendedHeader.C               SgAsmElfSection.C                   SgAsmRegisterReferenceExpression.C         SgDerivedTypeStatement.C              SgIorAssignOp.C                         SgPntrArrRefExp.C                            SgTypeUnsignedLongLong.C 
SgAsmDOSFileHeader.C                   SgAsmElfSectionTable.C              SgAsmSingleFloatValueExpression.C          SgDesignatedInitializer.C             SgIOStatement.C                         SgPointerAssignOp.C                          SgTypeUnsignedShort.C 
SgAsmDoubleFloatValueExpression.C      SgAsmElfSectionTableEntry.C         SgAsmStatement.C                           SgDimensionObject.C                   SgLabelRefExp.C                         SgPointerDerefExp.C                          SgTypeVoid.C 
SgAsmElfSegmentTable.C              SgAsmStmt.C                                SgDirectedGraphEdge.C                 SgLabelStatement.C                      SgPointerMemberType.C                        SgTypeWchar.C 
SgAsmDwarfAccessDeclaration.C          SgAsmElfSegmentTableEntry.C         SgAsmStoredString.C                        SgDirectory.C                         SgLabelSymbol.C                         SgPointerType.C                              SgUnaryAddOp.C 
SgAsmDwarfArrayType.C                  SgAsmElfSegmentTableEntryList.C     SgAsmStringStorage.C                       SgDirectoryList.C                     SgLessOrEqualOp.C                       SgPragma.C                                   SgUnaryOp.C 
SgAsmDwarfBaseType.C                   SgAsmElfStringSection.C             SgAsmType128bitFloat.C                     SgDivAssignOp.C                       SgLessThanOp.C                          SgPragmaDeclaration.C                        SgUndefDirectiveStatement.C 
SgAsmDwarfCatchBlock.C                 SgAsmElfStrtab.C                    SgAsmType80bitFloat.C                      SgDivideOp.C                          SgLineDirectiveStatement.C              SgPrintStatement.C                           SgUndirectedGraphEdge.C 
SgAsmDwarfClassTemplate.C              SgAsmElfSymbol.C                    SgAsmTypeByte.C                            SgDotExp.C                            SgLinemarkerDirectiveStatement.C        SgProcedureHeaderStatement.C                 SgUnknownArrayOrFunctionReference.C 
SgAsmDwarfClassType.C                  SgAsmElfSymbolList.C                SgAsmType.C                                SgDotStarOp.C                         SgLinkageModifier.C                     SgProgramHeaderStatement.C                   SgUnknownFile.C 
SgAsmDwarfCommonBlock.C                SgAsmElfSymbolSection.C             SgAsmTypeDoubleFloat.C                     SgDoubleVal.C                         SgLocatedNode.C                         SgProject.C                                  SgUnparse_Info.C 
SgAsmDwarfCommonInclusion.C            SgAsmElfSymverDefinedAux.C          SgAsmTypeDoubleQuadWord.C                  SgDoWhileStmt.C                       SgLocatedNodeSupport.C                  SgPseudoDestructorRefExp.C                   SgUnsignedCharVal.C 
SgAsmDwarfCompilationUnit.C            SgAsmElfSymverDefinedAuxList.C      SgAsmTypeDoubleWord.C                      SgElaboratedTypeModifier.C            SgLongDoubleVal.C                       SgQualifiedName.C                            SgUnsignedIntVal.C 
SgAsmDwarfCompilationUnitList.C        SgAsmElfSymverDefinedEntry.C        SgAsmTypeQuadWord.C                        SgElseDirectiveStatement.C            SgLongIntVal.C                          SgQualifiedNameType.C                        SgUnsignedLongLongIntVal.C 
SgAsmDwarfCondition.C                  SgAsmElfSymverDefinedEntryList.C    SgAsmTypeSingleFloat.C                     SgElseifDirectiveStatement.C          SgLongLongIntVal.C                      SgReadStatement.C                            SgUnsignedLongVal.C 
SgAsmDwarfConstant.C                   SgAsmElfSymverDefinedSection.C      SgAsmTypeVector.C                          SgElseWhereStatement.C                SgLshiftAssignOp.C                      SgRealPartOp.C                               SgUnsignedShortVal.C 
SgAsmDwarfConstruct.C                  SgAsmElfSymverEntry.C               SgAsmTypeWord.C                            SgEmptyDirectiveStatement.C           SgLshiftOp.C                            SgReferenceType.C                            SgUPC_AccessModifier.C 
SgAsmDwarfConstructList.C              SgAsmElfSymverEntryList.C           SgAsmUnaryArmSpecialRegisterList.C         SgEndfileStatement.C                  SgMemberFunctionDeclaration.C           SgRefExp.C                                   SgUpcBarrierStatement.C 
SgAsmDwarfConstType.C                  SgAsmElfSymverNeededAux.C
SgAsmUnaryExpression.C                     SgEndifDirectiveStatement.C
SgMemberFunctionRefExp.C                SgRenamePair.C
SgUpcBlocksizeofExpression.C 
SgAsmDwarfDwarfProcedure.C             SgAsmElfSymverNeededAuxList.C
SgAsmUnaryMinus.C                          SgEntryStatement.C
SgMemberFunctionSymbol.C                SgRenameSymbol.C
SgUpcElemsizeofExpression.C 
SgAsmDwarfEntryPoint.C                 SgAsmElfSymverNeededEntry.C         SgAsmUnaryPlus.C                           SgEnumDeclaration.C                   SgMemberFunctionType.C                  SgReturnStmt.C                               SgUpcFenceStatement.C 
SgAsmDwarfEnumerationType.C            SgAsmElfSymverNeededEntryList.C     SgAsmUnaryRrx.C                            SgEnumFieldSymbol.C                   SgMinusAssignOp.C                       SgRewindStatement.C                          SgUpcForAllStatement.C 
SgAsmDwarfEnumerator.C                 SgAsmElfSymverNeededSection.C
SgAsmValueExpression.C                     SgEnumSymbol.C
SgMinusMinusOp.C                        SgRshiftAssignOp.C
SgUpcLocalsizeofExpression.C 
SgAsmDwarfFileType.C                   SgAsmElfSymverSection.C             SgAsmVectorValueExpression.C               SgEnumType.C                          SgMinusOp.C                             SgRshiftOp.C                                 SgUpcMythread.C 
SgAsmDwarfFormalParameter.C            SgAsmExecutableFileFormat.C         SgEnumVal.C                           SgModAssignOp.C                         SgScopeOp.C                                  SgUpcNotifyStatement.C 
SgAsmDwarfFormatLabel.C                SgAsmExpression.C                   SgAsmX86Instruction.C                      SgEqualityOp.C                        SgModifier.C                            SgScopeStatement.C                           SgUpcThreads.C 
SgAsmDwarfFriend.C                     SgAsmExprListExp.C                  SgEquivalenceStatement.C              SgModifierNodes.C                       SgSequenceStatement.C                        SgUpcWaitStatement.C 
SgAsmDwarfFunctionTemplate.C           SgAsmFieldDeclaration.C             SgAssignedGotoStatement.C                  SgErrorDirectiveStatement.C           SgModifierType.C                        SgShortVal.C                                 SgUserDefinedBinaryOp.C 
SgAsmDwarfImportedDeclaration.C        SgAsmFunction.C          SgAssignInitializer.C                      SgExponentiationOp.C                  SgModOp.C                               SgSizeOfOp.C                                 SgUserDefinedUnaryOp.C 
SgAsmDwarfImportedModule.C             SgAsmGenericDLL.C                   SgAssignOp.C                               SgExpression.C                        SgModuleStatement.C                     SgSourceFile.C                               SgUseStatement.C 
SgAsmDwarfImportedUnit.C               SgAsmGenericDLLList.C               SgAssignStatement.C                        SgExpressionRoot.C                    SgModuleSymbol.C                        SgSpawnStmt.C                                SgUsingDeclarationStatement.C 
SgAsmDwarfInformation.C                SgAsmGenericFile.C                  SgAssociateStatement.C                     SgExprListExp.C                       SgMultAssignOp.C                        SgSpecialFunctionModifier.C                  SgUsingDirectiveStatement.C 
SgAsmDwarfInheritance.C                SgAsmGenericFileList.C              SgAsteriskShapeExp.C                       SgExprStatement.C                     SgMultiplyOp.C                          SgStatement.C                                SgValueExp.C 
SgAsmDwarfInlinedSubroutine.C          SgAsmGenericFormat.C                SgAttribute.C                              SgFile.C                              SgName.C                                SgStatementExpression.C                      SgVarArgCopyOp.C 
SgAsmDwarfInterfaceType.C              SgAsmGenericHeader.C                SgAttributeSpecificationStatement.C        Sg_File_Info.C                        SgNamedType.C                           SgStatementFunctionStatement.C               SgVarArgEndOp.C 
SgAsmDwarfLabel.C                      SgAsmGenericHeaderList.C            SgBackspaceStatement.C                     SgFileList.C                          SgNameGroup.C                           SgStopOrPauseStatement.C                     SgVarArgOp.C 
SgAsmDwarfLexicalBlock.C               SgAsmGenericSection.C               SgBaseClass.C                              SgFloatVal.C                          SgNamelistStatement.C                   SgStorageModifier.C                          SgVarArgStartOneOperandOp.C 
SgAsmDwarfLine.C                       SgAsmGenericSectionList.C           SgBaseClassModifier.C                      SgFlushStatement.C                    SgNamespaceAliasDeclarationStatement.C  SgStringKeyedBidirectionalGraph.C            SgVarArgStartOp.C 
SgAsmDwarfLineList.C                   SgAsmGenericString.C                SgBasicBlock.C                             SgForAllStatement.C                   SgNamespaceDeclarationStatement.C       SgStringVal.C                                SgVariableDeclaration.C 
SgAsmDwarfMacro.C                      SgAsmGenericStrtab.C                SgBidirectionalGraph.C                     SgForInitStatement.C                  SgNamespaceDefinitionStatement.C        SgSubscriptExpression.C                      SgVariableDefinition.C 
SgAsmDwarfMacroList.C                  SgAsmGenericSymbol.C                SgBinaryComposite.C                        SgFormatItem.C                        SgNamespaceSymbol.C                     SgSubtractOp.C                               SgVariableSymbol.C 
SgAsmDwarfMember.C                     SgAsmGenericSymbolList.C            SgBinaryOp.C                               SgFormatItemList.C                    SgNewExp.C                              SgSupport.C                                  SgVariantExpression.C 
SgAsmDwarfModule.C                     SgAsmInstruction.C                  SgBitAndOp.C                               SgFormatStatement.C                   SgNode.C                                SgSwitchStatement.C                          SgVariantStatement.C 
SgAsmDwarfMutableType.C                SgAsmInterpretation.C               SgBitAttribute.C                           SgForStatement.C                      SgNotEqualOp.C                          SgSymbol.C                                   SgVarRefExp.C 
SgAsmDwarfNamelist.C                   SgAsmInterpretationList.C           SgBitComplementOp.C                        SgFortranDo.C                         SgNotOp.C                               SgSymbolTable.C                              SgWaitStatement.C 
SgAsmDwarfNamelistItem.C               SgAsmLEEntryPoint.C                 SgBitOrOp.C                                SgFortranIncludeLine.C                SgNullExpression.C                      SgTemplateArgument.C                         SgWarningDirectiveStatement.C 
SgAsmDwarfNamespace.C                  SgAsmLEEntryTable.C                 SgBitXorOp.C                               SgFortranNonblockedDo.C               SgNullifyStatement.C                    SgTemplateArgumentList.C                     SgWcharVal.C 
SgAsmDwarfPackedType.C                 SgAsmLEFileHeader.C                 SgBlockDataStatement.C                     SgFuncDecl_attr.C                     SgNullStatement.C                       SgTemplateDeclaration.C                      SgWhereStatement.C 
SgAsmDwarfPartialUnit.C                SgAsmLENameTable.C                  SgBoolValExp.C                             SgFunctionCallExp.C                   SgOmpAtomicStatement.C                  SgTemplateInstantiationDecl.C                SgWhileStmt.C 
SgAsmDwarfPointerType.C                SgAsmLEPageTable.C                  SgBreakStmt.C                              SgFunctionDeclaration.C               SgOmpBarrierStatement.C                 SgTemplateInstantiationDefn.C                SgWriteStatement.C 
SgAsmDwarfPtrToMemberType.C            SgAsmLEPageTableEntry.C             SgCaseOptionStmt.C                         SgFunctionDefinition.C                SgOmpBodyStatement.C                    SgTemplateInstantiationDirectiveStatement.C  SgXorAssignOp.C 
SgAsmDwarfReferenceType.C              SgAsmLERelocTable.C                 SgCastExp.C                                SgFunctionModifier.C                  SgOmpClauseBodyStatement.C              SgTemplateInstantiationFunctionDecl.C
>>>>>>> Add more modifications to ALLOCATE clause.

  set(libgeneratedIR_SRC
    SgAccessModifier.C
    SgActualArgumentExpression.C
    SgAddOp.C
    SgAddressOfOp.C
    SgAggregateInitializer.C
    SgAliasSymbol.C
    SgAllocateStatement.C
    SgAndAssignOp.C
    SgAndOp.C
    SgArithmeticIfStatement.C
    SgArrayType.C
    SgArrowExp.C
    SgArrowStarOp.C
    SgAsmArmInstruction.C
    SgAsmBasicString.C
    SgAsmBinaryAdd.C
    SgAsmBinaryAddPostupdate.C
    SgAsmBinaryAddPreupdate.C
    SgAsmBinaryAsr.C
    SgAsmBinaryDivide.C
    SgAsmBinaryExpression.C
    SgAsmBinaryLsl.C
    SgAsmBinaryLsr.C
    SgAsmBinaryMod.C
    SgAsmBinaryMultiply.C
    SgAsmBinaryRor.C
    SgAsmBinarySubtract.C
    SgAsmBinarySubtractPostupdate.C
    SgAsmBinarySubtractPreupdate.C
    SgAsmBlock.C
    SgAsmCoffStrtab.C
    SgAsmCoffSymbol.C
    SgAsmCoffSymbolList.C
    SgAsmCoffSymbolTable.C
    SgAsmCommonSubExpression.C
    SgAsmControlFlagsExpression.C
    SgAsmDataStructureDeclaration.C
    SgAsmDOSExtendedHeader.C
    SgAsmDOSFileHeader.C
    SgAsmDoubleFloatValueExpression.C
    SgAsmDwarfAccessDeclaration.C
    SgAsmDwarfArrayType.C
    SgAsmDwarfBaseType.C
    SgAsmDwarfCatchBlock.C
    SgAsmDwarfClassTemplate.C
    SgAsmDwarfClassType.C
    SgAsmDwarfCommonBlock.C
    SgAsmDwarfCommonInclusion.C
    SgAsmDwarfCompilationUnit.C
    SgAsmDwarfCompilationUnitList.C
    SgAsmDwarfCondition.C
    SgAsmDwarfConstant.C
    SgAsmDwarfConstruct.C
    SgAsmDwarfConstructList.C
    SgAsmDwarfConstType.C
    SgAsmDwarfDwarfProcedure.C
    SgAsmDwarfEntryPoint.C
    SgAsmDwarfEnumerationType.C
    SgAsmDwarfEnumerator.C
    SgAsmDwarfFileType.C
    SgAsmDwarfFormalParameter.C
    SgAsmDwarfFormatLabel.C
    SgAsmDwarfFriend.C
    SgAsmDwarfFunctionTemplate.C
    SgAsmDwarfImportedDeclaration.C
    SgAsmDwarfImportedModule.C
    SgAsmDwarfImportedUnit.C
    SgAsmDwarfInformation.C
    SgAsmDwarfInheritance.C
    SgAsmDwarfInlinedSubroutine.C
    SgAsmDwarfInterfaceType.C
    SgAsmDwarfLabel.C
    SgAsmDwarfLexicalBlock.C
    SgAsmDwarfLine.C
    SgAsmDwarfLineList.C
    SgAsmDwarfMacro.C
    SgAsmDwarfMacroList.C
    SgAsmDwarfMember.C
    SgAsmDwarfModule.C
    SgAsmDwarfMutableType.C
    SgAsmDwarfNamelist.C
    SgAsmDwarfNamelistItem.C
    SgAsmDwarfNamespace.C
    SgAsmDwarfPackedType.C
    SgAsmDwarfPartialUnit.C
    SgAsmDwarfPointerType.C
    SgAsmDwarfPtrToMemberType.C
    SgAsmDwarfReferenceType.C
    SgAsmDwarfRestrictType.C
    SgAsmDwarfSetType.C
    SgAsmDwarfSharedType.C
    SgAsmDwarfStringType.C
    SgAsmDwarfStructureType.C
    SgAsmDwarfSubprogram.C
    SgAsmDwarfSubrangeType.C
    SgAsmDwarfSubroutineType.C
    SgAsmDwarfTemplateTypeParameter.C
    SgAsmDwarfTemplateValueParameter.C
    SgAsmDwarfThrownType.C
    SgAsmDwarfTryBlock.C
    SgAsmDwarfTypedef.C
    SgAsmDwarfUnionType.C
    SgAsmDwarfUnknownConstruct.C
    SgAsmDwarfUnspecifiedParameters.C
    SgAsmDwarfUnspecifiedType.C
    SgAsmDwarfUpcRelaxedType.C
    SgAsmDwarfUpcSharedType.C
    SgAsmDwarfUpcStrictType.C
    SgAsmDwarfVariable.C
    SgAsmDwarfVariant.C
    SgAsmDwarfVariantPart.C
    SgAsmDwarfVolatileType.C
    SgAsmDwarfWithStmt.C
    SgAsmElfDynamicEntry.C
    SgAsmElfDynamicEntryList.C
    SgAsmElfDynamicSection.C
    SgAsmElfEHFrameEntryCI.C
    SgAsmElfEHFrameEntryCIList.C
    SgAsmElfEHFrameEntryFD.C
    SgAsmElfEHFrameEntryFDList.C
    SgAsmElfEHFrameSection.C
    SgAsmElfFileHeader.C
    SgAsmElfNoteEntry.C
    SgAsmElfNoteEntryList.C
    SgAsmElfNoteSection.C
    SgAsmElfRelocEntry.C
    SgAsmElfRelocEntryList.C
    SgAsmElfRelocSection.C
    SgAsmElfSection.C
    SgAsmElfSectionTable.C
    SgAsmElfSectionTableEntry.C
    SgAsmElfSegmentTable.C
    SgAsmElfSegmentTableEntry.C
    SgAsmElfSegmentTableEntryList.C
    SgAsmElfStringSection.C
    SgAsmElfStrtab.C
    SgAsmElfSymbol.C
    SgAsmElfSymbolList.C
    SgAsmElfSymbolSection.C
    SgAsmElfSymverDefinedAux.C
    SgAsmElfSymverDefinedAuxList.C
    SgAsmElfSymverDefinedEntry.C
    SgAsmElfSymverDefinedEntryList.C
    SgAsmElfSymverDefinedSection.C
    SgAsmElfSymverEntry.C
    SgAsmElfSymverEntryList.C
    SgAsmElfSymverNeededAux.C
    SgAsmElfSymverNeededAuxList.C
    SgAsmElfSymverNeededEntry.C
    SgAsmElfSymverNeededEntryList.C
    SgAsmElfSymverNeededSection.C
    SgAsmElfSymverSection.C
    SgAsmExecutableFileFormat.C
    SgAsmExpression.C
    SgAsmExprListExp.C
    SgAsmFieldDeclaration.C
    SgAsmFunction.C
    SgAsmGenericDLL.C
    SgAsmGenericDLLList.C
    SgAsmGenericFile.C
    SgAsmGenericFileList.C
    SgAsmGenericFormat.C
    SgAsmGenericHeader.C
    SgAsmGenericHeaderList.C
    SgAsmGenericSection.C
    SgAsmGenericSectionList.C
    SgAsmGenericString.C
    SgAsmGenericStrtab.C
    SgAsmGenericSymbol.C
    SgAsmGenericSymbolList.C
    SgAsmInstruction.C
    SgAsmIntegerValueExpression.C
    SgAsmInterpretation.C
    SgAsmInterpretationList.C
    SgAsmLEEntryPoint.C
    SgAsmLEEntryTable.C
    SgAsmLEFileHeader.C
    SgAsmLENameTable.C
    SgAsmLEPageTable.C
    SgAsmLEPageTableEntry.C
    SgAsmLERelocTable.C
    SgAsmLESection.C
    SgAsmLESectionTable.C
    SgAsmLESectionTableEntry.C
    SgAsmMemoryReferenceExpression.C
    SgAsmNEEntryPoint.C
    SgAsmNEEntryTable.C
    SgAsmNEFileHeader.C
    SgAsmNEModuleTable.C
    SgAsmNENameTable.C
    SgAsmNERelocEntry.C
    SgAsmNERelocTable.C
    SgAsmNESection.C
    SgAsmNESectionTable.C
    SgAsmNESectionTableEntry.C
    SgAsmNEStringTable.C
    SgAsmNode.C
    SgAsmOp.C
    SgAsmOperandList.C
    SgAsmPEExportDirectory.C
    SgAsmPEExportEntry.C
    SgAsmPEExportEntryList.C
    SgAsmPEExportSection.C
    SgAsmPEFileHeader.C
    SgAsmPEImportDirectory.C
    SgAsmPEImportDirectoryList.C
    SgAsmPEImportItem.C
    SgAsmPEImportItemList.C
    SgAsmPEImportSection.C
    SgAsmPERVASizePair.C
    SgAsmPERVASizePairList.C
    SgAsmPESection.C
    SgAsmPESectionTable.C
    SgAsmPESectionTableEntry.C
    SgAsmPEStringSection.C
    SgAsmPowerpcInstruction.C
    SgAsmRegisterReferenceExpression.C
    SgAsmSingleFloatValueExpression.C
    SgAsmStatement.C
    SgAsmStmt.C
    SgAsmStoredString.C
    SgAsmStringStorage.C
    SgAsmType128bitFloat.C
    SgAsmType80bitFloat.C
    SgAsmTypeByte.C
    SgAsmType.C
    SgAsmTypeDoubleFloat.C
    SgAsmTypeDoubleQuadWord.C
    SgAsmTypeDoubleWord.C
    SgAsmTypeQuadWord.C
    SgAsmTypeSingleFloat.C
    SgAsmTypeVector.C
    SgAsmTypeWord.C
    SgAsmUnaryArmSpecialRegisterList.C
    SgAsmUnaryExpression.C
    SgAsmUnaryMinus.C
    SgAsmUnaryPlus.C
    SgAsmUnaryRrx.C
    SgAsmValueExpression.C
    SgAsmVectorValueExpression.C
    SgAsmX86Instruction.C
    SgAssignedGotoStatement.C
    SgAssignInitializer.C
    SgAssignOp.C
    SgAssignStatement.C
    SgAssociateStatement.C
    SgAsteriskShapeExp.C
    SgAttribute.C
    SgAttributeSpecificationStatement.C
    SgBackspaceStatement.C
    SgBaseClass.C
    SgBaseClassModifier.C
    SgBasicBlock.C
    SgBidirectionalGraph.C
    SgBinaryComposite.C
    SgBinaryOp.C
    SgBitAndOp.C
    SgBitAttribute.C
    SgBitComplementOp.C
    SgBitOrOp.C
    SgBitXorOp.C
    SgBlockDataStatement.C
    SgBoolValExp.C
    SgBreakStmt.C
    SgCaseOptionStmt.C
    SgCastExp.C
    SgCatchOptionStmt.C
    SgCatchStatementSeq.C
    SgCharVal.C
    SgClassDeclaration.C
    SgClassDecl_attr.C
    SgClassDefinition.C
    SgClassNameRefExp.C
    SgClassSymbol.C
    SgClassType.C
    SgClinkageDeclarationStatement.C
    SgClinkageEndStatement.C
    SgClinkageStartStatement.C
    SgCloseStatement.C
    SgColonShapeExp.C
    SgCommaOpExp.C
    SgCommonBlock.C
    SgCommonBlockObject.C
    SgCommonSymbol.C
    SgComplexVal.C
    SgComputedGotoStatement.C
    SgConcatenationOp.C
    SgConditionalExp.C
    SgConjugateOp.C
    SgConstructorInitializer.C
    SgConstVolatileModifier.C
    SgContainsStatement.C
    SgContinueStmt.C
    SgC_PreprocessorDirectiveStatement.C
    SgCtorInitializerList.C
    SgDataStatementGroup.C
    SgDataStatementObject.C
    SgDataStatementValue.C
    SgDeadIfDirectiveStatement.C
    SgDeallocateStatement.C
    SgDeclarationModifier.C
    SgDeclarationStatement.C
    SgDefaultOptionStmt.C
    SgDefaultSymbol.C
    SgDefineDirectiveStatement.C
    SgDeleteExp.C
    SgDerivedTypeStatement.C
    SgDesignatedInitializer.C
    SgDimensionObject.C
    SgDirectedGraphEdge.C
    SgDirectory.C
    SgDirectoryList.C
    SgDivAssignOp.C
    SgDivideOp.C
    SgDotExp.C
    SgDotStarOp.C
    SgDoubleVal.C
    SgDoWhileStmt.C
    SgElaboratedTypeModifier.C
    SgElseDirectiveStatement.C
    SgElseifDirectiveStatement.C
    SgElseWhereStatement.C
    SgEmptyDirectiveStatement.C
    SgEndfileStatement.C
    SgEndifDirectiveStatement.C
    SgEntryStatement.C
    SgEnumDeclaration.C
    SgEnumFieldSymbol.C
    SgEnumSymbol.C
    SgEnumType.C
    SgEnumVal.C
    SgEqualityOp.C
    SgEquivalenceStatement.C
    SgErrorDirectiveStatement.C
    SgExponentiationOp.C
    SgExpression.C
    SgExpressionRoot.C
    SgExprListExp.C
    SgExprStatement.C
    SgFile.C
    Sg_File_Info.C
    SgFileList.C
    SgFloatVal.C
    SgFlushStatement.C
    SgForAllStatement.C
    SgForInitStatement.C
    SgFormatItem.C
    SgFormatItemList.C
    SgFormatStatement.C
    SgForStatement.C
    SgFortranDo.C
    SgFortranIncludeLine.C
    SgFortranNonblockedDo.C
    SgFuncDecl_attr.C
    SgFunctionCallExp.C
    SgFunctionDeclaration.C
    SgFunctionDefinition.C
    SgFunctionModifier.C
    SgFunctionParameterList.C
    SgFunctionParameterTypeList.C
    SgFunctionRefExp.C
    SgFunctionSymbol.C
    SgFunctionType.C
    SgFunctionTypeSymbol.C
    SgFunctionTypeTable.C
    SgGlobal.C
    SgGotoStatement.C
    SgGraph.C
    SgGraphEdge.C
    SgGraphEdgeList.C
    SgGraphNode.C
    SgGraphNodeList.C
    SgGreaterOrEqualOp.C
    SgGreaterThanOp.C
    SgIdentDirectiveStatement.C
    SgIfdefDirectiveStatement.C
    SgIfDirectiveStatement.C
    SgIfndefDirectiveStatement.C
    SgIfStmt.C
    SgImagPartOp.C
    SgImplicitStatement.C
    SgImpliedDo.C
    SgImportStatement.C
    SgIncidenceDirectedGraph.C
    SgIncidenceUndirectedGraph.C
    SgIncludeDirectiveStatement.C
    SgIncludeNextDirectiveStatement.C
    SgInitializedName.C
    SgInitializer.C
    SgInquireStatement.C
    SgIntegerDivideOp.C
    SgInterfaceBody.C
    SgInterfaceStatement.C
    SgInterfaceSymbol.C
    SgIntKeyedBidirectionalGraph.C
    SgIntrinsicSymbol.C
    SgIntVal.C
    SgIOItemExpression.C
    SgIorAssignOp.C
    SgIOStatement.C
    SgLabelRefExp.C
    SgLabelStatement.C
    SgLabelSymbol.C
    SgLessOrEqualOp.C
    SgLessThanOp.C
    SgLineDirectiveStatement.C
    SgLinemarkerDirectiveStatement.C
    SgLinkageModifier.C
    SgLocatedNode.C
    SgLocatedNodeSupport.C
    SgLongDoubleVal.C
    SgLongIntVal.C
    SgLongLongIntVal.C
    SgLshiftAssignOp.C
    SgLshiftOp.C
    SgMemberFunctionDeclaration.C
    SgMemberFunctionRefExp.C
    SgMemberFunctionSymbol.C
    SgMemberFunctionType.C
    SgMinusAssignOp.C
    SgMinusMinusOp.C
    SgMinusOp.C
    SgModAssignOp.C
    SgModifier.C
    SgModifierNodes.C
    SgModifierType.C
    SgModOp.C
    SgModuleStatement.C
    SgModuleSymbol.C
    SgMultAssignOp.C
    SgMultiplyOp.C
    SgName.C
    SgNamedType.C
    SgNameGroup.C
    SgNamelistStatement.C
    SgNamespaceAliasDeclarationStatement.C
    SgNamespaceDeclarationStatement.C
    SgNamespaceDefinitionStatement.C
    SgNamespaceSymbol.C
    SgNewExp.C
    SgNode.C
    SgNotEqualOp.C
    SgNotOp.C
    SgNullExpression.C
    SgNullifyStatement.C
    SgNullStatement.C
    SgOmpAtomicStatement.C
    SgOmpBarrierStatement.C
    SgOmpBodyStatement.C
    SgOmpClauseBodyStatement.C
    SgOmpClause.C
    SgOmpCollapseClause.C
    SgOmpCopyinClause.C
    SgOmpCopyprivateClause.C
    SgOmpCriticalStatement.C
    SgOmpDefaultClause.C
    SgOmpDoStatement.C
    SgOmpExpressionClause.C
    SgOmpFirstprivateClause.C
    SgOmpFlushStatement.C
    SgOmpForStatement.C
    SgOmpIfClause.C
    SgOmpLastprivateClause.C
    SgOmpMasterStatement.C
    SgOmpNowaitClause.C
    SgOmpNumThreadsClause.C
    SgOmpOrderedClause.C
    SgOmpOrderedStatement.C
    SgOmpParallelStatement.C
    SgOmpPrivateClause.C
    SgOmpReductionClause.C
    SgOmpScheduleClause.C
    SgOmpSectionsStatement.C
    SgOmpSectionStatement.C
    SgOmpSharedClause.C
    SgOmpSingleStatement.C
    SgOmpTaskStatement.C
    SgOmpTaskwaitStatement.C
    SgOmpThreadprivateStatement.C
    SgOmpUntiedClause.C
    SgOmpVariablesClause.C
    SgOmpWorkshareStatement.C
    SgOpenStatement.C
    SgOptions.C
    SgOrOp.C
    SgParameterStatement.C
    SgPartialFunctionModifierType.C
    SgPartialFunctionType.C
    SgPlusAssignOp.C
    SgPlusPlusOp.C
    SgPntrArrRefExp.C
    SgPointerAssignOp.C
    SgPointerDerefExp.C
    SgPointerMemberType.C
    SgPointerType.C
    SgPragma.C
    SgPragmaDeclaration.C
    SgPrintStatement.C
    SgProcedureHeaderStatement.C
    SgProgramHeaderStatement.C
    SgProject.C
    SgPseudoDestructorRefExp.C
    SgQualifiedName.C
    SgQualifiedNameType.C
    SgReadStatement.C
    SgRealPartOp.C
    SgReferenceType.C
    SgRefExp.C
    SgRenamePair.C
    SgRenameSymbol.C
    SgReturnStmt.C
    SgRewindStatement.C
    SgRshiftAssignOp.C
    SgRshiftOp.C
    SgScopeOp.C
    SgScopeStatement.C
    SgSequenceStatement.C
    SgShortVal.C
    SgSizeOfOp.C
    SgSourceFile.C
    SgSpawnStmt.C
    SgSpecialFunctionModifier.C
    SgStatement.C
    SgStatementExpression.C
    SgStatementFunctionStatement.C
    SgStopOrPauseStatement.C
    SgStorageModifier.C
    SgStringKeyedBidirectionalGraph.C
    SgStringVal.C
    SgSubscriptExpression.C
    SgSubtractOp.C
    SgSupport.C
    SgSwitchStatement.C
    SgSymbol.C
    SgSymbolTable.C
    SgTemplateArgument.C
    SgTemplateArgumentList.C
    SgTemplateDeclaration.C
    SgTemplateInstantiationDecl.C
    SgTemplateInstantiationDefn.C
    SgTemplateInstantiationDirectiveStatement.C
    SgTemplateInstantiationFunctionDecl.C
    SgTemplateInstantiationMemberFunctionDecl.C
    SgTemplateParameter.C
    SgTemplateParameterList.C
    SgTemplateSymbol.C
    SgTemplateType.C
    SgThisExp.C
    SgThrowOp.C
    SgToken.C
    SgTryStmt.C
    SgTypeBool.C
    SgType.C
    SgTypeChar.C
    SgTypeComplex.C
    SgTypeDefault.C
    SgTypedefDeclaration.C
    SgTypedefSeq.C
    SgTypedefSymbol.C
    SgTypedefType.C
    SgTypeDouble.C
    SgTypeEllipse.C
    SgTypeFloat.C
    SgTypeGlobalVoid.C
    SgTypeIdOp.C
    SgTypeImaginary.C
    SgTypeInt.C
    SgTypeLong.C
    SgTypeLongDouble.C
    SgTypeLongLong.C
    SgTypeModifier.C
    SgTypeShort.C
    SgTypeSignedChar.C
    SgTypeSignedInt.C
    SgTypeSignedLong.C
    SgTypeSignedLongLong.C
    SgTypeSignedShort.C
    SgTypeString.C
    SgTypeUnknown.C
    SgTypeUnsignedChar.C
    SgTypeUnsignedInt.C
    SgTypeUnsignedLong.C
    SgTypeUnsignedLongLong.C
    SgTypeUnsignedShort.C
    SgTypeVoid.C
    SgTypeWchar.C
    SgUnaryAddOp.C
    SgUnaryOp.C
    SgUndefDirectiveStatement.C
    SgUndirectedGraphEdge.C
    SgUnknownArrayOrFunctionReference.C
    SgUnknownFile.C
    SgUnparse_Info.C
    SgUnsignedCharVal.C
    SgUnsignedIntVal.C
    SgUnsignedLongLongIntVal.C
    SgUnsignedLongVal.C
    SgUnsignedShortVal.C
    SgUPC_AccessModifier.C
    SgUpcBarrierStatement.C
    SgUpcBlocksizeofExpression.C
    SgUpcElemsizeofExpression.C
    SgUpcFenceStatement.C
    SgUpcForAllStatement.C
    SgUpcLocalsizeofExpression.C
    SgUpcMythread.C
    SgUpcNotifyStatement.C
    SgUpcThreads.C
    SgUpcWaitStatement.C
    SgUserDefinedBinaryOp.C
    SgUserDefinedUnaryOp.C
    SgUseStatement.C
    SgUsingDeclarationStatement.C
    SgUsingDirectiveStatement.C
    SgValueExp.C
    SgVarArgCopyOp.C
    SgVarArgEndOp.C
    SgVarArgOp.C
    SgVarArgStartOneOperandOp.C
    SgVarArgStartOp.C
    SgVariableDeclaration.C
    SgVariableDefinition.C
    SgVariableSymbol.C
    SgVariantExpression.C
    SgVariantStatement.C
    SgVarRefExp.C
    SgWaitStatement.C
    SgWarningDirectiveStatement.C
    SgWcharVal.C
    SgWhereStatement.C
    SgWhileStmt.C
    SgWriteStatement.C
    SgXorAssignOp.C)

  if(VERBOSE)
    message(STATUS "CxxGrammarMetaProgram_SRCS = " ${CxxGrammarMetaProgram_SRCS})
  endif()
  get_target_property(META_EXE_LOC CxxGrammarMetaProgram LOCATION) 

  # define a custom command to call the code generator and generate source/header files
  # Generate the edg-sage connection using ROSETTA
  add_custom_command(
    OUTPUT ${libgeneratedIR_SRC}
    # depend on the code generator and input files
    DEPENDS  CxxGrammarMetaProgram ${GRAMMAR_SOURCE_FILES}
    # We need to copy the input files  to the build tree since 
    # the code generator will output the generated source files into the same directory as where the input files locate
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/Grammar
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/astFileIO
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ROSE_TOP_SRC_DIR}/src/ROSETTA/Grammar/ ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/Grammar/
    # create this directory is needed since a few generated files will be put there        
    COMMAND ${META_EXE_LOC}
    ARGS ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/)

  add_library(libgeneratedIR STATIC ${libgeneratedIR_SRC})
  add_dependencies(libgeneratedIR CxxGrammarMetaProgram)

  # Need to define a mechanism to collect a list of all files (later).
  install(FILES  SgAccessModifier.h SgAsmDwarfRestrictType.h DESTINATION ${INCLUDE_INSTALL_DIR})
endif()
