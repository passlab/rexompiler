/*
test code for loopTiling
by Liao, 6/25/2009
*/
#include "rose.h"
#include <string>
#include <iostream>
#include "commandline_processing.h"

using namespace std;

int main(int argc, char * argv[])

{
  int line;
  int tilesize=1,depth=1;
  // command line processing
  //--------------------------------------------------
  vector<std::string> argvList (argv, argv+argc);
  if (!CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopTiling:","line",line, true)
     || !CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopTiling:","depth",depth, true)
     || !CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopTiling:","tilesize",tilesize, true))
   {
     cout<<"Usage: loopTiling inputFile.c -rose:loopTiling:line <line_number> -rose:loopTiling:depth D -rose:loopTiling:tilesize N"<<endl;
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

  // Tile it
  //--------------------------------------------------
  bool result=false;
  result = SageInterface::loopTiling(forLoop,depth, tilesize);
  ROSE_ASSERT(result != false);

//  AstPostProcessing(project);
  // run all tests
  AstTests::runAllTests(project);

  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

