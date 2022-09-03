#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__10857___data 
{
  void *a_p;
  void *b_p;
  void *c_p;
}
;
static void OUT__1__10857__(void *__out_argv);

struct OUT__2__10857___data 
{
  void *a_p;
  void *b_p;
  void *c_p;
}
;
static void OUT__2__10857__(void *__out_argv);

struct OUT__3__10857___data 
{
  void *a_p;
  void *b_p;
  void *c_p;
}
;
static void OUT__3__10857__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int a;
  int b;
  int c;
  struct OUT__3__10857___data __out_argv3__10857__;
  __out_argv3__10857__ . c_p = ((void *)(&c));
  __out_argv3__10857__ . b_p = ((void *)(&b));
  __out_argv3__10857__ . a_p = ((void *)(&a));
  XOMP_parallel_start(OUT__3__10857__,&__out_argv3__10857__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction_modifier.c",11);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction_modifier.c",15);
  struct OUT__2__10857___data __out_argv2__10857__;
  __out_argv2__10857__ . c_p = ((void *)(&c));
  __out_argv2__10857__ . b_p = ((void *)(&b));
  __out_argv2__10857__ . a_p = ((void *)(&a));
  XOMP_parallel_start(OUT__2__10857__,&__out_argv2__10857__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction_modifier.c",17);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction_modifier.c",21);
  struct OUT__1__10857___data __out_argv1__10857__;
  __out_argv1__10857__ . c_p = ((void *)(&c));
  __out_argv1__10857__ . b_p = ((void *)(&b));
  __out_argv1__10857__ . a_p = ((void *)(&a));
  XOMP_parallel_start(OUT__1__10857__,&__out_argv1__10857__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction_modifier.c",23);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction_modifier.c",27);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10857__(void *__out_argv)
{
  int *a = (int *)(((struct OUT__1__10857___data *)__out_argv) -> a_p);
  int *b = (int *)(((struct OUT__1__10857___data *)__out_argv) -> b_p);
  int *c = (int *)(((struct OUT__1__10857___data *)__out_argv) -> c_p);
  int _p_a;
  _p_a = 0;
  int _p_b;
  _p_b = 0;
  int _p_c;
  _p_c = 0;
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
  XOMP_atomic_start();
   *a =  *a - _p_a;
  XOMP_atomic_end();
  XOMP_atomic_start();
   *b =  *b - _p_b;
  XOMP_atomic_end();
  XOMP_atomic_start();
   *c =  *c - _p_c;
  XOMP_atomic_end();
}

static void OUT__2__10857__(void *__out_argv)
{
  int *a = (int *)(((struct OUT__2__10857___data *)__out_argv) -> a_p);
  int *b = (int *)(((struct OUT__2__10857___data *)__out_argv) -> b_p);
  int *c = (int *)(((struct OUT__2__10857___data *)__out_argv) -> c_p);
  int _p_a;
  _p_a = 0;
  int _p_b;
  _p_b = 0;
  int _p_c;
  _p_c = 0;
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
  XOMP_atomic_start();
   *a =  *a + _p_a;
  XOMP_atomic_end();
  XOMP_atomic_start();
   *b =  *b + _p_b;
  XOMP_atomic_end();
  XOMP_atomic_start();
   *c =  *c + _p_c;
  XOMP_atomic_end();
}

static void OUT__3__10857__(void *__out_argv)
{
  int *a = (int *)(((struct OUT__3__10857___data *)__out_argv) -> a_p);
  int *b = (int *)(((struct OUT__3__10857___data *)__out_argv) -> b_p);
  int *c = (int *)(((struct OUT__3__10857___data *)__out_argv) -> c_p);
  int _p_a;
  _p_a = 0;
  int _p_b;
  _p_b = 0;
  int _p_c;
  _p_c = 0;
  printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
  XOMP_atomic_start();
   *a =  *a + _p_a;
  XOMP_atomic_end();
  XOMP_atomic_start();
   *b =  *b + _p_b;
  XOMP_atomic_end();
  XOMP_atomic_start();
   *c =  *c - _p_c;
  XOMP_atomic_end();
}
