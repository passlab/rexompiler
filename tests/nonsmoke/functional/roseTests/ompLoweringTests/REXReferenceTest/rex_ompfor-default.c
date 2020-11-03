/*
* default loop scheduling
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "rex_kmp.h" 
static void OUT__1__4217__(int *__global_tid,int *__bound_tid,void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  int i;
  int j;
  __kmpc_fork_call(0,1,OUT__1__4217__,0);
  return 0;
}

static void OUT__1__4217__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  if (__kmpc_single(0, *__global_tid)) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
    __kmpc_end_single(0, *__global_tid);
  }
  __kmpc_barrier(0, *__global_tid);
{
    int _p_i;
    int _p_j;
    int __index_;
    int __lower_ = 0;
    int __upper_ = 9;
    int __stride_ = 1;
    int __last_iter_ = 0;
    __kmpc_for_static_init_4(0, *__global_tid,34,&__last_iter_,&__lower_,&__upper_,&__stride_,1,1);
    if (__upper_ > 9) 
      __upper_ = 9;
    for (__index_ = __lower_; __index_ <= __upper_; __index_ += 1) {
      _p_j = omp_get_thread_num();
      printf("Iteration %d, by thread %d\n",__index_,_p_j);
    }
    __kmpc_for_static_fini(0, *__global_tid);
    __kmpc_barrier(0, *__global_tid);
  }
}
