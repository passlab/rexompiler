/*
 * test decremental loop iteration space
 * Liao 9/22/2009
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "libxomp.h" 

struct OUT__1__9654___data 
{
  void *iend_p;
  void *ist_p;
}
;
static void OUT__1__9654__(void *__out_argv);

void foo(int iend,int ist)
{
  int i;
  struct OUT__1__9654___data __out_argv1__9654__;
  __out_argv1__9654__ . ist_p = ((void *)(&ist));
  __out_argv1__9654__ . iend_p = ((void *)(&iend));
  XOMP_parallel_start(OUT__1__9654__,&__out_argv1__9654__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor5.c",12);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor5.c",22);
}

static void OUT__1__9654__(void *__out_argv)
{
  int *iend = (int *)(((struct OUT__1__9654___data *)__out_argv) -> iend_p);
  int *ist = (int *)(((struct OUT__1__9654___data *)__out_argv) -> ist_p);
  if (XOMP_single()) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
  }
  XOMP_barrier();
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default( *iend, *ist,-1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ >= p_upper_; p_index_ += -1) {
      printf("Iteration %d is carried out by thread %d\n",p_index_,(omp_get_thread_num()));
    }
  }
}
