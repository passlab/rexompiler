/*
1. A local copy of reduction variable  is made and initialized depending on the op(e.g. 0 for +).
2. Compiler finds standard reduction expressions containing op and uses them to update the local copy. 
3. Local copies are reduced into a single value and combined with the original global value.
*/
#include <stdio.h>
#include <assert.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__10698___data 
{
  void *sum_p;
  void *upper_p;
}
;
static void OUT__1__10698__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  long sum = 0;
  int upper = 100;
  struct OUT__1__10698___data __out_argv1__10698__;
  __out_argv1__10698__ . upper_p = ((void *)(&upper));
  __out_argv1__10698__ . sum_p = ((void *)(&sum));
  XOMP_parallel_start(OUT__1__10698__,&__out_argv1__10698__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction-classic.c",19);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction-classic.c",22);
  printf("sum from 1 to %d is %ld\n",upper,sum);
  (((void )(sizeof(((sum == 5050?1 : 0))))) , ((
{
    if (sum == 5050) 
      ;
     else 
      __assert_fail("sum==5050","reduction-classic.c",24,__PRETTY_FUNCTION__);
  })));
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10698__(void *__out_argv)
{
  long *sum = (long *)(((struct OUT__1__10698___data *)__out_argv) -> sum_p);
  int *upper = (int *)(((struct OUT__1__10698___data *)__out_argv) -> upper_p);
  int _p_i;
  long _p_sum;
  _p_sum = 0;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(1, *upper,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_sum = _p_sum + p_index_;
  }
  XOMP_atomic_start();
   *sum =  *sum + _p_sum;
  XOMP_atomic_end();
  XOMP_barrier();
}
