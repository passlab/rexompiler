// An OpenMP clause keyword: num_threads
//  used as a regular variable in a clause's variable list
//  Extracted from NPB 3.2 benchmarks
//
//  6/10/2010
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__9573___data 
{
  void *num_threads_p;
  void *omp_p;
  void *dynamic_p;
  void *none_p;
}
;
static void OUT__1__9573__(void *__out_argv);

void c_print_results()
{
// none is another OpenMP keyword used with default()
  int num_threads;
  int max_threads;
  int omp = 0;
  int dynamic = 0;
  int none = 0;
  max_threads = 1;
  num_threads = 1;
/*   figure out number of threads used */
#ifdef _OPENMP
  max_threads = omp_get_max_threads();
  struct OUT__1__9573___data __out_argv1__9573__;
  __out_argv1__9573__ . none_p = ((void *)(&none));
  __out_argv1__9573__ . dynamic_p = ((void *)(&dynamic));
  __out_argv1__9573__ . omp_p = ((void *)(&omp));
  __out_argv1__9573__ . num_threads_p = ((void *)(&num_threads));
  XOMP_parallel_start(OUT__1__9573__,&__out_argv1__9573__,1,6,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/shared.c",21);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/shared.c",25);
#endif
}

static void OUT__1__9573__(void *__out_argv)
{
  int *num_threads = (int *)(((struct OUT__1__9573___data *)__out_argv) -> num_threads_p);
  int *omp = (int *)(((struct OUT__1__9573___data *)__out_argv) -> omp_p);
  int *dynamic = (int *)(((struct OUT__1__9573___data *)__out_argv) -> dynamic_p);
  int *none = (int *)(((struct OUT__1__9573___data *)__out_argv) -> none_p);
  if (XOMP_master()) 
     *num_threads = omp_get_num_threads() +  *none +  *omp +  *dynamic;
}
