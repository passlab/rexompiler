#include <omp.h>
#define n 100
#include "libxomp.h" 
int a[100];

struct OUT__1__10141___data 
{
  void *j_p;
}
;
static void OUT__1__10141__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int j;
  j = 0;
  struct OUT__1__10141___data __out_argv1__10141__;
  __out_argv1__10141__ . j_p = ((void *)(&j));
  XOMP_parallel_start(OUT__1__10141__,&__out_argv1__10141__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lastprivate.c",8);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lastprivate.c",12);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10141__(void *__out_argv)
{
  int *j = (int *)(((struct OUT__1__10141___data *)__out_argv) -> j_p);
  int _p_i;
  int _p_j;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(1,100,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_j = _p_j + 1;
    a[p_index_] = a[p_index_] + _p_j;
  }
  if (p_index_ != p_lower_ && p_index_ > 100) 
     *j = _p_j;
  XOMP_barrier();
}
