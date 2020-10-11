#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__10525___data 
{
  void *i_p;
}
;
static void OUT__1__10525__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i = 0;
  struct OUT__1__10525___data __out_argv1__10525__;
  __out_argv1__10525__ . i_p = ((void *)(&i));
  XOMP_parallel_start(OUT__1__10525__,&__out_argv1__10525__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/get_max_threads.c",9);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/get_max_threads.c",16);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10525__(void *__out_argv)
{
  int *i = (int *)(((struct OUT__1__10525___data *)__out_argv) -> i_p);
#ifdef _OPENMP
   *i = omp_get_max_threads();
#endif
  if (XOMP_single()) {
    printf("Hello,world! Max thread =%d\n", *i);
  }
  XOMP_barrier();
}
