#include "libxomp.h" 
void do1();
void do2();
void do3();
static void OUT__1__9800__(void *__out_argv);
static void OUT__2__9800__(void *__out_argv);
static void OUT__3__9800__(void *__out_argv);

void foo()
{
  XOMP_parallel_start(OUT__3__9800__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/affinity.c",6);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/affinity.c",9);
  XOMP_parallel_start(OUT__2__9800__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/affinity.c",10);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/affinity.c",13);
  XOMP_parallel_start(OUT__1__9800__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/affinity.c",14);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/affinity.c",17);
}

static void OUT__1__9800__(void *__out_argv)
{
  do1();
}

static void OUT__2__9800__(void *__out_argv)
{
  do1();
}

static void OUT__3__9800__(void *__out_argv)
{
  do1();
}
