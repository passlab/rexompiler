#include "libxomp.h" 

int foo(int x)
{
  int v;
  XOMP_atomic_start();
  v = x;
  XOMP_atomic_end();
  XOMP_atomic_start();
  v = x--;
  XOMP_atomic_end();
  return v;
}

struct OUT__1__9675___data 
{
  void *a_p;
}
;
static void OUT__1__9675__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int a;
  int b;
  int c;
  struct OUT__1__9675___data __out_argv1__9675__;
  __out_argv1__9675__ . a_p = ((void *)(&a));
  XOMP_parallel_start(OUT__1__9675__,&__out_argv1__9675__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/atomic-3.c",16);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/atomic-3.c",22);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9675__(void *__out_argv)
{
  int *a = (int *)(((struct OUT__1__9675___data *)__out_argv) -> a_p);
  XOMP_atomic_start();
   *a = 1;
  XOMP_atomic_end();
  XOMP_atomic_start();
  ( *a)++;
  XOMP_atomic_end();
}
