#include <omp.h>
#include "libxomp.h" 

struct OUT__1__10204___data 
{
  void *a_p;
  void *b_p;
}
;
static void OUT__1__10204__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int a;
  int b;
  struct OUT__1__10204___data __out_argv1__10204__;
  __out_argv1__10204__ . b_p = ((void *)(&b));
  __out_argv1__10204__ . a_p = ((void *)(&a));
  XOMP_parallel_start(OUT__1__10204__,&__out_argv1__10204__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/masterSingle.c",7);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/masterSingle.c",13);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10204__(void *__out_argv)
{
  int *a = (int *)(((struct OUT__1__10204___data *)__out_argv) -> a_p);
  int *b = (int *)(((struct OUT__1__10204___data *)__out_argv) -> b_p);
  if (XOMP_master()) 
     *a =  *a + 1;
  if (XOMP_single()) {
     *b =  *b + 1;
  }
  XOMP_barrier();
}
