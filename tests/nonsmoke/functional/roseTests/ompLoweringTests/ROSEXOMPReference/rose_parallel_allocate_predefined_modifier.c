#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 
static void OUT__1__12802__(void *__out_argv);
static void OUT__2__12802__(void *__out_argv);
static void OUT__3__12802__(void *__out_argv);
static void OUT__4__12802__(void *__out_argv);
static void OUT__5__12802__(void *__out_argv);
static void OUT__6__12802__(void *__out_argv);
static void OUT__7__12802__(void *__out_argv);
static void OUT__8__12802__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int a;
  int b;
  int c;
  XOMP_parallel_start(OUT__8__12802__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",10);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",13);
  XOMP_parallel_start(OUT__7__12802__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",15);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",18);
  XOMP_parallel_start(OUT__6__12802__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",20);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",23);
  XOMP_parallel_start(OUT__5__12802__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",25);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",28);
  XOMP_parallel_start(OUT__4__12802__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",30);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",33);
  XOMP_parallel_start(OUT__3__12802__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",35);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",38);
  XOMP_parallel_start(OUT__2__12802__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",40);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",43);
  XOMP_parallel_start(OUT__1__12802__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",45);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel_allocate_predefined_modifier.c",48);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__12802__(void *__out_argv)
{
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
}

static void OUT__2__12802__(void *__out_argv)
{
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
}

static void OUT__3__12802__(void *__out_argv)
{
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
}

static void OUT__4__12802__(void *__out_argv)
{
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
}

static void OUT__5__12802__(void *__out_argv)
{
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
}

static void OUT__6__12802__(void *__out_argv)
{
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
}

static void OUT__7__12802__(void *__out_argv)
{
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
}

static void OUT__8__12802__(void *__out_argv)
{
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
}
