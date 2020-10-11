#include<assert.h> 
#include<omp.h> 
#include<stdio.h> 
#include "libxomp.h" 

struct OUT__1__10855___data 
{
  void *i_p;
  void *thread_num_p;
}
;
static void OUT__1__10855__(void *__out_argv);

struct OUT__2__10855___data 
{
  void *i_p;
  void *sum_p;
  void *thread_num_p;
}
;
static void OUT__2__10855__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i = 100;
  int sum = 100;
  int thread_num;
  struct OUT__2__10855___data __out_argv2__10855__;
  __out_argv2__10855__ . thread_num_p = ((void *)(&thread_num));
  __out_argv2__10855__ . sum_p = ((void *)(&sum));
  __out_argv2__10855__ . i_p = ((void *)(&i));
  XOMP_parallel_start(OUT__2__10855__,&__out_argv2__10855__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-reduction2.c",9);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-reduction2.c",16);
  printf("thread num=%d sum =%d\n",thread_num,sum);
  (((void )(sizeof(((sum == i * thread_num + 100?1 : 0))))) , ((
{
    if (sum == i * thread_num + 100) 
      ;
     else 
      __assert_fail("sum == (i*thread_num + 100)","parallel-reduction2.c",18,__PRETTY_FUNCTION__);
  })));
  sum = 100;
  struct OUT__1__10855___data __out_argv1__10855__;
  __out_argv1__10855__ . thread_num_p = ((void *)(&thread_num));
  __out_argv1__10855__ . i_p = ((void *)(&i));
  XOMP_parallel_start(OUT__1__10855__,&__out_argv1__10855__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-reduction2.c",21);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-reduction2.c",28);
  printf("thread num=%d sum =%d\n",thread_num,sum);
  (((void )(sizeof(((sum != i * thread_num + 100?1 : 0))))) , ((
{
    if (sum != i * thread_num + 100) 
      ;
     else 
      __assert_fail("sum != (i*thread_num + 100)","parallel-reduction2.c",30,__PRETTY_FUNCTION__);
  })));
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10855__(void *__out_argv)
{
  int *i = (int *)(((struct OUT__1__10855___data *)__out_argv) -> i_p);
  int *thread_num = (int *)(((struct OUT__1__10855___data *)__out_argv) -> thread_num_p);
  int _p_sum;
  if (XOMP_single()) {
     *thread_num = omp_get_num_threads();
  }
  XOMP_barrier();
  _p_sum +=  *i;
}

static void OUT__2__10855__(void *__out_argv)
{
  int *i = (int *)(((struct OUT__2__10855___data *)__out_argv) -> i_p);
  int *sum = (int *)(((struct OUT__2__10855___data *)__out_argv) -> sum_p);
  int *thread_num = (int *)(((struct OUT__2__10855___data *)__out_argv) -> thread_num_p);
  int _p_sum;
  _p_sum = 0;
  if (XOMP_single()) {
     *thread_num = omp_get_num_threads();
  }
  XOMP_barrier();
  _p_sum +=  *i;
  XOMP_atomic_start();
   *sum =  *sum + _p_sum;
  XOMP_atomic_end();
}
