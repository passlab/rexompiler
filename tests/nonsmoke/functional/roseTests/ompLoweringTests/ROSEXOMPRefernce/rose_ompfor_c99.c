/*
* default loop scheduling
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "libxomp.h" 
int a[20];
static void OUT__1__9909__(void *__out_argv);

int main(int argc,char **argv)
{
  int i;
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__9909__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor_c99.c",11);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor_c99.c",22);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9909__(void *__out_argv)
{
  if (XOMP_single()) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
  }
  XOMP_barrier();
{
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(0,1,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      a[p_index_] = p_index_ * 2;
      printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
    }
  }
}
