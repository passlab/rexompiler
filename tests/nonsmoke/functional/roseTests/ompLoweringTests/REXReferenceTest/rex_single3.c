/*
*/
#include <stdio.h>
#include <omp.h>
#include<assert.h> 
#include "rex_kmp.h" 
int a;
int b;
static void OUT__1__5842__(int *__global_tid,int *__bound_tid,void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  __kmpc_fork_call(0,1,OUT__1__5842__,0);
  return 0;
}

static void OUT__1__5842__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  if (__kmpc_single(0, *__global_tid)) {{
      int num_threads = 2;
    }
    __kmpc_end_single(0, *__global_tid);
  }
  __kmpc_barrier(0, *__global_tid);
  if (__kmpc_single(0, *__global_tid)) {{
      int num_threads = 3;
    }
    __kmpc_end_single(0, *__global_tid);
  }
}
