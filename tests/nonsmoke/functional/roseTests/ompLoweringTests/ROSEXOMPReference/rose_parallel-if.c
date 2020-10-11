// Test if clause handling
// number of threads should be set to 1 if the if-clause's expression evaluates to be false
#include <assert.h>
#include <stdio.h>
#include <omp.h>
#include "libxomp.h" 
static void OUT__1__10039__(void *__out_argv);
static void OUT__2__10039__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i = 0;
  XOMP_parallel_start(OUT__2__10039__,0,i == 0,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-if.c",10);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-if.c",13);
  XOMP_parallel_start(OUT__1__10039__,0,i != 0,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-if.c",15);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel-if.c",22);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10039__(void *__out_argv)
{
  if (XOMP_single()) {
    (((void )(sizeof(((omp_get_num_threads() == 1?1 : 0))))) , ((
{
      if (omp_get_num_threads() == 1) 
        ;
       else 
        __assert_fail("omp_get_num_threads() == 1","parallel-if.c",19,__PRETTY_FUNCTION__);
    })));
  }
  XOMP_barrier();
  printf("Mutual exclusive output 2.\n");
}

static void OUT__2__10039__(void *__out_argv)
{
  printf("Mutual exclusive output 1.\n");
}
