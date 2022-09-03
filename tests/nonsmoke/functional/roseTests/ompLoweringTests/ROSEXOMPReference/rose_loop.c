#include  <stdio.h>
#define N 100
#include "libxomp.h" 

struct OUT__1__9384___data 
{
  float (*x_p)[100];
  float (*y_p)[100];
  void *a_p;
}
;
static void OUT__1__9384__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  float x[100];
  float y[100];
  float a = 2.0;
// initialize
  for (int i = 0; i < 100; i++) {
    x[i] = i;
    y[i] = 0;
  }
  struct OUT__1__9384___data __out_argv1__9384__;
  __out_argv1__9384__ . a_p = ((void *)(&a));
  __out_argv1__9384__ . y_p = &y;
  __out_argv1__9384__ . x_p = &x;
  XOMP_parallel_start(OUT__1__9384__,&__out_argv1__9384__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/loop.c",10);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/loop.c",14);
  if (y[100 - 1] != (100 - 1) * 2.0) 
    printf("Error: 2*(N-1) != y[N-1]=%f",y[100 - 1]);
  XOMP_terminate(status);
}

static void OUT__1__9384__(void *__out_argv)
{
  float (*x)[100] = (float (*)[100])(((struct OUT__1__9384___data *)__out_argv) -> x_p);
  float (*y)[100] = (float (*)[100])(((struct OUT__1__9384___data *)__out_argv) -> y_p);
  float *a = (float *)(((struct OUT__1__9384___data *)__out_argv) -> a_p);
#pragma omp loop  bind(parallel)
  for (int i = 0; i < 100; ++i) 
    ( *y)[i] =  *a * ( *x)[i] + ( *y)[i];
}
