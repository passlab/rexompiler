/*
 * Dynamic schedule
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "libxomp.h" 
int a[20];

void foo(int lower,int upper,int stride)
{
  int i;
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_dynamic_init(lower,upper + 1,-1 * stride,1);
    if (XOMP_loop_dynamic_start(lower,upper + 1,-1 * stride,1,&p_lower_,&p_upper_)) {
      do {
        for (p_index_ = p_lower_; p_index_ >= p_upper_; p_index_ += -1 * stride) {
          a[p_index_] = p_index_ * 2;
          printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
        }
      }while (XOMP_loop_dynamic_next(&p_lower_,&p_upper_));
    }
    XOMP_loop_end();
  }
}
static void OUT__1__9653__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__9653__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor4.c",24);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor4.c",29);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9653__(void *__out_argv)
{
  if (XOMP_single()) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
  }
  XOMP_barrier();
  foo(0,20,3);
}
