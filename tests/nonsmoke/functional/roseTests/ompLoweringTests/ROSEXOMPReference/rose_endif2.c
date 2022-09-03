/* A complex case of #if ..#endif
 * Extracted from BOTS
 *
 * Liao 2/4/2010
 * */
#include <stdio.h>
#include "libxomp.h" 
static int par_res;
extern int fib(int );

struct OUT__1__9510___data 
{
  void *n_p;
}
;
static void OUT__1__9510__(void *__out_argv);

void fib0(int n)
{
  struct OUT__1__9510___data __out_argv1__9510__;
  __out_argv1__9510__ . n_p = ((void *)(&n));
  XOMP_parallel_start(OUT__1__9510__,&__out_argv1__9510__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/endif2.c",12);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/endif2.c",17);
#endif
  printf("Fibonacci result for %d is %d\n",n,par_res);
}

static void OUT__1__9510__(void *__out_argv)
{
  int *n = (int *)(((struct OUT__1__9510___data *)__out_argv) -> n_p);
  if (XOMP_single()) {
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
#else
    par_res = fib( *n);
  }
  XOMP_barrier();
}
