/*
 * Parsing OpenMP pragma text
by Liao, 9/17/2008
Last Modified: 9/19/2008
*/
#include "rose.h"
#include "RoseAst.h"
#include <iostream>
#include <string>
#include "ompSupport.h"
using namespace std;
using namespace OmpSupport;

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);

  AstTests::runAllTests(project);

//  visitorTraversal myvisitor;
//  myvisitor.traverseInputFiles(project,preorder);
#if 0  // used to trigger issue outliner-32
  SgGlobal * global =  SageInterface::getFirstGlobalScope(project);
  SgSourceFile* originalSourceFile = TransformationSupport::getSourceFile(global); 
  // check this first, before doing any ast post processing
  checkPhysicalSourcePosition(originalSourceFile);

  AstPostProcessing (originalSourceFile);
#endif 
#if 0
  RoseAst ast(project);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgNode* node = *i; //cout<<"We are here:"<<(*i)->class_name()<<endl;
    if (SageInterface::isOmpStatement(node))
    {
      SgStatement* stmt = isSgStatement(node);
      string stmtstr= stmt->unparseToString();
      istringstream istr(stmtstr); 

      char firstline[512]; 
      istr.getline(firstline, 512, '\n');
      cout<<firstline<<endl;
    }

  }
#endif  
  return backend(project);
}
