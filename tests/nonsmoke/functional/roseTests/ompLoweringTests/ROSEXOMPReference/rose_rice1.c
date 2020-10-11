/*
 * Contributed by Yonghong Yan
 *
 *  A tricky case when a variable is used to define array size
 *  Outliner will have trouble to generate a legal array type based on the variable
 *  within the outlined function
 *
 *  int *N = (int *)(((struct OUT__1__1527___data *)__out_argv) -> N_p);
 *   //wrong! N is a pointer type now
 *   // compiler will complain: size of array has non-integer type 
 *  int (*a)[N] = (int (*)[N])(((struct OUT__1__1527___data *)__out_argv) -> a_p); 
 *
 *  A possible fix is to run constant propagation first before OpenMP translation.a
 *  Or adjust N to be *N: the variable substitution for shared variable did not work on the type node, which might be ignored by AST traversal by default.
 *  Or the entire declaration statement is not searched at all!!
 *
 * 1/12/2009
 * */
#include <omp.h>
#include <stdio.h>
#define N 100000
#include "libxomp.h" 

struct OUT__1__9410___data 
{
  int (*a_p)[100000];
}
;
static void OUT__1__9410__(void *__out_argv);

struct OUT__2__9410___data 
{
  void *nthreads_p;
}
;
static void OUT__2__9410__(void *__out_argv);

int main(int argc,char *argv[])
{
  int status = 0;
  XOMP_init(argc,argv);
  int th_id;
  int nthreads;
  struct OUT__2__9410___data __out_argv2__9410__;
  __out_argv2__9410__ . nthreads_p = ((void *)(&nthreads));
  XOMP_parallel_start(OUT__2__9410__,&__out_argv2__9410__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/rice1.c",25);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/rice1.c",34);
//   int N = 100000;
  int i;
  int a[100000];
  struct OUT__1__9410___data __out_argv1__9410__;
  __out_argv1__9410__ . a_p = &a;
  XOMP_parallel_start(OUT__1__9410__,&__out_argv1__9410__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/rice1.c",39);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/rice1.c",41);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9410__(void *__out_argv)
{
  int (*a)[100000] = (int (*)[100000])(((struct OUT__1__9410___data *)__out_argv) -> a_p);
  int _p_i;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,99999,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    ( *a)[p_index_] = 2 * p_index_;
  }
  XOMP_barrier();
}

static void OUT__2__9410__(void *__out_argv)
{
  int *nthreads = (int *)(((struct OUT__2__9410___data *)__out_argv) -> nthreads_p);
  int _p_th_id;
  _p_th_id = omp_get_thread_num();
  printf("Hello World from thread %d\n",_p_th_id);
  XOMP_barrier();
  if (_p_th_id == 0) {
     *nthreads = omp_get_num_threads();
    printf("There are %d threads\n", *nthreads);
  }
}
