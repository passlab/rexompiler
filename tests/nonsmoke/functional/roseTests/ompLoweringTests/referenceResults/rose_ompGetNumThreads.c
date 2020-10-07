/*
Contributed by pranav@ics.forth.gr
4/14/2010
*/
#include <stdio.h>
#include <omp.h>
// The non-existence of omp.h is essential to repeat the original bug
//#include <omp.h>
#include "libxomp.h" 

struct OUT__1__10581___data 
{
  void *k_p;
}
;
static void OUT__1__10581__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int k;
  struct OUT__1__10581___data __out_argv1__10581__;
  __out_argv1__10581__ . k_p = ((void *)(&k));
  XOMP_parallel_start(OUT__1__10581__,&__out_argv1__10581__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompGetNumThreads.c",16);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompGetNumThreads.c",23);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10581__(void *__out_argv)
{
  int *k = (int *)(((struct OUT__1__10581___data *)__out_argv) -> k_p);
  if (XOMP_master()) {
     *k = omp_get_num_threads();
    printf("Number of Threads requested = %i\n", *k);
  }
}
