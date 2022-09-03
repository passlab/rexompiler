/*
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "libxomp.h" 
float x;
int y;
static void OUT__1__9705__(void *__out_argv);

int main(int argc,char *argv[])
{
  int status = 0;
  XOMP_init(argc,argv);
#ifdef _OPENMP
  omp_set_num_threads(4);
#endif
  x = 1.0;
  y = 1;
  XOMP_parallel_start(OUT__1__9705__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/private.c",18);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/private.c",21);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9705__(void *__out_argv)
{
  float _p_x;
  printf("x=%f, y=%d\n",_p_x,y);
}
