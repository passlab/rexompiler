#include<stdio.h> 
#include "libxomp.h" 

void do_sth()
{
  printf("hello.\n");
}
static void OUT__1__9685__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__9685__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/barrier.c",9);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/barrier.c",14);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9685__(void *__out_argv)
{
  do_sth();
  XOMP_barrier();
  do_sth();
}
