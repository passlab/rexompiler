/*
What will happen if the loop indices inside an omp for is not private?
by C. Liao
 */
#include <stdio.h>
#ifdef _OPENMP 
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__9811___data 
{
  int (*a_p)[1000][1000];
}
;
static void OUT__1__9811__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int a[1000][1000];
  int i;
  int j;
  struct OUT__1__9811___data __out_argv1__9811__;
  __out_argv1__9811__ . a_p = &a;
  XOMP_parallel_start(OUT__1__9811__,&__out_argv1__9811__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/privatej.c",15);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/privatej.c",18);
  for (i = 0; i < 1000; i++) 
    for (j = 0; j < 1000; j++) 
      if (a[i][j] != 999) 
        printf("a[%d][%d]!=999\n",i,j);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9811__(void *__out_argv)
{
  int (*a)[1000][1000] = (int (*)[1000][1000])(((struct OUT__1__9811___data *)__out_argv) -> a_p);
  int _p_i;
  int _p_j;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,999,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    for (_p_j = 0; _p_j < 1000; _p_j++) 
      ( *a)[p_index_][_p_j] = 999;
  }
  XOMP_barrier();
}
