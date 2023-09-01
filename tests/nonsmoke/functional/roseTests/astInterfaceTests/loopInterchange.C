/*
test code for loopInterchange
by Liao, 6/15/2009
*/
#include "rose.h"
#include <string>
#include <iostream>
#include "commandline_processing.h"

using namespace std;

int main(int argc, char * argv[])

{
  int line;
  int order=1,depth=2;
  // command line processing
  //--------------------------------------------------
  vector<std::string> argvList (argv, argv+argc);
  if (!CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopInterchange:","line", line, true)
     || !CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopInterchange:","depth",depth, true)
     || !CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopInterchange:","order",order, true))
   {
     cout<<"Usage: loopInterchange inputFile.c -rose:loopInterchange:line <line_number> -rose:loopInterchange:depth D -rose:loopInterchange:order N"<<endl;
     return 0;
   }

  // Retrieve corresponding SgForStatement from line number
  //--------------------------------------------------
  SgProject *project = frontend (argvList);
  SgForStatement* forLoop = NULL;
  ROSE_ASSERT(project != NULL);
  SgFilePtrList & filelist = project->get_fileList();
  SgFilePtrList::iterator iter= filelist.begin();
  for (;iter!=filelist.end();iter++)
  {
    SgSourceFile* sFile = isSgSourceFile(*iter);
    SgStatement * stmt = SageInterface::getFirstStatementAtLine(sFile, line);
    forLoop = isSgForStatement(stmt);
    if (forLoop != NULL) {
      cout<<"Find a loop from line:"<<line<<endl;
      break;
    } else {
      cout<<"Cannot find a matching target from line:"<<line<<endl;
      return 0;
    }
  }
  // Interchange it
  //--------------------------------------------------
  bool result=false;
  result = SageInterface::loopInterchange(forLoop,depth, order);
  ROSE_ASSERT(result != false);

//  AstPostProcessing(project);
  // run all tests
  AstTests::runAllTests(project);

  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

