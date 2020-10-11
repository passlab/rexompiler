/*
 * Combined parallel for
 * with multiple clauses
 * */
#include "libxomp.h" 

struct OUT__1__10114___data 
{
  int (*a_p)[1000];
  void *sum_p;
}
;
static void OUT__1__10114__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int a[1000];
  int sum;
  struct OUT__1__10114___data __out_argv1__10114__;
  __out_argv1__10114__ . sum_p = ((void *)(&sum));
  __out_argv1__10114__ . a_p = &a;
  XOMP_parallel_start(OUT__1__10114__,&__out_argv1__10114__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallelfor.c",9);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallelfor.c",14);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10114__(void *__out_argv)
{
  int (*a)[1000] = (int (*)[1000])(((struct OUT__1__10114___data *)__out_argv) -> a_p);
  int *sum = (int *)(((struct OUT__1__10114___data *)__out_argv) -> sum_p);
  int _p_i;
  int _p_sum;
  _p_sum = 0;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_ordered_dynamic_init(0,999,1,5);
  if (XOMP_loop_ordered_dynamic_start(0,999,1,5,&p_lower_,&p_upper_)) {
    do {
      for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
        ( *a)[p_index_] = p_index_ * 2;
        _p_sum += p_index_;
      }
    }while (XOMP_loop_ordered_dynamic_next(&p_lower_,&p_upper_));
  }
  XOMP_atomic_start();
   *sum =  *sum + _p_sum;
  XOMP_atomic_end();
  XOMP_loop_end();
}
