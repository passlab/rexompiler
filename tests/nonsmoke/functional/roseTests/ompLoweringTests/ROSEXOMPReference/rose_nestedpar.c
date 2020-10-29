#include<omp.h>
#include <stdio.h>
#include "libxomp.h" 

struct OUT__2__9908___data 
{
  void *s_p;
}
;
static void OUT__2__9908__(void *__out_argv);

void paroutput(char *s)
{
  struct OUT__2__9908___data __out_argv2__9908__;
  __out_argv2__9908__ . s_p = ((void *)(&s));
  XOMP_parallel_start(OUT__2__9908__,&__out_argv2__9908__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar.c",6);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar.c",7);
}
static void OUT__1__9908__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__9908__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar.c",12);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar.c",22);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9908__(void *__out_argv)
{
  paroutput("before single");
  if (XOMP_single()) {
    paroutput("inside single");
  }
  XOMP_barrier();
  paroutput("after single");
}

static void OUT__2__9908__(void *__out_argv)
{
  char **s = (char **)(((struct OUT__2__9908___data *)__out_argv) -> s_p);
  printf("%s\n", *s);
}
