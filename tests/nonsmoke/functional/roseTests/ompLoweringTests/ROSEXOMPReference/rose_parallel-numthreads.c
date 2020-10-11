// Test if/num_threads clause handling
#include <assert.h>
#include <stdio.h>
#include <omp.h>
#include "libxomp.h" 
static void OUT__1__10915__(void *__out_argv);
static void OUT__2__10915__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i = 0;
  XOMP_parallel_start(OUT__2__10915__,0,i == 0,3,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-numthreads.c",9);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-numthreads.c",16);
  XOMP_parallel_start(OUT__1__10915__,0,i != 0,3,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-numthreads.c",18);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-numthreads.c",25);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10915__(void *__out_argv)
{
  if (XOMP_single()) {
    (((void )(sizeof(((omp_get_num_threads() == 1?1 : 0))))) , ((
{
      if (omp_get_num_threads() == 1) 
        ;
       else 
        __assert_fail("omp_get_num_threads() == 1","parallel-numthreads.c",22,__PRETTY_FUNCTION__);
    })));
  }
  XOMP_barrier();
  printf("Mutual exclusive output 2.\n");
}

static void OUT__2__10915__(void *__out_argv)
{
  if (XOMP_single()) {
    (((void )(sizeof(((omp_get_num_threads() == 3?1 : 0))))) , ((
{
      if (omp_get_num_threads() == 3) 
        ;
       else 
        __assert_fail("omp_get_num_threads() == 3","parallel-numthreads.c",13,__PRETTY_FUNCTION__);
    })));
  }
  XOMP_barrier();
  printf("Mutual exclusive output 1.\n");
}
