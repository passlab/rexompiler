// Test if clause handling
// number of threads should be set to 1 if the if-clause's expression evaluates to be false
// if clause has higher precedence. if it evaluates to 0, num_threads() has no effect.
#include <assert.h>
#include <stdio.h>
#include <omp.h>
#include "libxomp.h" 
static void OUT__1__11167__(void *__out_argv);
static void OUT__2__11167__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i = 0;
  XOMP_parallel_start(OUT__2__11167__,0,i == 0,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-if-numthreads.c",11);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-if-numthreads.c",14);
  XOMP_parallel_start(OUT__1__11167__,0,i != 0,3,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-if-numthreads.c",16);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-if-numthreads.c",23);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__11167__(void *__out_argv)
{
  if (XOMP_single()) {
    (((void )(sizeof(((omp_get_num_threads() == 1?1 : 0))))) , ((
{
      if (omp_get_num_threads() == 1) 
        ;
       else 
        __assert_fail("omp_get_num_threads() == 1","parallel-if-numthreads.c",20,__PRETTY_FUNCTION__);
    })));
  }
  XOMP_barrier();
  printf("Mutual exclusive output 2.\n");
}

static void OUT__2__11167__(void *__out_argv)
{
  printf("Mutual exclusive output 1.\n");
}
