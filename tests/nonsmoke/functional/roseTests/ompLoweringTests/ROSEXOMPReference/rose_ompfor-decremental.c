/*
 * default, decremental
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "libxomp.h" 

void foo(int lower,int upper,int stride)
{
  int i;
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(lower,upper + 1,-1 * stride,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ >= p_upper_; p_index_ += -1 * stride) {
      printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
    }
    XOMP_barrier();
  }
}
static void OUT__1__10802__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__10802__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor-decremental.c",22);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor-decremental.c",27);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10802__(void *__out_argv)
{
  if (XOMP_single()) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
  }
  XOMP_barrier();
  foo(9,- 1,3);
}
