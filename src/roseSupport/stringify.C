#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>

// DO NOT EDIT -- This implementation was automatically generated for the enum defined at
// /src/midend/programAnalysis/genericDataflow/simpleAnalyses/taintAnalysis.h line 63
namespace stringify { namespace TaintLattice {
    const char* Vertex(int64_t i) {
        switch (i) {
            case 0L: return "VERTEX_BOTTOM";
            case 1L: return "VERTEX_UNTAINTED";
            case 2L: return "VERTEX_TAINTED";
            default: return "";
        }
    }

    std::string Vertex(int64_t i, const std::string &strip) {
        std::string s = Vertex(i);
        if (s.empty())
            s = "(TaintLattice::Vertex)" + boost::lexical_cast<std::string>(i);
        if (boost::starts_with(s, strip))
            s = s.substr(strip.size());
        return s;
    }

    const std::vector<int64_t>& Vertex() {
        static const int64_t values[] = {
            0L,
            1L,
            2L
        };
        static const std::vector<int64_t> retval(values, values + 3);
        return retval;
    }

}}

namespace Rose {
    std::string stringifyTaintLatticeVertex(int64_t i, const char *strip, bool canonic) {
        std::string retval = stringify::TaintLattice::Vertex(i);
        if (retval.empty()) {
            retval = "(TaintLattice::Vertex)" + boost::lexical_cast<std::string>(i);
        } else {
            if (strip && !strncmp(strip, retval.c_str(), strlen(strip)))
                retval = retval.substr(strlen(strip));
            if (canonic)
                retval = "TaintLattice::Vertex::" + retval;
        }
        return retval;
    }

    const std::vector<int64_t>& stringifyTaintLatticeVertex() {
        return stringify::TaintLattice::Vertex();
    }
}

// DO NOT EDIT -- This implementation was automatically generated for the enum defined at
// /src/frontend/SageIII/rose_attributes_list.h line 133
namespace stringify { namespace PreprocessingInfo {
    const char* RelativePositionType(int64_t i) {
        switch (i) {
            case 0L: return "defaultValue";
            case 1L: return "undef";
            case 2L: return "before";
            case 3L: return "after";
            case 4L: return "inside";
            case 6L: return "before_syntax";
            case 7L: return "after_syntax";
            default: return "";
        }
    }

    std::string RelativePositionType(int64_t i, const std::string &strip) {
        std::string s = RelativePositionType(i);
        if (s.empty())
            s = "(PreprocessingInfo::RelativePositionType)" + boost::lexical_cast<std::string>(i);
        if (boost::starts_with(s, strip))
            s = s.substr(strip.size());
        return s;
    }

    const std::vector<int64_t>& RelativePositionType() {
        static const int64_t values[] = {
            0L,
            1L,
            2L,
            3L,
            4L,
            6L,
            7L
        };
        static const std::vector<int64_t> retval(values, values + 7);
        return retval;
    }

}}

namespace Rose {
    std::string stringifyPreprocessingInfoRelativePositionType(int64_t i, const char *strip, bool canonic) {
        std::string retval = stringify::PreprocessingInfo::RelativePositionType(i);
        if (retval.empty()) {
            retval = "(PreprocessingInfo::RelativePositionType)" + boost::lexical_cast<std::string>(i);
        } else {
            if (strip && !strncmp(strip, retval.c_str(), strlen(strip)))
                retval = retval.substr(strlen(strip));
            if (canonic)
                retval = "PreprocessingInfo::RelativePositionType::" + retval;
        }
        return retval;
    }

    const std::vector<int64_t>& stringifyPreprocessingInfoRelativePositionType() {
        return stringify::PreprocessingInfo::RelativePositionType();
    }
}

