#include <stdio.h>
#include <omp.h>
#include "libxomp.h" 
static long num_steps = 100000;
double step;
/* Liao 1/29/2010, reduced to 2 since Omni runtime will claim if it exceeds the 
 * number of cores within a machine */
#define NUM_THREADS 2

struct OUT__1__10213___data 
{
  void *i_p;
  void *id_p;
  void *x_p;
  double (*sum_p)[2];
}
;
static void OUT__1__10213__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int id;
  double x;
  double pi;
  double sum[2];
  step = 1.0 / ((double )num_steps);
  omp_set_num_threads(2);
  struct OUT__1__10213___data __out_argv1__10213__;
  __out_argv1__10213__ . sum_p = &sum;
  __out_argv1__10213__ . x_p = ((void *)(&x));
  __out_argv1__10213__ . id_p = ((void *)(&id));
  __out_argv1__10213__ . i_p = ((void *)(&i));
  XOMP_parallel_start(OUT__1__10213__,&__out_argv1__10213__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/falsesharing.c",16);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/falsesharing.c",23);
  for ((i = 0 , pi = 0.0); i < 2; i++) 
    pi += sum[i] * step;
  printf("PI=%f\n",pi);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10213__(void *__out_argv)
{
  int *i = (int *)(((struct OUT__1__10213___data *)__out_argv) -> i_p);
  int *id = (int *)(((struct OUT__1__10213___data *)__out_argv) -> id_p);
  double *x = (double *)(((struct OUT__1__10213___data *)__out_argv) -> x_p);
  double (*sum)[2] = (double (*)[2])(((struct OUT__1__10213___data *)__out_argv) -> sum_p);
   *id = omp_get_thread_num();
  for (( *i =  *id , ( *sum)[ *id] = 0.0); ( *i) < num_steps;  *i =  *i + 2) {
     *x = (( *i) + 0.5) * step;
    ( *sum)[ *id] += 4.0 / (1.0 +  *x *  *x);
  }
}
