#include <stdio.h>
#include <omp.h>
#define LARGE_NUMBER 10
//#define LARGE_NUMBER 10000000
#include "libxomp.h" 
double item[10];

void process(double input)
{
  printf("processing %f by thread %d\n",input,(omp_get_thread_num()));
}

struct OUT__1__10172___data 
{
  int i;
}
;
static void OUT__1__10172__(void *__out_argv);

struct OUT__2__10172___data 
{
  int i;
}
;
static void OUT__2__10172__(void *__out_argv);
static void OUT__3__10172__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__3__10172__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/task_untied3.c",13);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/task_untied3.c",29);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10172__(void *__out_argv)
{
  int i = (int )(((struct OUT__1__10172___data *)__out_argv) -> i);
  int _p_i = i;
  process(item[_p_i]);
}

static void OUT__2__10172__(void *__out_argv)
{
  int i = (int )(((struct OUT__2__10172___data *)__out_argv) -> i);
  int _p_i = i;
  for (_p_i = 0; _p_i < 10; _p_i++) {
    struct OUT__1__10172___data __out_argv1__10172__;
    __out_argv1__10172__ . i = _p_i;
    XOMP_task(OUT__1__10172__,&__out_argv1__10172__,0,sizeof(struct OUT__1__10172___data ),4,1,0);
  }
}

static void OUT__3__10172__(void *__out_argv)
{
  if (XOMP_single()) {
    int i;
    printf("Using %d threads.\n",(omp_get_num_threads()));
    struct OUT__2__10172___data __out_argv2__10172__;
    __out_argv2__10172__ . i = i;
    XOMP_task(OUT__2__10172__,&__out_argv2__10172__,0,sizeof(struct OUT__2__10172___data ),4,1,1);
  }
  XOMP_barrier();
}
