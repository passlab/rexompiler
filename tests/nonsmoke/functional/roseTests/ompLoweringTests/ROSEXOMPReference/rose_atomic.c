#include <omp.h>
#include "libxomp.h" 

struct OUT__1__9579___data 
{
  void *a_p;
}
;
static void OUT__1__9579__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int a;
  struct OUT__1__9579___data __out_argv1__9579__;
  __out_argv1__9579__ . a_p = ((void *)(&a));
  XOMP_parallel_start(OUT__1__9579__,&__out_argv1__9579__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/atomic.c",7);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/atomic.c",11);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9579__(void *__out_argv)
{
  int *a = (int *)(((struct OUT__1__9579___data *)__out_argv) -> a_p);
  XOMP_atomic_start();
   *a += 1;
  XOMP_atomic_end();
}
