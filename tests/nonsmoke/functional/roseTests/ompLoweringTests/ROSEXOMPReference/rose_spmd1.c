/*
1. manual control over work sharing
*/
#ifdef _OPENMP
#include <omp.h>
#endif 
#include <stdio.h>
#define N 20
#include "libxomp.h" 
int a[20];
static void OUT__1__9427__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int id;
  int Nthrds;
  int istart;
  int iend;
  XOMP_parallel_start(OUT__1__9427__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/spmd1.c",15);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/spmd1.c",32);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9427__(void *__out_argv)
{
  int _p_i;
  int _p_id;
  int _p_Nthrds;
  int _p_istart;
  int _p_iend;
/* each thread get self ID and total number of threads
       then decide which portion of work to do
       */
  _p_id = omp_get_thread_num();
  _p_Nthrds = omp_get_num_threads();
  _p_istart = _p_id * 20 / _p_Nthrds;
  _p_iend = (_p_id + 1) * 20 / _p_Nthrds;
//  for (i=0;i<20;i++)
  for (_p_i = _p_istart; _p_i < _p_iend; _p_i++) {
    a[_p_i] = _p_i * 2;
    printf("Iteration %2d is carried out by thread %2d\n",_p_i,(omp_get_thread_num()));
  }
}
