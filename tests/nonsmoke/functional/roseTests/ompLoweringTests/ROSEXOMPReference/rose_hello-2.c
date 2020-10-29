/*
By C. Liao
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

void foo(int *i)
{
   *i = 2;
}
static void OUT__1__9569__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i = 0;
  XOMP_parallel_start(OUT__1__9569__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/hello-2.c",18);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/hello-2.c",26);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9569__(void *__out_argv)
{
  int _p_i;
#ifdef _OPENMP
  _p_i = omp_get_thread_num();
#endif
  foo(&_p_i);
  printf("Hello,world! I am thread %d\n",_p_i);
  _p_i++;
}
