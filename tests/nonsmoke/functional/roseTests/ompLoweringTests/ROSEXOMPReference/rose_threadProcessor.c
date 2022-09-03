#include <stdio.h>
#include <omp.h>
#include <pthread.h>
#include "libxomp.h" 
extern int pthread_num_processors_np();
static void OUT__1__10534__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int tid;
  int procid;
  omp_set_num_threads(4);
  XOMP_parallel_start(OUT__1__10534__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/threadProcessor.c",10);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/threadProcessor.c",16);
  XOMP_terminate(status);
}

static void OUT__1__10534__(void *__out_argv)
{
  int _p_tid;
  int _p_procid;
  _p_tid = omp_get_thread_num();
  _p_procid = pthread_num_processors_np();
  printf("Hello,world.! by thread %d  on processor %d\n",_p_tid,_p_procid);
}
