/*Example to show outlining vs. inlining*/
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "libxomp.h" 

void do_sth(int a,int b,int c)
{
}

struct OUT__1__9323___data 
{
  int a;
  void *b_p;
}
;
static void OUT__1__9323__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int a;
  int b;
  int c;
  struct OUT__1__9323___data __out_argv1__9323__;
  __out_argv1__9323__ . b_p = ((void *)(&b));
  __out_argv1__9323__ . a = a;
  XOMP_parallel_start(OUT__1__9323__,&__out_argv1__9323__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/omp1.c",15);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/omp1.c",16);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9323__(void *__out_argv)
{
  int a = (int )(((struct OUT__1__9323___data *)__out_argv) -> a);
  int *b = (int *)(((struct OUT__1__9323___data *)__out_argv) -> b_p);
  int _p_a = a;
  int _p_c;
  do_sth(_p_a, *b,_p_c);
}
