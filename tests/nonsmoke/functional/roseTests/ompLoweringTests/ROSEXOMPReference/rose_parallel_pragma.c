// A test case with third party pragmas inside a parallel region
#include "libxomp.h" 
extern void quicksort();
static void OUT__1__10514__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__10514__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_pragma.c",6);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_pragma.c",15);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10514__(void *__out_argv)
{
  
#pragma intel omp taskq
{
    
#pragma intel omp task
{
      quicksort();
    }
  }
}
