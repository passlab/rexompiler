#include <omp.h>
#include "libxomp.h" 

struct OUT__1__9585___data 
{
  void *i_p;
}
;
static void OUT__1__9585__(void *__out_argv);

int atom()
{
  int i = 0;
  struct OUT__1__9585___data __out_argv1__9585__;
  __out_argv1__9585__ . i_p = ((void *)(&i));
  XOMP_parallel_start(OUT__1__9585__,&__out_argv1__9585__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/atoms-2.c",5);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/atoms-2.c",9);
  return i;
}

static void OUT__1__9585__(void *__out_argv)
{
  int *i = (int *)(((struct OUT__1__9585___data *)__out_argv) -> i_p);
  if (omp_get_thread_num() == 10) {
     *i = 10;
  }
}
