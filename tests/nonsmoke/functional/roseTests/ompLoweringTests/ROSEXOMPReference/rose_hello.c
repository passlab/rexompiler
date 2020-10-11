/*
1. how to include header
2. parallel region
3. runtime routine
4. undeterminated execution order
5. control number of threads
By C. Liao
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__9474___data 
{
  void *j_p;
}
;
static void OUT__1__9474__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i = 0;
  int j = 0;
  struct OUT__1__9474___data __out_argv1__9474__;
  __out_argv1__9474__ . j_p = ((void *)(&j));
  XOMP_parallel_start(OUT__1__9474__,&__out_argv1__9474__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/hello.c",19);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/hello.c",25);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9474__(void *__out_argv)
{
  int *j = (int *)(((struct OUT__1__9474___data *)__out_argv) -> j_p);
  int _p_i;
#ifdef _OPENMP
  _p_i = omp_get_thread_num() +  *j;
#endif
  printf("Hello,world! I am thread %d\n",_p_i);
}
