/*
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include<assert.h> 
#include "rex_kmp.h" 

struct OUT__1__3414___data 
{
  void *i_p;
  void *num_threads_p;
}
;
static void OUT__1__3414__(int *__global_tid,int *__bound_tid,void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  int i = 100;
  int num_threads = 0;
  struct OUT__1__3414___data __out_argv1__3414__;
  __out_argv1__3414__ . num_threads_p = ((void *)(&num_threads));
  __out_argv1__3414__ . i_p = ((void *)(&i));
  __kmpc_fork_call(0,1,OUT__1__3414__,&__out_argv1__3414__);
  (((void )(sizeof(((i == 200?1 : 0))))) , ((
{
    if (i == 200) 
      ;
     else 
      __assert_fail("i == 200","single.c",30,__PRETTY_FUNCTION__);
  })));
  return 0;
}

static void OUT__1__3414__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  int *i = (int *)(((struct OUT__1__3414___data *)__out_argv) -> i_p);
  int *num_threads = (int *)(((struct OUT__1__3414___data *)__out_argv) -> num_threads_p);
  if (__kmpc_single(0, *__global_tid)) {{
       *num_threads = omp_get_num_threads();
      __kmpc_atomic_start();
       *i += 100;
      __kmpc_atomic_end();
    }
    __kmpc_end_single(0, *__global_tid);
  }
  __kmpc_barrier(0, *__global_tid);
  if (__kmpc_single(0, *__global_tid)) {{
       *num_threads = omp_get_num_threads();
    }
    __kmpc_end_single(0, *__global_tid);
  }
}
