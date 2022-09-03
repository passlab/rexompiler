/*
 * Test for line continuation within pragmas
 *
 * Liao 2/5/2010
 *
 * */
#include "libxomp.h" 

struct OUT__1__9883___data 
{
  int **a_p;
  void *i_p;
}
;
static void OUT__1__9883__(void *__out_argv);

void foo1(int a[])
{
  int i;
  int j;
  struct OUT__1__9883___data __out_argv1__9883__;
  __out_argv1__9883__ . i_p = ((void *)(&i));
  __out_argv1__9883__ . a_p = &a;
  XOMP_parallel_start(OUT__1__9883__,&__out_argv1__9883__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/linebreak.c",11);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/linebreak.c",17);
}

static void OUT__1__9883__(void *__out_argv)
{
  int **a = (int **)(((struct OUT__1__9883___data *)__out_argv) -> a_p);
  int *i = (int *)(((struct OUT__1__9883___data *)__out_argv) -> i_p);
  int _p_j;
  ( *a)[ *i] = _p_j;
}
