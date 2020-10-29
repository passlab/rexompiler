/*
* default loop scheduling
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "libxomp.h" 
int a[20];
static void OUT__1__9601__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  XOMP_parallel_start(OUT__1__9601__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor.c",12);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor.c",24);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9601__(void *__out_argv)
{
  if (XOMP_single()) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
  }
  XOMP_barrier();
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(0,1,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) 
//for (i=0;i<20;i+=3)
{
      a[p_index_] = p_index_ * 2;
      printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
    }
  }
}
