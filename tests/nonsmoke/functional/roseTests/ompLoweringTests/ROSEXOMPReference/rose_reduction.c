static void *xomp_critical_user_;
/*
1. A local copy of reduction variable  is made and initialized depending on the op(e.g. 0 for +).
2. Compiler finds standard reduction expressions containing op and uses them to update the local copy. 
3. Local copies are reduced into a single value and combined with the original global value.
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__9915___data 
{
  void *sum_p;
  void *total_p;
}
;
static void OUT__1__9915__(void *__out_argv);

struct OUT__2__9915___data 
{
  void *sum_p;
  void *total_p;
}
;
static void OUT__2__9915__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  long sum = 0;
  int total = 100;
  struct OUT__2__9915___data __out_argv2__9915__;
  __out_argv2__9915__ . total_p = ((void *)(&total));
  __out_argv2__9915__ . sum_p = ((void *)(&sum));
  XOMP_parallel_start(OUT__2__9915__,&__out_argv2__9915__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction.c",18);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction.c",21);
  long sum0;
  struct OUT__1__9915___data __out_argv1__9915__;
  __out_argv1__9915__ . total_p = ((void *)(&total));
  __out_argv1__9915__ . sum_p = ((void *)(&sum));
  XOMP_parallel_start(OUT__1__9915__,&__out_argv1__9915__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction.c",24);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction.c",34);
  printf("sum of 1 to %d = %ld\n",total,sum);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9915__(void *__out_argv)
{
  long *sum = (long *)(((struct OUT__1__9915___data *)__out_argv) -> sum_p);
  int *total = (int *)(((struct OUT__1__9915___data *)__out_argv) -> total_p);
  long _p_sum0;
  _p_sum0 = 0;
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(0, *total,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      _p_sum0 = _p_sum0 + p_index_;
    }
    XOMP_barrier();
  }
  XOMP_critical_start(&xomp_critical_user_);
   *sum =  *sum + _p_sum0;
  XOMP_critical_end(&xomp_critical_user_);
}

static void OUT__2__9915__(void *__out_argv)
{
  long *sum = (long *)(((struct OUT__2__9915___data *)__out_argv) -> sum_p);
  int *total = (int *)(((struct OUT__2__9915___data *)__out_argv) -> total_p);
  int _p_i;
  long _p_sum;
  _p_sum = 0;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0, *total,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_sum = _p_sum + p_index_;
  }
  XOMP_atomic_start();
   *sum =  *sum + _p_sum;
  XOMP_atomic_end();
  XOMP_barrier();
}