// DO NOT EDIT -- This implementation was automatically generated for the enum defined at
// -build/src/frontend/SageIII/Cxx_Grammar.h line 692
namespace stringify {
    const char* VariantT(int64_t i) {
        switch (i) {
            case 1L: return "V_SgAccessModifier";
            case 2L: return "V_SgActualArgumentExpression";
            case 3L: return "V_SgAddOp";
            case 4L: return "V_SgAddressOfOp";
            case 5L: return "V_SgAggregateInitializer";
            case 6L: return "V_SgAliasSymbol";
            case 7L: return "V_SgAllocateStatement";
            case 8L: return "V_SgAndAssignOp";
            case 9L: return "V_SgAndOp";
            case 10L: return "V_SgArithmeticIfStatement";
            case 11L: return "V_SgArrayType";
            case 12L: return "V_SgArrowExp";
            case 13L: return "V_SgArrowStarOp";
            case 14L: return "V_SgAsmOp";
            case 15L: return "V_SgAsmStmt";
            case 16L: return "V_SgAssertStmt";
            case 17L: return "V_SgAssignInitializer";
            case 18L: return "V_SgAssignOp";
            case 19L: return "V_SgAssignStatement";
            case 20L: return "V_SgAssignedGotoStatement";
            case 21L: return "V_SgAssociateStatement";
            case 22L: return "V_SgAsteriskShapeExp";
            case 23L: return "V_SgAttribute";
            case 24L: return "V_SgAttributeSpecificationStatement";
            case 25L: return "V_SgAutoType";
            case 26L: return "V_SgAwaitExpression";
            case 27L: return "V_SgBackspaceStatement";
            case 28L: return "V_SgBaseClass";
            case 29L: return "V_SgExpBaseClass";
            case 30L: return "V_SgBaseClassModifier";
            case 31L: return "V_SgBasicBlock";
            case 32L: return "V_SgBidirectionalGraph";
            case 34L: return "V_SgBinaryOp";
            case 35L: return "V_SgBitAndOp";
            case 36L: return "V_SgBitAttribute";
            case 37L: return "V_SgBitComplementOp";
            case 38L: return "V_SgBitEqvOp";
            case 39L: return "V_SgBitOrOp";
            case 40L: return "V_SgBitXorOp";
            case 41L: return "V_SgBlockDataStatement";
            case 42L: return "V_SgBoolValExp";
            case 43L: return "V_SgBreakStmt";
            case 44L: return "V_SgBracedInitializer";
            case 45L: return "V_SgC_PreprocessorDirectiveStatement";
            case 46L: return "V_SgCaseOptionStmt";
            case 47L: return "V_SgCastExp";
            case 48L: return "V_SgCatchOptionStmt";
            case 49L: return "V_SgCatchStatementSeq";
            case 50L: return "V_SgCharVal";
            case 51L: return "V_SgChar16Val";
            case 52L: return "V_SgChar32Val";
            case 53L: return "V_SgChooseExpression";
            case 54L: return "V_SgClassDecl_attr";
            case 55L: return "V_SgClassDeclaration";
            case 56L: return "V_SgClassDefinition";
            case 57L: return "V_SgClassNameRefExp";
            case 58L: return "V_SgClassSymbol";
            case 59L: return "V_SgClassType";
            case 60L: return "V_SgClinkageDeclarationStatement";
            case 61L: return "V_SgClinkageEndStatement";
            case 62L: return "V_SgClinkageStartStatement";
            case 63L: return "V_SgCloseStatement";
            case 64L: return "V_SgColonShapeExp";
            case 65L: return "V_SgCommaOpExp";
            case 66L: return "V_SgCommonBlock";
            case 67L: return "V_SgCommonBlockObject";
            case 68L: return "V_SgCommonSymbol";
            case 69L: return "V_SgComplexVal";
            case 70L: return "V_SgComprehension";
            case 72L: return "V_SgCompoundAssignOp";
            case 73L: return "V_SgCompoundInitializer";
            case 74L: return "V_SgCompoundLiteralExp";
            case 75L: return "V_SgComputedGotoStatement";
            case 76L: return "V_SgConcatenationOp";
            case 77L: return "V_SgConditionalExp";
            case 78L: return "V_SgConjugateOp";
            case 79L: return "V_SgConstVolatileModifier";
            case 80L: return "V_SgConstructorInitializer";
            case 81L: return "V_SgContainsStatement";
            case 82L: return "V_SgContinueStmt";
            case 83L: return "V_SgCtorInitializerList";
            case 84L: return "V_SgDataStatementGroup";
            case 85L: return "V_SgDataStatementObject";
            case 86L: return "V_SgDataStatementValue";
            case 87L: return "V_SgDeadIfDirectiveStatement";
            case 88L: return "V_SgDeallocateStatement";
            case 89L: return "V_SgDeclarationModifier";
            case 90L: return "V_SgDeclarationScope";
            case 91L: return "V_SgDeclarationStatement";
            case 92L: return "V_SgDeclType";
            case 93L: return "V_SgDefaultOptionStmt";
            case 94L: return "V_SgDefaultSymbol";
            case 95L: return "V_SgDefineDirectiveStatement";
            case 96L: return "V_SgDeleteExp";
            case 97L: return "V_SgDerivedTypeStatement";
            case 98L: return "V_SgDesignatedInitializer";
            case 99L: return "V_SgDictionaryComprehension";
            case 100L: return "V_SgDictionaryExp";
            case 101L: return "V_SgDimensionObject";
            case 103L: return "V_SgDirectedGraphEdge";
            case 105L: return "V_SgDirectory";
            case 106L: return "V_SgDirectoryList";
            case 107L: return "V_SgDivAssignOp";
            case 108L: return "V_SgDivideOp";
            case 109L: return "V_SgDoWhileStmt";
            case 110L: return "V_SgDotExp";
            case 111L: return "V_SgDotStarOp";
            case 112L: return "V_SgDoubleVal";
            case 113L: return "V_SgElaboratedTypeModifier";
            case 114L: return "V_SgElementwiseOp";
            case 115L: return "V_SgElementwiseAddOp";
            case 116L: return "V_SgElementwiseDivideOp";
            case 117L: return "V_SgElementwiseLeftDivideOp";
            case 118L: return "V_SgElementwiseMultiplyOp";
            case 119L: return "V_SgElementwisePowerOp";
            case 120L: return "V_SgElementwiseSubtractOp";
            case 121L: return "V_SgElseDirectiveStatement";
            case 122L: return "V_SgElseWhereStatement";
            case 123L: return "V_SgElseifDirectiveStatement";
            case 124L: return "V_SgEmptyDeclaration";
            case 125L: return "V_SgEmptyDirectiveStatement";
            case 126L: return "V_SgEndfileStatement";
            case 127L: return "V_SgEndifDirectiveStatement";
            case 128L: return "V_SgEntryStatement";
            case 129L: return "V_SgEnumDeclaration";
            case 130L: return "V_SgEnumFieldSymbol";
            case 131L: return "V_SgEnumSymbol";
            case 132L: return "V_SgEnumType";
            case 133L: return "V_SgEnumVal";
            case 134L: return "V_SgEqualityOp";
            case 135L: return "V_SgEquivalenceStatement";
            case 136L: return "V_SgErrorDirectiveStatement";
            case 137L: return "V_SgExecStatement";
            case 138L: return "V_SgExponentiationOp";
            case 139L: return "V_SgExponentiationAssignOp";
            case 140L: return "V_SgExprListExp";
            case 141L: return "V_SgExprStatement";
            case 142L: return "V_SgExpression";
            case 143L: return "V_SgExpressionRoot";
            case 144L: return "V_SgFile";
            case 145L: return "V_SgFileList";
            case 146L: return "V_SgFloatVal";
            case 147L: return "V_SgFloat128Val";
            case 148L: return "V_SgFloat80Val";
            case 149L: return "V_SgFoldExpression";
            case 150L: return "V_SgFlushStatement";
            case 151L: return "V_SgForAllStatement";
            case 152L: return "V_SgForInitStatement";
            case 153L: return "V_SgForStatement";
            case 154L: return "V_SgFormatItem";
            case 155L: return "V_SgFormatItemList";
            case 156L: return "V_SgFormatStatement";
            case 157L: return "V_SgFortranDo";
            case 158L: return "V_SgFortranIncludeLine";
            case 159L: return "V_SgFortranNonblockedDo";
            case 160L: return "V_SgFuncDecl_attr";
            case 161L: return "V_SgFunctionCallExp";
            case 162L: return "V_SgFunctionDeclaration";
            case 163L: return "V_SgFunctionDefinition";
            case 164L: return "V_SgFunctionParameterScope";
            case 165L: return "V_SgFunctionModifier";
            case 166L: return "V_SgFunctionParameterList";
            case 167L: return "V_SgFunctionParameterRefExp";
            case 168L: return "V_SgFunctionParameterTypeList";
            case 169L: return "V_SgFunctionRefExp";
            case 170L: return "V_SgFunctionSymbol";
            case 171L: return "V_SgFunctionType";
            case 172L: return "V_SgFunctionTypeSymbol";
            case 173L: return "V_SgFunctionTypeTable";
            case 174L: return "V_SgTypeTable";
            case 175L: return "V_SgGlobal";
            case 176L: return "V_SgGotoStatement";
            case 177L: return "V_SgGraph";
            case 178L: return "V_SgGraphEdge";
            case 179L: return "V_SgGraphEdgeList";
            case 180L: return "V_SgGraphNode";
            case 181L: return "V_SgGraphNodeList";
            case 182L: return "V_SgGreaterOrEqualOp";
            case 183L: return "V_SgGreaterThanOp";
            case 184L: return "V_SgIOItemExpression";
            case 185L: return "V_SgIOStatement";
            case 186L: return "V_SgIdentDirectiveStatement";
            case 187L: return "V_SgIfDirectiveStatement";
            case 188L: return "V_SgIfStmt";
            case 189L: return "V_SgIfdefDirectiveStatement";
            case 190L: return "V_SgIfndefDirectiveStatement";
            case 191L: return "V_SgImageControlStatement";
            case 192L: return "V_SgImagPartOp";
            case 193L: return "V_SgImplicitStatement";
            case 194L: return "V_SgImpliedDo";
            case 195L: return "V_SgImportStatement";
            case 196L: return "V_SgIncidenceDirectedGraph";
            case 197L: return "V_SgIncidenceUndirectedGraph";
            case 198L: return "V_SgIncludeDirectiveStatement";
            case 199L: return "V_SgIncludeFile";
            case 200L: return "V_SgIncludeNextDirectiveStatement";
            case 201L: return "V_SgInitializedName";
            case 202L: return "V_SgInitializer";
            case 203L: return "V_SgInquireStatement";
            case 204L: return "V_SgIntKeyedBidirectionalGraph";
            case 205L: return "V_SgIntVal";
            case 206L: return "V_SgIntegerDivideOp";
            case 207L: return "V_SgIntegerDivideAssignOp";
            case 208L: return "V_SgInterfaceBody";
            case 209L: return "V_SgHeaderFileBody";
            case 210L: return "V_SgHeaderFileReport";
            case 211L: return "V_SgInterfaceStatement";
            case 212L: return "V_SgInterfaceSymbol";
            case 213L: return "V_SgIntrinsicSymbol";
            case 214L: return "V_SgIsOp";
            case 215L: return "V_SgIsNotOp";
            case 216L: return "V_SgIorAssignOp";
            case 217L: return "V_SgKeyDatumPair";
            case 218L: return "V_SgCudaKernelExecConfig";
            case 219L: return "V_SgCudaKernelCallExp";
            case 220L: return "V_SgLabelRefExp";
            case 221L: return "V_SgLabelStatement";
            case 222L: return "V_SgLabelSymbol";
            case 223L: return "V_SgLambdaCapture";
            case 224L: return "V_SgLambdaCaptureList";
            case 225L: return "V_SgLambdaExp";
            case 226L: return "V_SgLambdaRefExp";
            case 227L: return "V_SgLeftDivideOp";
            case 228L: return "V_SgLessOrEqualOp";
            case 229L: return "V_SgLessThanOp";
            case 230L: return "V_SgLineDirectiveStatement";
            case 231L: return "V_SgLinemarkerDirectiveStatement";
            case 232L: return "V_SgLinkageModifier";
            case 233L: return "V_SgListComprehension";
            case 234L: return "V_SgListExp";
            case 235L: return "V_SgLocatedNode";
            case 236L: return "V_SgLocatedNodeSupport";
            case 237L: return "V_SgLongDoubleVal";
            case 238L: return "V_SgLongIntVal";
            case 239L: return "V_SgLongLongIntVal";
            case 240L: return "V_SgLshiftAssignOp";
            case 241L: return "V_SgLshiftOp";
            case 242L: return "V_SgMagicColonExp";
            case 243L: return "V_SgMatrixExp";
            case 244L: return "V_SgMatrixTransposeOp";
            case 245L: return "V_SgMemberFunctionDeclaration";
            case 246L: return "V_SgMemberFunctionRefExp";
            case 247L: return "V_SgMemberFunctionSymbol";
            case 248L: return "V_SgMemberFunctionType";
            case 249L: return "V_SgMembershipOp";
            case 250L: return "V_SgMicrosoftAttributeDeclaration";
            case 251L: return "V_SgMinusAssignOp";
            case 252L: return "V_SgMinusMinusOp";
            case 253L: return "V_SgMinusOp";
            case 254L: return "V_SgModAssignOp";
            case 255L: return "V_SgModOp";
            case 256L: return "V_SgModifier";
            case 257L: return "V_SgModifierNodes";
            case 258L: return "V_SgModifierType";
            case 259L: return "V_SgModuleStatement";
            case 260L: return "V_SgModuleSymbol";
            case 261L: return "V_SgMultAssignOp";
            case 262L: return "V_SgMultiplyOp";
            case 263L: return "V_SgName";
            case 264L: return "V_SgNameGroup";
            case 265L: return "V_SgNamedType";
            case 266L: return "V_SgNamelistStatement";
            case 267L: return "V_SgNamespaceAliasDeclarationStatement";
            case 268L: return "V_SgNamespaceDeclarationStatement";
            case 269L: return "V_SgNamespaceDefinitionStatement";
            case 270L: return "V_SgNamespaceSymbol";
            case 271L: return "V_SgNaryOp";
            case 272L: return "V_SgNaryBooleanOp";
            case 273L: return "V_SgNaryComparisonOp";
            case 274L: return "V_SgNewExp";
            case 275L: return "V_SgNode";
            case 276L: return "V_SgNoexceptOp";
            case 277L: return "V_SgNotEqualOp";
            case 278L: return "V_SgNotOp";
            case 279L: return "V_SgNonMembershipOp";
            case 280L: return "V_SgNonrealDecl";
            case 281L: return "V_SgNonrealRefExp";
            case 282L: return "V_SgNonrealSymbol";
            case 283L: return "V_SgNonrealType";
            case 284L: return "V_SgNonrealBaseClass";
            case 285L: return "V_SgNullExpression";
            case 286L: return "V_SgNullptrValExp";
            case 287L: return "V_SgNullStatement";
            case 288L: return "V_SgNullifyStatement";
            case 289L: return "V_SgOmpExecStatement";
            case 290L: return "V_SgOmpAtomicStatement";
            case 291L: return "V_SgOmpBarrierStatement";
            case 292L: return "V_SgOmpCriticalStatement";
            case 293L: return "V_SgOmpClauseBodyStatement";
            case 294L: return "V_SgOmpBodyStatement";
            case 295L: return "V_SgOmpClauseStatement";
            case 296L: return "V_SgOmpDoStatement";
            case 297L: return "V_SgOmpFlushStatement";
            case 298L: return "V_SgOmpAllocateStatement";
            case 299L: return "V_SgOmpDeclareSimdStatement";
            case 300L: return "V_SgOmpForSimdStatement";
            case 301L: return "V_SgOmpMasterStatement";
            case 302L: return "V_SgOmpTaskyieldStatement";
            case 303L: return "V_SgOmpMetadirectiveStatement";
            case 304L: return "V_SgOmpOrderedStatement";
            case 305L: return "V_SgOmpOrderedDependStatement";
            case 306L: return "V_SgOmpParallelStatement";
            case 307L: return "V_SgOmpTeamsStatement";
            case 308L: return "V_SgOmpCancellationPointStatement";
            case 309L: return "V_SgOmpDeclareMapperStatement";
            case 310L: return "V_SgOmpCancelStatement";
            case 311L: return "V_SgOmpTaskgroupStatement";
            case 312L: return "V_SgOmpDepobjStatement";
            case 313L: return "V_SgOmpDistributeStatement";
            case 314L: return "V_SgOmpLoopStatement";
            case 315L: return "V_SgOmpScanStatement";
            case 316L: return "V_SgOmpTaskloopStatement";
            case 317L: return "V_SgOmpTargetEnterDataStatement";
            case 318L: return "V_SgOmpTargetExitDataStatement";
            case 319L: return "V_SgOmpSectionStatement";
            case 320L: return "V_SgOmpSectionsStatement";
            case 321L: return "V_SgOmpSingleStatement";
            case 322L: return "V_SgOmpTaskStatement";
            case 323L: return "V_SgOmpTaskwaitStatement";
            case 324L: return "V_SgOmpThreadprivateStatement";
            case 325L: return "V_SgOmpWorkshareStatement";
            case 326L: return "V_SgOmpTargetStatement";
            case 327L: return "V_SgOmpTargetDataStatement";
            case 328L: return "V_SgOmpTargetParallelForStatement";
            case 329L: return "V_SgOmpTargetUpdateStatement";
            case 330L: return "V_SgOmpRequiresStatement";
            case 331L: return "V_SgOmpTargetParallelStatement";
            case 332L: return "V_SgOmpTargetParallelForSimdStatement";
            case 333L: return "V_SgOmpTargetParallelLoopStatement";
            case 334L: return "V_SgOmpTargetSimdStatement";
            case 335L: return "V_SgOmpTargetTeamsStatement";
            case 336L: return "V_SgOmpTargetTeamsDistributeStatement";
            case 337L: return "V_SgOmpTargetTeamsDistributeSimdStatement";
            case 338L: return "V_SgOmpTargetTeamsLoopStatement";
            case 339L: return "V_SgOmpTargetTeamsDistributeParallelForStatement";
            case 340L: return "V_SgOmpTargetTeamsDistributeParallelForSimdStatement";
            case 341L: return "V_SgOmpDistributeSimdStatement";
            case 342L: return "V_SgOmpDistributeParallelForStatement";
            case 343L: return "V_SgOmpDistributeParallelForSimdStatement";
            case 344L: return "V_SgOmpTaskloopSimdStatement";
            case 345L: return "V_SgOmpMasterTaskloopSimdStatement";
            case 346L: return "V_SgOmpParallelMasterTaskloopStatement";
            case 347L: return "V_SgOmpParallelMasterTaskloopSimdStatement";
            case 348L: return "V_SgOmpTeamsDistributeStatement";
            case 349L: return "V_SgOmpTeamsDistributeSimdStatement";
            case 350L: return "V_SgOmpTeamsDistributeParallelForStatement";
            case 351L: return "V_SgOmpTeamsDistributeParallelForSimdStatement";
            case 352L: return "V_SgOmpTeamsLoopStatement";
            case 353L: return "V_SgOmpParallelLoopStatement";
            case 354L: return "V_SgOmpParallelMasterStatement";
            case 355L: return "V_SgOmpMasterTaskloopStatement";
            case 356L: return "V_SgOmpUnrollStatement";
            case 357L: return "V_SgOmpTileStatement";
            case 358L: return "V_SgOmpSimdStatement";
            case 359L: return "V_SgOmpForStatement";
            case 360L: return "V_SgOmpClause";
            case 361L: return "V_SgOmpAllocateClause";
            case 362L: return "V_SgOmpAllocatorClause";
            case 363L: return "V_SgOmpUsesAllocatorsClause";
            case 364L: return "V_SgOmpUsesAllocatorsDefination";
            case 365L: return "V_SgOmpToClause";
            case 366L: return "V_SgOmpFromClause";
            case 367L: return "V_SgOmpThreadsClause";
            case 368L: return "V_SgOmpSimdClause";
            case 369L: return "V_SgOmpBeginClause";
            case 370L: return "V_SgOmpCollapseClause";
            case 371L: return "V_SgOmpCopyinClause";
            case 372L: return "V_SgOmpCopyprivateClause";
            case 373L: return "V_SgOmpDefaultClause";
            case 374L: return "V_SgOmpEndClause";
            case 375L: return "V_SgOmpExpressionClause";
            case 376L: return "V_SgOmpFirstprivateClause";
            case 377L: return "V_SgOmpIfClause";
            case 378L: return "V_SgOmpFinalClause";
            case 379L: return "V_SgOmpPriorityClause";
            case 380L: return "V_SgOmpDeviceClause";
            case 381L: return "V_SgOmpLastprivateClause";
            case 382L: return "V_SgOmpNowaitClause";
            case 383L: return "V_SgOmpReadClause";
            case 384L: return "V_SgOmpWriteClause";
            case 385L: return "V_SgOmpUpdateClause";
            case 386L: return "V_SgOmpDepobjUpdateClause";
            case 387L: return "V_SgOmpDestroyClause";
            case 388L: return "V_SgOmpCaptureClause";
            case 389L: return "V_SgOmpSeqCstClause";
            case 390L: return "V_SgOmpAcqRelClause";
            case 391L: return "V_SgOmpReleaseClause";
            case 392L: return "V_SgOmpAcquireClause";
            case 393L: return "V_SgOmpReverseOffloadClause";
            case 394L: return "V_SgOmpUnifiedAddressClause";
            case 395L: return "V_SgOmpUnifiedSharedMemoryClause";
            case 396L: return "V_SgOmpDynamicAllocatorsClause";
            case 397L: return "V_SgOmpAtomicDefaultMemOrderClause";
            case 398L: return "V_SgOmpExtImplementationDefinedRequirementClause";
            case 399L: return "V_SgOmpRelaxedClause";
            case 400L: return "V_SgOmpParallelClause";
            case 401L: return "V_SgOmpSectionsClause";
            case 402L: return "V_SgOmpForClause";
            case 403L: return "V_SgOmpTaskgroupClause";
            case 404L: return "V_SgOmpNumThreadsClause";
            case 405L: return "V_SgOmpNumTeamsClause";
            case 406L: return "V_SgOmpGrainsizeClause";
            case 407L: return "V_SgOmpDetachClause";
            case 408L: return "V_SgOmpNumTasksClause";
            case 409L: return "V_SgOmpNogroupClause";
            case 410L: return "V_SgOmpHintClause";
            case 411L: return "V_SgOmpOrderClause";
            case 412L: return "V_SgOmpDistScheduleClause";
            case 413L: return "V_SgOmpBindClause";
            case 414L: return "V_SgOmpNontemporalClause";
            case 415L: return "V_SgOmpInclusiveClause";
            case 416L: return "V_SgOmpExclusiveClause";
            case 417L: return "V_SgOmpIsDevicePtrClause";
            case 418L: return "V_SgOmpUseDevicePtrClause";
            case 419L: return "V_SgOmpUseDeviceAddrClause";
            case 420L: return "V_SgOmpThreadLimitClause";
            case 421L: return "V_SgOmpOrderedClause";
            case 422L: return "V_SgOmpPrivateClause";
            case 423L: return "V_SgOmpReductionClause";
            case 424L: return "V_SgOmpInReductionClause";
            case 425L: return "V_SgOmpTaskReductionClause";
            case 426L: return "V_SgOmpDefaultmapClause";
            case 427L: return "V_SgOmpScheduleClause";
            case 428L: return "V_SgOmpSharedClause";
            case 429L: return "V_SgOmpUntiedClause";
            case 430L: return "V_SgOmpMergeableClause";
            case 431L: return "V_SgOmpVariablesClause";
            case 432L: return "V_SgOmpMapClause";
            case 433L: return "V_SgOmpSafelenClause";
            case 434L: return "V_SgOmpSimdlenClause";
            case 435L: return "V_SgOmpLinearClause";
            case 436L: return "V_SgOmpUniformClause";
            case 437L: return "V_SgOmpAlignedClause";
            case 438L: return "V_SgOmpProcBindClause";
            case 439L: return "V_SgOmpAtomicClause";
            case 440L: return "V_SgOmpInbranchClause";
            case 441L: return "V_SgOmpNotinbranchClause";
            case 442L: return "V_SgOmpDependClause";
            case 443L: return "V_SgOmpAffinityClause";
            case 444L: return "V_SgOmpWhenClause";
            case 445L: return "V_SgOmpFullClause";
            case 446L: return "V_SgOmpPartialClause";
            case 447L: return "V_SgOmpSizesClause";
            case 448L: return "V_SgOpenclAccessModeModifier";
            case 449L: return "V_SgOpenStatement";
            case 450L: return "V_SgOptions";
            case 451L: return "V_SgOrOp";
            case 452L: return "V_SgParameterStatement";
            case 453L: return "V_SgPartialFunctionModifierType";
            case 454L: return "V_SgPartialFunctionType";
            case 455L: return "V_SgPassStatement";
            case 456L: return "V_SgPlusAssignOp";
            case 457L: return "V_SgPlusPlusOp";
            case 458L: return "V_SgPntrArrRefExp";
            case 459L: return "V_SgPointerAssignOp";
            case 460L: return "V_SgPointerDerefExp";
            case 461L: return "V_SgPointerMemberType";
            case 462L: return "V_SgPointerType";
            case 463L: return "V_SgPowerOp";
            case 464L: return "V_SgPragma";
            case 465L: return "V_SgPragmaDeclaration";
            case 466L: return "V_SgPrintStatement";
            case 467L: return "V_SgProcedureHeaderStatement";
            case 468L: return "V_SgProgramHeaderStatement";
            case 469L: return "V_SgProject";
            case 470L: return "V_SgPseudoDestructorRefExp";
            case 471L: return "V_SgQualifiedName";
            case 472L: return "V_SgQualifiedNameType";
            case 473L: return "V_SgRangeExp";
            case 474L: return "V_SgRangeBasedForStatement";
            case 475L: return "V_SgReadStatement";
            case 476L: return "V_SgRealPartOp";
            case 477L: return "V_SgRefExp";
            case 478L: return "V_SgReferenceType";
            case 479L: return "V_SgRenamePair";
            case 480L: return "V_SgRenameSymbol";
            case 481L: return "V_SgReturnStmt";
            case 482L: return "V_SgRewindStatement";
            case 483L: return "V_SgRshiftAssignOp";
            case 484L: return "V_SgRshiftOp";
            case 485L: return "V_SgRvalueReferenceType";
            case 486L: return "V_SgScopeOp";
            case 487L: return "V_SgScopeStatement";
            case 488L: return "V_SgSequenceStatement";
            case 489L: return "V_SgSetComprehension";
            case 490L: return "V_SgShortVal";
            case 491L: return "V_SgSIMDBinaryOp";
            case 492L: return "V_SgSIMDAddOp";
            case 493L: return "V_SgSIMDSubOp";
            case 494L: return "V_SgSIMDMulOp";
            case 495L: return "V_SgSIMDDivOp";
            case 496L: return "V_SgSIMDFmaOp";
            case 497L: return "V_SgSIMDLoad";
            case 498L: return "V_SgSIMDBroadcast";
            case 499L: return "V_SgSIMDStore";
            case 500L: return "V_SgSIMDPartialStore";
            case 501L: return "V_SgSIMDScalarStore";
            case 502L: return "V_SgSIMDGather";
            case 503L: return "V_SgSIMDExplicitGather";
            case 504L: return "V_SgSIMDScatter";
            case 505L: return "V_SgSizeOfOp";
            case 506L: return "V_SgAlignOfOp";
            case 507L: return "V_SgSourceFile";
            case 508L: return "V_SgSpaceshipOp";
            case 509L: return "V_SgSpawnStmt";
            case 510L: return "V_SgSyncAllStatement";
            case 511L: return "V_SgSyncImagesStatement";
            case 512L: return "V_SgSyncMemoryStatement";
            case 513L: return "V_SgSyncTeamStatement";
            case 514L: return "V_SgLockStatement";
            case 515L: return "V_SgUnlockStatement";
            case 516L: return "V_SgProcessControlStatement";
            case 517L: return "V_SgSpecialFunctionModifier";
            case 518L: return "V_SgStatement";
            case 519L: return "V_SgStaticAssertionDeclaration";
            case 520L: return "V_SgStmtDeclarationStatement";
            case 521L: return "V_SgStatementExpression";
            case 522L: return "V_SgStatementFunctionStatement";
            case 524L: return "V_SgStorageModifier";
            case 525L: return "V_SgStringConversion";
            case 526L: return "V_SgStringKeyedBidirectionalGraph";
            case 527L: return "V_SgStringVal";
            case 528L: return "V_SgStructureModifier";
            case 529L: return "V_SgSubscriptExpression";
            case 530L: return "V_SgSubtractOp";
            case 531L: return "V_SgSupport";
            case 532L: return "V_SgSwitchStatement";
            case 533L: return "V_SgSymbol";
            case 534L: return "V_SgSymbolTable";
            case 535L: return "V_SgTemplateArgument";
            case 536L: return "V_SgTemplateArgumentList";
            case 537L: return "V_SgTemplateDeclaration";
            case 538L: return "V_SgTemplateClassDeclaration";
            case 539L: return "V_SgTemplateClassSymbol";
            case 540L: return "V_SgTemplateFunctionDeclaration";
            case 541L: return "V_SgTemplateFunctionRefExp";
            case 542L: return "V_SgTemplateFunctionSymbol";
            case 543L: return "V_SgTemplateMemberFunctionDeclaration";
            case 544L: return "V_SgTemplateMemberFunctionRefExp";
            case 545L: return "V_SgTemplateMemberFunctionSymbol";
            case 546L: return "V_SgTemplateTypedefDeclaration";
            case 547L: return "V_SgTemplateTypedefSymbol";
            case 548L: return "V_SgTemplateVariableDeclaration";
            case 549L: return "V_SgTemplateVariableSymbol";
            case 550L: return "V_SgTemplateClassDefinition";
            case 551L: return "V_SgTemplateFunctionDefinition";
            case 552L: return "V_SgTemplateInstantiationDecl";
            case 553L: return "V_SgTemplateInstantiationDefn";
            case 554L: return "V_SgTemplateInstantiationDirectiveStatement";
            case 555L: return "V_SgTemplateInstantiationFunctionDecl";
            case 556L: return "V_SgTemplateInstantiationMemberFunctionDecl";
            case 557L: return "V_SgTemplateInstantiationTypedefDeclaration";
            case 558L: return "V_SgTemplateParameter";
            case 559L: return "V_SgTemplateParameterVal";
            case 560L: return "V_SgTemplateParameterList";
            case 561L: return "V_SgTemplateSymbol";
            case 562L: return "V_SgTemplateType";
            case 563L: return "V_SgThisExp";
            case 564L: return "V_SgTypeTraitBuiltinOperator";
            case 565L: return "V_SgSuperExp";
            case 566L: return "V_SgThrowOp";
            case 567L: return "V_SgToken";
            case 568L: return "V_SgTryStmt";
            case 569L: return "V_SgTupleExp";
            case 570L: return "V_SgType";
            case 571L: return "V_SgTypeBool";
            case 572L: return "V_SgTypeChar";
            case 573L: return "V_SgTypeChar16";
            case 574L: return "V_SgTypeChar32";
            case 575L: return "V_SgTypeComplex";
            case 576L: return "V_SgTypeDefault";
            case 577L: return "V_SgTypeExpression";
            case 578L: return "V_SgTypeLabel";
            case 579L: return "V_SgTypeDouble";
            case 580L: return "V_SgTypeEllipse";
            case 581L: return "V_SgTypeFixed";
            case 582L: return "V_SgTypeFloat";
            case 583L: return "V_SgTypeFloat128";
            case 584L: return "V_SgTypeFloat80";
            case 585L: return "V_SgTypeGlobalVoid";
            case 586L: return "V_SgTypeIdOp";
            case 587L: return "V_SgTypeImaginary";
            case 588L: return "V_SgTypeInt";
            case 589L: return "V_SgTypeLong";
            case 590L: return "V_SgTypeLongDouble";
            case 591L: return "V_SgTypeLongLong";
            case 592L: return "V_SgTypeModifier";
            case 593L: return "V_SgTypeMatrix";
            case 594L: return "V_SgTypeTuple";
            case 595L: return "V_SgTypeNullptr";
            case 596L: return "V_SgTypeOfType";
            case 597L: return "V_SgTypeShort";
            case 598L: return "V_SgTypeSigned128bitInteger";
            case 599L: return "V_SgTypeSignedChar";
            case 600L: return "V_SgTypeSignedInt";
            case 601L: return "V_SgTypeSignedLong";
            case 602L: return "V_SgTypeSignedLongLong";
            case 603L: return "V_SgTypeSignedShort";
            case 604L: return "V_SgTypeString";
            case 605L: return "V_SgTypeUnknown";
            case 606L: return "V_SgTypeUnsigned128bitInteger";
            case 607L: return "V_SgTypeUnsignedChar";
            case 608L: return "V_SgTypeUnsignedInt";
            case 609L: return "V_SgTypeUnsignedLong";
            case 610L: return "V_SgTypeUnsignedLongLong";
            case 611L: return "V_SgTypeUnsignedShort";
            case 612L: return "V_SgTypeVoid";
            case 613L: return "V_SgTypeWchar";
            case 614L: return "V_SgTypedefDeclaration";
            case 615L: return "V_SgTypedefSeq";
            case 616L: return "V_SgTypedefSymbol";
            case 617L: return "V_SgTypedefType";
            case 618L: return "V_SgUPC_AccessModifier";
            case 619L: return "V_SgUnaryAddOp";
            case 620L: return "V_SgUnaryOp";
            case 621L: return "V_SgUndefDirectiveStatement";
            case 622L: return "V_SgUndirectedGraphEdge";
            case 623L: return "V_SgUnknownArrayOrFunctionReference";
            case 624L: return "V_SgUnknownFile";
            case 626L: return "V_SgUnparse_Info";
            case 627L: return "V_SgUnsignedCharVal";
            case 628L: return "V_SgUnsignedIntVal";
            case 629L: return "V_SgUnsignedLongLongIntVal";
            case 630L: return "V_SgUnsignedLongVal";
            case 631L: return "V_SgUnsignedShortVal";
            case 632L: return "V_SgUpcBarrierStatement";
            case 633L: return "V_SgUpcBlocksizeofExpression";
            case 634L: return "V_SgUpcElemsizeofExpression";
            case 635L: return "V_SgUpcFenceStatement";
            case 636L: return "V_SgUpcForAllStatement";
            case 637L: return "V_SgUpcLocalsizeofExpression";
            case 638L: return "V_SgUpcMythread";
            case 639L: return "V_SgUpcNotifyStatement";
            case 640L: return "V_SgUpcThreads";
            case 641L: return "V_SgUpcWaitStatement";
            case 642L: return "V_SgUseStatement";
            case 643L: return "V_SgUserDefinedBinaryOp";
            case 644L: return "V_SgUserDefinedUnaryOp";
            case 645L: return "V_SgUsingDeclarationStatement";
            case 646L: return "V_SgUsingDirectiveStatement";
            case 647L: return "V_SgValueExp";
            case 648L: return "V_SgVarArgCopyOp";
            case 649L: return "V_SgVarArgEndOp";
            case 650L: return "V_SgVarArgOp";
            case 651L: return "V_SgVarArgStartOneOperandOp";
            case 652L: return "V_SgVarArgStartOp";
            case 653L: return "V_SgVarRefExp";
            case 654L: return "V_SgVariableDeclaration";
            case 655L: return "V_SgVariableDefinition";
            case 656L: return "V_SgVariableSymbol";
            case 657L: return "V_SgVariantExpression";
            case 658L: return "V_SgVariantStatement";
            case 659L: return "V_SgVoidVal";
            case 660L: return "V_SgWaitStatement";
            case 661L: return "V_SgWarningDirectiveStatement";
            case 662L: return "V_SgWithStatement";
            case 663L: return "V_SgWcharVal";
            case 664L: return "V_SgWhereStatement";
            case 665L: return "V_SgWhileStmt";
            case 666L: return "V_SgWriteStatement";
            case 667L: return "V_SgXorAssignOp";
            case 668L: return "V_SgYieldExpression";
            case 669L: return "V_Sg_File_Info";
            case 670L: return "V_SgTypeCAFTeam";
            case 671L: return "V_SgCAFWithTeamStatement";
            case 672L: return "V_SgCAFCoExpression";
            case 673L: return "V_SgCallExpression";
            case 674L: return "V_SgTypeCrayPointer";
            case 675L: return "V_SgClassExp";
            case 677L: return "V_SgNumVariants";
            default: return "";
        }
    }

