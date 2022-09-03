/*
 *  Test the insertion of multiple calls to runtime terminate functions.
 *  The function calls should not share the same statement.
 *  7/25/2011
By C. Liao
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__10585___data 
{
  void *j_p;
}
;
static void OUT__1__10585__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i = 0;
  int j = 0;
  struct OUT__1__10585___data __out_argv1__10585__;
  __out_argv1__10585__ . j_p = ((void *)(&j));
  XOMP_parallel_start(OUT__1__10585__,&__out_argv1__10585__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/multiple_return.c",16);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/multiple_return.c",22);
  if (i) {
    XOMP_terminate(status);
    return 0;
  }
   else {
    XOMP_terminate(status);
    return 0;
  }
}

static void OUT__1__10585__(void *__out_argv)
{
  int *j = (int *)(((struct OUT__1__10585___data *)__out_argv) -> j_p);
  int _p_i;
#ifdef _OPENMP
  _p_i = omp_get_thread_num() +  *j;
#endif
  printf("Hello,world! I am thread %d\n",_p_i);
}
