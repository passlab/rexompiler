// Example ROSE Translator used for testing ROSE infrastructure
#include "rose.h"

int main( int argc, char * argv[] )
   {
#if 0
  // Output the ROSE specific predefined macros.
     outputPredefinedMacros();
#endif

#if 1
       // DQ (3/6/2017): Test API to set frontend and backend options for tools (minimal output from ROSE-based tools).
       // Note that the defaults are for minimal output from ROSE-based tools.
          Rose::global_options.set_frontend_notes(false);
          Rose::global_options.set_frontend_warnings(false);
          Rose::global_options.set_backend_warnings(false);
#endif
        }
#endif

     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

  // DQ (3/20/2017): Test info about mode (code coverage).
     ROSE_ASSERT(SageBuilder::display(SageBuilder::SourcePositionClassificationMode) == "e_sourcePositionTransformation");

  // DQ (3/20/2017): Test this function after EDG/ROSE translation (not required for users).
     SageBuilder::clearScopeStack();

#if 0
  // DQ (3/22/2019): Test this on our regression tests.
     SageInterface::translateToUseCppDeclarations(project);
#endif

#if 0
  // DQ (9/8/2017): Debugging ROSE_ASSERT.
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
  // DQ (12/22/2019): Call multi-file version (instead of generateDOT() function).
  // generateAstGraph(project, 2000);
  // generateDOT ( *project );
     generateDOTforMultipleFile(*project);
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

#if 0
     SgNode::get_globalTypeTable()->print_typetable();
#endif

#if 0
  // DQ (9/8/2017): Debugging ROSE_ASSERT.
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // regenerate the source code and call the vendor 
  // compiler, only backend error code is reported.
  // return backend(project);
     int status = backend(project);

  // DQ (10/21/2020): Adding IR node usage statistics reporting.
  // AstNodeStatistics::IRnodeUsageStatistics();

  // DQ (10/21/2020): Adding performance reporting.
     TimingPerformance::generateReport();
  // TimingPerformance::generateReportToFile(project);
  // TimingPerformance::set_project(SgProject* project);

     return status;
   }