    std::string VariantT(int64_t i, const std::string &strip) {
        std::string s = VariantT(i);
        if (s.empty())
            s = "(VariantT)" + boost::lexical_cast<std::string>(i);
        if (boost::starts_with(s, strip))
            s = s.substr(strip.size());
        return s;
    }

    const std::vector<int64_t>& VariantT() {
        static const int64_t values[] = {
            1L,
            2L,
            3L,
            4L,
            5L,
            6L,
            7L,
            8L,
            9L,
            10L,
            11L,
            12L,
            13L,
            14L,
            15L,
            16L,
            17L,
            18L,
            19L,
            20L,
            21L,
            22L,
            23L,
            24L,
            25L,
            26L,
            27L,
            28L,
            29L,
            30L,
            31L,
            32L,
            34L,
            35L,
            36L,
            37L,
            38L,
            39L,
            40L,
            41L,
            42L,
            43L,
            44L,
            45L,
            46L,
            47L,
            48L,
            49L,
            50L,
            51L,
            52L,
            53L,
            54L,
            55L,
            56L,
            57L,
            58L,
            59L,
            60L,
            61L,
            62L,
            63L,
            64L,
            65L,
            66L,
            67L,
            68L,
            69L,
            70L,
            72L,
            73L,
            74L,
            75L,
            76L,
            77L,
            78L,
            79L,
            80L,
            81L,
            82L,
            83L,
            84L,
            85L,
            86L,
            87L,
            88L,
            89L,
            90L,
            91L,
            92L,
            93L,
            94L,
            95L,
            96L,
            97L,
            98L,
            99L,
            100L,
            101L,
            103L,
            105L,
            106L,
            107L,
            108L,
            109L,
            110L,
            111L,
            112L,
            113L,
            114L,
            115L,
            116L,
            117L,
            118L,
            119L,
            120L,
            121L,
            122L,
            123L,
            124L,
            125L,
            126L,
            127L,
            128L,
            129L,
            130L,
            131L,
            132L,
            133L,
            134L,
            135L,
            136L,
            137L,
            138L,
            139L,
            140L,
            141L,
            142L,
            143L,
            144L,
            145L,
            146L,
            147L,
            148L,
            149L,
            150L,
            151L,
            152L,
            153L,
            154L,
            155L,
            156L,
            157L,
            158L,
            159L,
            160L,
            161L,
            162L,
            163L,
            164L,
            165L,
            166L,
            167L,
            168L,
            169L,
            170L,
            171L,
            172L,
            173L,
            174L,
            175L,
            176L,
            177L,
            178L,
            179L,
            180L,
            181L,
            182L,
            183L,
            184L,
            185L,
            186L,
            187L,
            188L,
            189L,
            190L,
            191L,
            192L,
            193L,
            194L,
            195L,
            196L,
            197L,
            198L,
            199L,
            200L,
            201L,
            202L,
            203L,
            204L,
            205L,
            206L,
            207L,
            208L,
            209L,
            210L,
            211L,
            212L,
            213L,
            214L,
            215L,
            216L,
            217L,
            218L,
            219L,
            220L,
            221L,
            222L,
            223L,
            224L,
            225L,
            226L,
            227L,
            228L,
            229L,
            230L,
            231L,
            232L,
            233L,
            234L,
            235L,
            236L,
            237L,
            238L,
            239L,
            240L,
            241L,
            242L,
            243L,
            244L,
            245L,
            246L,
            247L,
            248L,
            249L,
            250L,
            251L,
            252L,
            253L,
            254L,
            255L,
            256L,
            257L,
            258L,
            259L,
            260L,
            261L,
            262L,
            263L,
            264L,
            265L,
            266L,
            267L,
            268L,
            269L,
            270L,
            271L,
            272L,
            273L,
            274L,
            275L,
            276L,
            277L,
            278L,
            279L,
            280L,
            281L,
            282L,
            283L,
            284L,
            285L,
            286L,
            287L,
            288L,
            289L,
            290L,
            291L,
            292L,
            293L,
            294L,
            295L,
            296L,
            297L,
            298L,
            299L,
            300L,
            301L,
            302L,
            303L,
            304L,
            305L,
            306L,
            307L,
            308L,
            309L,
            310L,
            311L,
            312L,
            313L,
            314L,
            315L,
            316L,
            317L,
            318L,
            319L,
            320L,
            321L,
            322L,
            323L,
            324L,
            325L,
            326L,
            327L,
            328L,
            329L,
            330L,
            331L,
            332L,
            333L,
            334L,
            335L,
            336L,
            337L,
            338L,
            339L,
            340L,
            341L,
            342L,
            343L,
            344L,
            345L,
            346L,
            347L,
            348L,
            349L,
            350L,
            351L,
            352L,
            353L,
            354L,
            355L,
            356L,
            357L,
            358L,
            359L,
            360L,
            361L,
            362L,
            363L,
            364L,
            365L,
            366L,
            367L,
            368L,
            369L,
            370L,
            371L,
            372L,
            373L,
            374L,
            375L,
            376L,
            377L,
            378L,
            379L,
            380L,
            381L,
            382L,
            383L,
            384L,
            385L,
            386L,
            387L,
            388L,
            389L,
            390L,
            391L,
            392L,
            393L,
            394L,
            395L,
            396L,
            397L,
            398L,
            399L,
            400L,
            401L,
            402L,
            403L,
            404L,
            405L,
            406L,
            407L,
            408L,
            409L,
            410L,
            411L,
            412L,
            413L,
            414L,
            415L,
            416L,
            417L,
            418L,
            419L,
            420L,
            421L,
            422L,
            423L,
            424L,
            425L,
            426L,
            427L,
            428L,
            429L,
            430L,
            431L,
            432L,
            433L,
            434L,
            435L,
            436L,
            437L,
            438L,
            439L,
            440L,
            441L,
            442L,
            443L,
            444L,
            445L,
            446L,
            447L,
            448L,
            449L,
            450L,
            451L,
            452L,
            453L,
            454L,
            455L,
            456L,
            457L,
            458L,
            459L,
            460L,
            461L,
            462L,
            463L,
            464L,
            465L,
            466L,
            467L,
            468L,
            469L,
            470L,
            471L,
            472L,
            473L,
            474L,
            475L,
            476L,
            477L,
            478L,
            479L,
            480L,
            481L,
            482L,
            483L,
            484L,
            485L,
            486L,
            487L,
            488L,
            489L,
            490L,
            491L,
            492L,
            493L,
            494L,
            495L,
            496L,
            497L,
            498L,
            499L,
            500L,
            501L,
            502L,
            503L,
            504L,
            505L,
            506L,
            507L,
            508L,
            509L,
            510L,
            511L,
            512L,
            513L,
            514L,
            515L,
            516L,
            517L,
            518L,
            519L,
            520L,
            521L,
            522L,
            524L,
            525L,
            526L,
            527L,
            528L,
            529L,
            530L,
            531L,
            532L,
            533L,
            534L,
            535L,
            536L,
            537L,
            538L,
            539L,
            540L,
            541L,
            542L,
            543L,
            544L,
            545L,
            546L,
            547L,
            548L,
            549L,
            550L,
            551L,
            552L,
            553L,
            554L,
            555L,
            556L,
            557L,
            558L,
            559L,
            560L,
            561L,
            562L,
            563L,
            564L,
            565L,
            566L,
            567L,
            568L,
            569L,
            570L,
            571L,
            572L,
            573L,
            574L,
            575L,
            576L,
            577L,
            578L,
            579L,
            580L,
            581L,
            582L,
            583L,
            584L,
            585L,
            586L,
            587L,
            588L,
            589L,
            590L,
            591L,
            592L,
            593L,
            594L,
            595L,
            596L,
            597L,
            598L,
            599L,
            600L,
            601L,
            602L,
            603L,
            604L,
            605L,
            606L,
            607L,
            608L,
            609L,
            610L,
            611L,
            612L,
            613L,
            614L,
            615L,
            616L,
            617L,
            618L,
            619L,
            620L,
            621L,
            622L,
            623L,
            624L,
            626L,
            627L,
            628L,
            629L,
            630L,
            631L,
            632L,
            633L,
            634L,
            635L,
            636L,
            637L,
            638L,
            639L,
            640L,
            641L,
            642L,
            643L,
            644L,
            645L,
            646L,
            647L,
            648L,
            649L,
            650L,
            651L,
            652L,
            653L,
            654L,
            655L,
            656L,
            657L,
            658L,
            659L,
            660L,
            661L,
            662L,
            663L,
            664L,
            665L,
            666L,
            667L,
            668L,
            669L,
            670L,
            671L,
            672L,
            673L,
            674L,
            675L,
            677L
        };
        static const std::vector<int64_t> retval(values, values + 670);
        return retval;
    }

}

namespace Rose {
    std::string stringifyVariantT(int64_t i, const char *strip, bool canonic) {
        std::string retval = stringify::VariantT(i);
        if (retval.empty()) {
            retval = "(VariantT)" + boost::lexical_cast<std::string>(i);
        } else {
            if (strip && !strncmp(strip, retval.c_str(), strlen(strip)))
                retval = retval.substr(strlen(strip));
            if (canonic)
                retval = "VariantT::" + retval;
        }
        return retval;
    }

    const std::vector<int64_t>& stringifyVariantT() {
        return stringify::VariantT();
    }
}

