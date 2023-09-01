/*
Winnie:
test code for loopCollapsing
*/
#include "rose.h"
#include <string>
#include <iostream>
#include "commandline_processing.h"

using namespace std;

int main(int argc, char * argv[])

{
  int line;
  int factor =2;
  // command line processing
  //--------------------------------------------------
  vector<std::string> argvList (argv, argv+argc);
  if (!CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopcollapse:","line", line, true)
     || !CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopcollapse:","factor",factor, true))
   {
     cout<<"Usage: loopCollapsing inputFile.c -rose:loopcollapse:line <line_number> -rose:loopcollapse:factor N"<<endl;
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

  //--------------------------------------------------
  bool result=false;
  result = SageInterface::loopCollapsing(forLoop, factor);
  ROSE_ASSERT(result != false);

  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

