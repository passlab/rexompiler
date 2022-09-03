/*
* Decremental loop iteration, 
* Default loop scheduling
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "libxomp.h" 
int a[20];

struct OUT__1__9652___data 
{
  void *j_p;
}
;
static void OUT__1__9652__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int j = 100;
  struct OUT__1__9652___data __out_argv1__9652__;
  __out_argv1__9652__ . j_p = ((void *)(&j));
  XOMP_parallel_start(OUT__1__9652__,&__out_argv1__9652__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor3.c",14);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor3.c",26);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9652__(void *__out_argv)
{
  int *j = (int *)(((struct OUT__1__9652___data *)__out_argv) -> j_p);
  if (XOMP_single()) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
  }
  XOMP_barrier();
{
    int _p_i;
    int _p_j =  *j;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(19,0,-3,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ >= p_upper_; p_index_ += -3) {
      a[p_index_] = p_index_ * 2 + _p_j;
      printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
    }
    if (p_index_ != p_lower_ && p_index_ < 0) 
       *j = _p_j;
  }
}
