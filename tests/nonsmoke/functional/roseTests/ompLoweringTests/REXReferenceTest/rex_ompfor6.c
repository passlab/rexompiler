/*
* test #if #else #endif
* from NPB 2.3 C version LU
*  #if defined(_OPENMP) is attached to the for loop,
*  which will be moved under SgOmpForStatement as its body.
*
*  In this case, we want to #if defined(_OPENMP) to go 
*  with the for loop and not be automatically kept in its original place
*  by SageInterface::removeStatement();
*
* Liao 10/28/2010
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "rex_kmp.h" 
int a[20];
static void OUT__1__5794__(int *__global_tid,int *__bound_tid,void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  int i;
  __kmpc_fork_call(0,1,OUT__1__5794__,0);
}

static void OUT__1__5794__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  if (__kmpc_single(0, *__global_tid)) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
    __kmpc_end_single(0, *__global_tid);
  }
  __kmpc_barrier(0, *__global_tid);
{
    int _p_i;
    int __index_;
    int __lower_ = 0;
    int __upper_ = 19;
    int __stride_ = 1;
    int __last_iter_ = 0;
    __kmpc_for_static_init_4(0, *__global_tid,34,&__last_iter_,&__lower_,&__upper_,&__stride_,1,1);
    if (__upper_ > 19) 
      __upper_ = 19;
#if defined(_OPENMP)     
    for (__index_ = __lower_; __index_ <= __upper_; __index_ += 1) 
#else      
#endif      
{
      a[__index_] = __index_ * 2;
      printf("Iteration %2d is carried out by thread %2d\n",__index_,(omp_get_thread_num()));
    }
    __kmpc_for_static_fini(0, *__global_tid);
    __kmpc_barrier(0, *__global_tid);
  }
}
