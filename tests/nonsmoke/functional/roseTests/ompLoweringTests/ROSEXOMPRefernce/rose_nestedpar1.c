#include<omp.h>
#include <stdio.h>
#include "libxomp.h" 

void paroutput(char *s)
{
}
static void OUT__1__9957__(void *__out_argv);
static void OUT__2__9957__(void *__out_argv);
static void OUT__3__9957__(void *__out_argv);
static void OUT__4__9957__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
#ifdef _OPENMP
  omp_set_nested(1);
#endif
  XOMP_parallel_start(OUT__4__9957__,0,1,4,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar1.c",14);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar1.c",27);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9957__(void *__out_argv)
{
  printf("after single.\n");
}

static void OUT__2__9957__(void *__out_argv)
{
  printf("Inside single.\n");
}

static void OUT__3__9957__(void *__out_argv)
{
  printf("before single.\n");
}

static void OUT__4__9957__(void *__out_argv)
{
  XOMP_parallel_start(OUT__3__9957__,0,1,4,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar1.c",16);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar1.c",17);
  if (XOMP_single()) {
    XOMP_parallel_start(OUT__2__9957__,0,1,4,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar1.c",22);
    XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar1.c",23);
  }
  XOMP_barrier();
  XOMP_parallel_start(OUT__1__9957__,0,1,4,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar1.c",25);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/nestedpar1.c",26);
}
