#include "sage3basic.h"
#include "InterProcDataFlowAnalysis.h"

void InterProcDataFlowAnalysis::run () {

     bool change;
     int iteration = 0;
    do {
            change = false;
            
            std::vector<SgFunctionDeclaration*> processingOrder;        

            getFunctionDeclarations(processingOrder);
           
            for (SgFunctionDeclaration* funcDecl: processingOrder) {

                change |= runAndCheckIntraProcAnalysis(funcDecl);
            
            }
            
            iteration++;
        } while (change);
        std::cout << "Total Interprocedural iterations: " << iteration << std::endl;
}
