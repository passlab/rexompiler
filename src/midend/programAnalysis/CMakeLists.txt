add_subdirectory(CallGraphAnalysis)
add_subdirectory(OAWrap)

# DQ (11/25/2009) Comment out under Windows.
if(WIN32)
  message("Commented out use of OpenAnalysis in Windows.")
else()
  add_subdirectory(CFG)
endif()

add_subdirectory(bitvectorDataflow)
add_subdirectory(VirtualFunctionAnalysis)
add_subdirectory(valuePropagation)
add_subdirectory(defUseAnalysis)
add_subdirectory(dominanceAnalysis)
add_subdirectory(staticInterproceduralSlicing)
add_subdirectory(pointerAnal)
add_subdirectory(variableRenaming)
add_subdirectory(genericDataflow)
add_subdirectory(dataflowAnalysis)


if (NOT enable-internalFrontendDevelopment)
 add_library(midend_pa OBJECT
   defUseAnalysis/DefUseAnalysisAbstract.cpp
   defUseAnalysis/DefUseAnalysis.cpp
   defUseAnalysis/GlobalVarAnalysis.cpp
   defUseAnalysis/LivenessAnalysis.cpp
   defUseAnalysis/dfaToDot.cpp
   defUseAnalysis/DefUseAnalysis_perFunction.cpp
   valuePropagation/ValueAnnot.C
   valuePropagation/ArrayAnnot.C
   valuePropagation/ValuePropagate.C
   variableRenaming/VariableRenaming.C
   OAWrap/OAWrap.C
   OAWrap/SAGE2OA.C
   CallGraphAnalysis/CallGraph.C
   CallGraphAnalysis/ClassHierarchyGraph.C
   staticInterproceduralSlicing/MergedDependenceGraph.C
   staticInterproceduralSlicing/ControlFlowGraph.C
   staticInterproceduralSlicing/FunctionDependenceGraph.C
   staticInterproceduralSlicing/ControlDependenceGraph.C
   staticInterproceduralSlicing/DependenceGraph.C
   staticInterproceduralSlicing/EDefUse.C
   staticInterproceduralSlicing/DataDependenceGraph.C
   staticInterproceduralSlicing/SlicingInfo.C
   staticInterproceduralSlicing/CreateSlice.C
   staticInterproceduralSlicing/CreateSliceSet.C
   staticInterproceduralSlicing/SystemDependenceGraph.C
   staticInterproceduralSlicing/DefUseExtension.C
   CFG/CFG_ROSE.C
   pointerAnal/PtrAnalCFG.C
   pointerAnal/PtrAnal.C
   bitvectorDataflow/DataFlowAnalysis.C
   bitvectorDataflow/ReachingDefinition.C
   bitvectorDataflow/DefUseChain.C
   VirtualFunctionAnalysis/VirtualFunctionAnalysis.C
   VirtualFunctionAnalysis/PtrAliasAnalysis.C
   VirtualFunctionAnalysis/IntraProcAliasAnalysis.C
   VirtualFunctionAnalysis/IntraProcDataFlowAnalysis.C
   VirtualFunctionAnalysis/InterProcDataFlowAnalysis.C)

  add_dependencies(midend_pa rosetta_generated)
endif()
