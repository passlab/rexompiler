//! PDF generator for AST
#include "rose.h"
using namespace std;
int
main ( int argc, char** argv )
{
  // DQ (4/6/2017): This will not fail if we skip calling ROSE_INITIALIZE (but
  // any warning message using the message looging feature in ROSE will fail).
  ROSE_INITIALIZE;

  vector<string> argvList(argv,argv+argc);
  if (CommandlineProcessing::isOption(argvList, "","--help",false))
  {
    printf("--------------Tool specific instructions for pdfGenerator----\n");
    printf("To dump AST from input files only :  pdfGenerator your_input_file.c\n");
    printf("To dump full AST, including headers: pdfGenerator -rose:convertFullAST your_input_file.c\n");
    printf("---------------end of tool specific instructions --------------\n");
    return 1;
  }
  
  // accept -rose:convertFullAST
  if (CommandlineProcessing::isOption(argvList, "-rose:","convertFullAST",true))
    bool dumpFullAST = true; 

  SgProject* project = frontend(argvList);
  ROSE_ASSERT (project != NULL);

  std::string filename = SageInterface::generateProjectName(project);

  if (project->get_verbose() > 0) {
    cout << "Generating AST tree (" << numberOfNodes() << " nodes) in file " << filename << ".pdf.\n";
  }

  AstPDFGeneration astpdfgen;
  astpdfgen.generateInputFiles(project);
  return 0;
}
