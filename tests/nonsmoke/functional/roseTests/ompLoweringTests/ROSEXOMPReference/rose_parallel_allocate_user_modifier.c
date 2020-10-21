#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 
static void OUT__1__12203__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int a;
  int b;
  int c;
  XOMP_parallel_start(OUT__1__12203__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_user_modifier.c",10);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_user_modifier.c",14);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__12203__(void *__out_argv)
{
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
}
