/*single and copyprivate*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "libxomp.h" 
static void OUT__1__10198__(void *__out_argv);

int main(int argc,char *argv[])
{
  int status = 0;
  XOMP_init(argc,argv);
  float x = 0.0;
  int y = 0;
#ifdef _OPENMP
  omp_set_num_threads(4);
#endif
  XOMP_parallel_start(OUT__1__10198__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/copyprivate2.c",15);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/copyprivate2.c",23);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10198__(void *__out_argv)
{
  float _p_x;
  int _p_y;
  if (XOMP_single()) {
    _p_x = 546.0;
    _p_y = 777;
  }
  XOMP_barrier();
  printf("x=%f, y=%d\n",_p_x,_p_y);
}
