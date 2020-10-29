// Test the handling of pretty function
// There should be a hidden variable declaration inserted under the closest enclosing scope
// Liao 2013-1-10
#include "libxomp.h" 
static void OUT__1__10050__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
//  int i=100,sum=0;
  XOMP_parallel_start(OUT__1__10050__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/prettyfunc.c",7);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/prettyfunc.c",10);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10050__(void *__out_argv)
{
  __PRETTY_FUNCTION__;
}
