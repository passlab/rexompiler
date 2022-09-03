// Test upper case
//  6/11/2010
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__9878___data 
{
  void *NUM_THREADS_p;
}
;
static void OUT__1__9878__(void *__out_argv);

void c_print_results()
{
  int NUM_THREADS;
  int max_threads;
  max_threads = 1;
  NUM_THREADS = 1;
/*   figure out number of threads used */
#ifdef _OPENMP
  max_threads = omp_get_max_threads();
  struct OUT__1__9878___data __out_argv1__9878__;
  __out_argv1__9878__ . NUM_THREADS_p = ((void *)(&NUM_THREADS));
  XOMP_parallel_start(OUT__1__9878__,&__out_argv1__9878__,1,6,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/upperCase.c",18);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/upperCase.c",22);
#endif
}

static void OUT__1__9878__(void *__out_argv)
{
  int *NUM_THREADS = (int *)(((struct OUT__1__9878___data *)__out_argv) -> NUM_THREADS_p);
  if (XOMP_master()) 
     *NUM_THREADS = omp_get_num_threads();
}
