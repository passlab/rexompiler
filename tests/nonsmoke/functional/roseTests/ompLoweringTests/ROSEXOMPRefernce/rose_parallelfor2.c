// A tricky case
#include <omp.h>
#include "libxomp.h" 

struct OUT__1__10164___data 
{
  int (*a_p)[1000];
}
;
static void OUT__1__10164__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int a[1000];
  struct OUT__1__10164___data __out_argv1__10164__;
  __out_argv1__10164__ . a_p = &a;
  XOMP_parallel_start(OUT__1__10164__,&__out_argv1__10164__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallelfor2.c",6);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallelfor2.c",9);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10164__(void *__out_argv)
{
  int (*a)[1000] = (int (*)[1000])(((struct OUT__1__10164___data *)__out_argv) -> a_p);
  int _p_i;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,999,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    ( *a)[p_index_] = p_index_ * 2;
  }
  XOMP_barrier();
}
