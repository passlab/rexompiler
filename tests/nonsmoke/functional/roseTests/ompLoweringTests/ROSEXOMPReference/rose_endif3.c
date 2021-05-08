/* various cases of #if ...#endif
 * */
#include<stdio.h>
#include "libxomp.h" 
static int par_res;
int fib(int ,int );
// Easiest case: the extend of #if ..#endif is explicit as {} is used

int fib0(int n)
{
//#pragma omp parallel
  if (XOMP_single()) {
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
#else
    par_res = fib0(n);
#endif
  }
  XOMP_barrier();
  printf("Fibonacci result for %d is %d\n",n,par_res);
  return par_res;
}
// hard case for ROSE: #endif is attached to printf
// it should be moved to par_res = fib(n)

int fib1(int n)
{
//#pragma omp parallel
  if (XOMP_single()) {
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
#else
    par_res = fib1(n);
  }
  XOMP_barrier();
#endif
//  printf("Fibonacci result for %d is %d\n",n,par_res);
  return par_res;
}
//TODO parallel joins the party

struct OUT__1__9511___data 
{
  void *n_p;
}
;
static void OUT__1__9511__(void *__out_argv);

int fib2(int n)
{
  struct OUT__1__9511___data __out_argv1__9511__;
  __out_argv1__9511__ . n_p = ((void *)(&n));
  XOMP_parallel_start(OUT__1__9511__,&__out_argv1__9511__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/endif3.c",43);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/endif3.c",51);
  return par_res;
}

static void OUT__1__9511__(void *__out_argv)
{
  int *n = (int *)(((struct OUT__1__9511___data *)__out_argv) -> n_p);
  if (XOMP_single()) 
// this bracket is essential  now, since we don't use wave by default to decide the scope of #endif!!
{
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
#else
    par_res = fib2( *n);
#endif
  }
  XOMP_barrier();
}
