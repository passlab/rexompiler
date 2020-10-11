#include <stdio.h>
#include <omp.h>
#include "libxomp.h" 
static void OUT__1__10750__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int mits = 5000;
#ifdef _OPENMP
  XOMP_parallel_start(OUT__1__10750__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/preprocessingInfo.c",7);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/preprocessingInfo.c",11);
#endif
  mits++;
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10750__(void *__out_argv)
{
  if (XOMP_single()) {
    printf("Running using %d threads...\n",(omp_get_num_threads()));
  }
  XOMP_barrier();
}
