/*
 * test the simplest case, no variable handling
By C. Liao
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 
static void OUT__1__9568__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__9568__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/hello-1.c",13);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/hello-1.c",16);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9568__(void *__out_argv)
{
  printf("Hello,world!");
}
