#include <stdio.h>
#include <omp.h>
#include <assert.h>
#include "libxomp.h" 

struct OUT__1__10257___data 
{
  void *sum_p;
}
;
static void OUT__1__10257__(void *__out_argv);

struct OUT__2__10257___data 
{
  int i;
  void *sum_p;
}
;
static void OUT__2__10257__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int i = 100;
  int sum = 0;
  struct OUT__2__10257___data __out_argv2__10257__;
  __out_argv2__10257__ . sum_p = ((void *)(&sum));
  __out_argv2__10257__ . i = i;
// with it: 
  XOMP_parallel_start(OUT__2__10257__,&__out_argv2__10257__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/firstprivate.c",9);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/firstprivate.c",13);
  i = 100;
  struct OUT__1__10257___data __out_argv1__10257__;
  __out_argv1__10257__ . sum_p = ((void *)(&sum));
// without it: 
  XOMP_parallel_start(OUT__1__10257__,&__out_argv1__10257__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/firstprivate.c",18);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/firstprivate.c",22);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10257__(void *__out_argv)
{
  int *sum = (int *)(((struct OUT__1__10257___data *)__out_argv) -> sum_p);
  int _p_i;
  int _p_sum;
  _p_sum = 0;
//assert(i != 100);
  _p_sum = _p_sum + _p_i;
  XOMP_atomic_start();
   *sum =  *sum + _p_sum;
  XOMP_atomic_end();
}

static void OUT__2__10257__(void *__out_argv)
{
  int i = (int )(((struct OUT__2__10257___data *)__out_argv) -> i);
  int *sum = (int *)(((struct OUT__2__10257___data *)__out_argv) -> sum_p);
  int _p_i = i;
  int _p_sum;
  _p_sum = 0;
  (((void )(sizeof(((_p_i == 100?1 : 0))))) , ((
{
    if (_p_i == 100) 
      ;
     else 
      __assert_fail("i == 100","firstprivate.c",11,__PRETTY_FUNCTION__);
  })));
  _p_sum = _p_sum + _p_i;
  XOMP_atomic_start();
   *sum =  *sum + _p_sum;
  XOMP_atomic_end();
}
