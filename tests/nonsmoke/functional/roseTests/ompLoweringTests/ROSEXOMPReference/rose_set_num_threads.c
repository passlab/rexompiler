static void *xomp_critical_user_;
#include <stdio.h>
#include <assert.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__10547___data 
{
  void *counter_p;
  void *nthreads_p;
}
;
static void OUT__1__10547__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int counter = 0;
  int nthreads;
#ifdef _OPENMP
  omp_set_num_threads(7);
  struct OUT__1__10547___data __out_argv1__10547__;
  __out_argv1__10547__ . nthreads_p = ((void *)(&nthreads));
  __out_argv1__10547__ . counter_p = ((void *)(&counter));
#endif  
  XOMP_parallel_start(OUT__1__10547__,&__out_argv1__10547__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/set_num_threads.c",13);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/set_num_threads.c",18);
  printf("number threads is:%d\n",nthreads);
#ifdef _OPENMP
  (((void )(sizeof(((counter == 7?1 : 0))))) , ((
{
    if (counter == 7) 
      ;
     else 
      __assert_fail("counter == 7","set_num_threads.c",21,__PRETTY_FUNCTION__);
  })));
#else
#endif  
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10547__(void *__out_argv)
{
  int *counter = (int *)(((struct OUT__1__10547___data *)__out_argv) -> counter_p);
  int *nthreads = (int *)(((struct OUT__1__10547___data *)__out_argv) -> nthreads_p);
  XOMP_critical_start(&xomp_critical_user_);
  ( *counter)++;
  XOMP_critical_end(&xomp_critical_user_);
   *nthreads = omp_get_num_threads();
}
