// Test if/num_threads clause handling
#include <assert.h>
#include <stdio.h>
#include <omp.h>
#include "rex_kmp.h" 
static void OUT__1__7122__(int *__global_tid,int *__bound_tid,void *__out_argv);
static void OUT__2__7122__(int *__global_tid,int *__bound_tid,void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  int i = 0;
  if (i == 0) {
    __kmpc_push_num_threads(0,(__kmpc_global_thread_num(0)),3);
    __kmpc_fork_call(0,1,OUT__2__7122__,0);
  }
   else {
    int __global_tid;
    __global_tid = __kmpc_global_thread_num(0);
    OUT__2__7122__(&__global_tid,0,0);
  }
  if (i != 0) {
    __kmpc_push_num_threads(0,(__kmpc_global_thread_num(0)),3);
    __kmpc_fork_call(0,1,OUT__1__7122__,0);
  }
   else {
    int __global_tid;
    __global_tid = __kmpc_global_thread_num(0);
    OUT__1__7122__(&__global_tid,0,0);
  }
  return 0;
}

static void OUT__1__7122__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  if (__kmpc_single(0, *__global_tid)) {{
      (((void )(sizeof(((omp_get_num_threads() == 1?1 : 0))))) , ((
{
        if (omp_get_num_threads() == 1) 
          ;
         else 
          __assert_fail("omp_get_num_threads() == 1","parallel-numthreads.c",22,__PRETTY_FUNCTION__);
      })));
    }
    __kmpc_end_single(0, *__global_tid);
  }
  __kmpc_barrier(0, *__global_tid);
  printf("Mutual exclusive output 2.\n");
}

static void OUT__2__7122__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  if (__kmpc_single(0, *__global_tid)) {{
      (((void )(sizeof(((omp_get_num_threads() == 3?1 : 0))))) , ((
{
        if (omp_get_num_threads() == 3) 
          ;
         else 
          __assert_fail("omp_get_num_threads() == 3","parallel-numthreads.c",13,__PRETTY_FUNCTION__);
      })));
    }
    __kmpc_end_single(0, *__global_tid);
  }
  __kmpc_barrier(0, *__global_tid);
  printf("Mutual exclusive output 1.\n");
}
