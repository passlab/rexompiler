#include<assert.h> 
#include<omp.h> 
#include<stdio.h> 
#include "rex_kmp.h" 

struct OUT__1__7062___data 
{
  void *i_p;
  void *thread_num_p;
}
;
static void OUT__1__7062__(int *__global_tid,int *__bound_tid,void *__out_argv);

struct OUT__2__7062___data 
{
  void *i_p;
  void *sum_p;
  void *thread_num_p;
}
;
static void OUT__2__7062__(int *__global_tid,int *__bound_tid,void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  int i = 100;
  int sum = 100;
  int thread_num;
  struct OUT__2__7062___data __out_argv2__7062__;
  __out_argv2__7062__ . thread_num_p = ((void *)(&thread_num));
  __out_argv2__7062__ . sum_p = ((void *)(&sum));
  __out_argv2__7062__ . i_p = ((void *)(&i));
  __kmpc_fork_call(0,1,OUT__2__7062__,&__out_argv2__7062__);
  printf("thread num=%d sum =%d\n",thread_num,sum);
  (((void )(sizeof(((sum == i * thread_num + 100?1 : 0))))) , ((
{
    if (sum == i * thread_num + 100) 
      ;
     else 
      __assert_fail("sum == (i*thread_num + 100)","parallel-reduction2.c",18,__PRETTY_FUNCTION__);
  })));
  sum = 100;
  struct OUT__1__7062___data __out_argv1__7062__;
  __out_argv1__7062__ . thread_num_p = ((void *)(&thread_num));
  __out_argv1__7062__ . i_p = ((void *)(&i));
  __kmpc_fork_call(0,1,OUT__1__7062__,&__out_argv1__7062__);
  printf("thread num=%d sum =%d\n",thread_num,sum);
  (((void )(sizeof(((sum != i * thread_num + 100?1 : 0))))) , ((
{
    if (sum != i * thread_num + 100) 
      ;
     else 
      __assert_fail("sum != (i*thread_num + 100)","parallel-reduction2.c",30,__PRETTY_FUNCTION__);
  })));
  return 0;
}

static void OUT__1__7062__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  int *i = (int *)(((struct OUT__1__7062___data *)__out_argv) -> i_p);
  int *thread_num = (int *)(((struct OUT__1__7062___data *)__out_argv) -> thread_num_p);
  int _p_sum;
  if (__kmpc_single(0, *__global_tid)) {{
       *thread_num = omp_get_num_threads();
    }
    __kmpc_end_single(0, *__global_tid);
  }
  __kmpc_barrier(0, *__global_tid);
  _p_sum +=  *i;
}

static void OUT__2__7062__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  int *i = (int *)(((struct OUT__2__7062___data *)__out_argv) -> i_p);
  int *sum = (int *)(((struct OUT__2__7062___data *)__out_argv) -> sum_p);
  int *thread_num = (int *)(((struct OUT__2__7062___data *)__out_argv) -> thread_num_p);
  int _p_sum;
  _p_sum = 0;
  if (__kmpc_single(0, *__global_tid)) {{
       *thread_num = omp_get_num_threads();
    }
    __kmpc_end_single(0, *__global_tid);
  }
  __kmpc_barrier(0, *__global_tid);
  _p_sum +=  *i;
  __kmpc_atomic_start();
   *sum =  *sum + _p_sum;
  __kmpc_atomic_end();
}
