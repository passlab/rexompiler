#include <stdio.h>
#include <omp.h>
#include <assert.h>
#include "libxomp.h" 

struct OUT__1__10693___data 
{
  void *sum_p;
  void *num_steps_p;
}
;
static void OUT__1__10693__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int sum = 77;
  int num_steps = 100;
  omp_set_num_threads(4);
  struct OUT__1__10693___data __out_argv1__10693__;
  __out_argv1__10693__ . num_steps_p = ((void *)(&num_steps));
  __out_argv1__10693__ . sum_p = ((void *)(&sum));
  XOMP_parallel_start(OUT__1__10693__,&__out_argv1__10693__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/firstlastprivate.c",12);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/firstlastprivate.c",16);
  printf("sum=%d\n",sum);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10693__(void *__out_argv)
{
  int *sum = (int *)(((struct OUT__1__10693___data *)__out_argv) -> sum_p);
  int *num_steps = (int *)(((struct OUT__1__10693___data *)__out_argv) -> num_steps_p);
  int _p_i;
  int _p_sum =  *sum;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(1, *num_steps,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_sum = _p_sum + p_index_;
  }
  if (p_index_ != p_lower_ && p_index_ >  *num_steps) 
     *sum = _p_sum;
  XOMP_barrier();
}
