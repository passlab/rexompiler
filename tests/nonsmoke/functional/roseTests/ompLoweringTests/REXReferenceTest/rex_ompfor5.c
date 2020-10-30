/*
 * test decremental loop iteration space
 * Liao 9/22/2009
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "rex_kmp.h" 

struct OUT__1__5793___data 
{
  void *iend_p;
  void *ist_p;
}
;
static void OUT__1__5793__(int *__global_tid,int *__bound_tid,void *__out_argv);

void foo(int iend,int ist)
{
  int i;
  struct OUT__1__5793___data __out_argv1__5793__;
  __out_argv1__5793__ . ist_p = ((void *)(&ist));
  __out_argv1__5793__ . iend_p = ((void *)(&iend));
  __kmpc_fork_call(0,1,OUT__1__5793__,&__out_argv1__5793__);
}

static void OUT__1__5793__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  int *iend = (int *)(((struct OUT__1__5793___data *)__out_argv) -> iend_p);
  int *ist = (int *)(((struct OUT__1__5793___data *)__out_argv) -> ist_p);
  if (__kmpc_single(0, *__global_tid)) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
    __kmpc_end_single(0, *__global_tid);
  }
  __kmpc_barrier(0, *__global_tid);
{
    int _p_i;
    int __index_;
    int __lower_ =  *iend;
    int __upper_ =  *ist;
    int __stride_ = -1;
    int __last_iter_ = 0;
    __kmpc_for_static_init_4(0, *__global_tid,34,&__last_iter_,&__lower_,&__upper_,&__stride_,-1,1);
    if (__upper_ <  *ist) 
      __upper_ =  *ist;
    for (__index_ = __lower_; __index_ >= __upper_; __index_ += -1) {
      printf("Iteration %d is carried out by thread %d\n",__index_,(omp_get_thread_num()));
    }
    __kmpc_for_static_fini(0, *__global_tid);
  }
}
