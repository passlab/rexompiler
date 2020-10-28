/*
 * Dynamic schedule
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "rex_kmp.h" 
int a[20];

void foo(int lower,int upper,int stride)
{
  int i;
{
    int _p_i;
    int __index_;
    int __lower_ = lower;
    int __upper_ = upper + 1;
    int __stride_ = -1 * stride;
    int __last_iter_ = 0;
    __kmpc_dispatch_init_4(0,(__kmpc_global_thread_num(0)),35,__lower_,__upper_,__stride_,1);
{
      while(__kmpc_dispatch_next_4(0,(__kmpc_global_thread_num(0)),&__last_iter_,&__lower_,&__upper_,&__stride_)){
        if (__upper_ < upper + 1) 
          __upper_ = upper + 1;
        for (__index_ = __lower_; __index_ >= __upper_; __index_ += -1 * stride) {
          a[__index_] = __index_ * 2;
          printf("Iteration %2d is carried out by thread %2d\n",__index_,(omp_get_thread_num()));
        }
      }
    }
    __kmpc_barrier(0,(__kmpc_global_thread_num(0)));
  }
}
static void OUT__1__5792__(int *__global_tid,int *__bound_tid,void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  __kmpc_fork_call(0,1,OUT__1__5792__,0);
  return 0;
}

static void OUT__1__5792__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  if (__kmpc_single(0, *__global_tid)) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
    __kmpc_end_single(0, *__global_tid);
  }
  __kmpc_barrier(0, *__global_tid);
  foo(0,20,3);
}
