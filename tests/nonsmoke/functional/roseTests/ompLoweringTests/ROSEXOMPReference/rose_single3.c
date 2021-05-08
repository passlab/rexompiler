/*
*/
#include <stdio.h>
#include <omp.h>
#include<assert.h> 
#include "libxomp.h" 
int a;
int b;
static void OUT__1__9635__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__9635__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/single3.c",12);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/single3.c",24);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9635__(void *__out_argv)
{
  if (XOMP_single()) {
    int num_threads = 2;
  }
  XOMP_barrier();
  if (XOMP_single()) {
    int num_threads = 3;
  }
}
