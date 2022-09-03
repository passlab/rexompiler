#include <stdio.h>
#if defined(_OPENMP)
#include <omp.h>
#endif /* _OPENMP */
#include "libxomp.h" 
static double a[1000];

static void init()
{
  int i = 0;
  i = i + 5;
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(0,999,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      a[p_index_] = ((double )p_index_) / 2.0;
    }
    XOMP_barrier();
  }
}
static void OUT__1__10910__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__10910__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/orphaned-directives.c",22);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/orphaned-directives.c",25);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10910__(void *__out_argv)
{
  init();
}
