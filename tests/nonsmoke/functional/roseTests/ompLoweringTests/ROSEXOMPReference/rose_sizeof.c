// Liao, 11/17/2009
// Test SgSizeOfOp::replace_expression()
// Distilled from spec_omp2001/benchspec/OMPM2001/332.ammp_m/atoms.c
#include "libxomp.h" 

struct OUT__1__9598___data 
{
  void *serial_p;
}
;
static void OUT__1__9598__(void *__out_argv);

int atom()
{
  int serial;
  struct OUT__1__9598___data __out_argv1__9598__;
  __out_argv1__9598__ . serial_p = ((void *)(&serial));
  XOMP_parallel_start(OUT__1__9598__,&__out_argv1__9598__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/sizeof.c",7);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/sizeof.c",11);
  return serial;
}

static void OUT__1__9598__(void *__out_argv)
{
  int *serial = (int *)(((struct OUT__1__9598___data *)__out_argv) -> serial_p);
  int i = (sizeof(( *serial)));
   *serial = i;
}
