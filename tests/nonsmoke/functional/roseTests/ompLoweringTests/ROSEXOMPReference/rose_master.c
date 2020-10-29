/*A nasty example of a preprocessing information within a basic block*/
#include <stdio.h>
#include <omp.h>
#include "libxomp.h" 

struct OUT__1__9594___data 
{
  int niter;
  void *nthreads_p;
}
;
static void OUT__1__9594__(void *__out_argv);

void foo(int step,int niter)
{
  int nthreads;
  struct OUT__1__9594___data __out_argv1__9594__;
  __out_argv1__9594__ . nthreads_p = ((void *)(&nthreads));
  __out_argv1__9594__ . niter = niter;
  XOMP_parallel_start(OUT__1__9594__,&__out_argv1__9594__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/master.c",8);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/master.c",21);
}

static void OUT__1__9594__(void *__out_argv)
{
  int niter = (int )(((struct OUT__1__9594___data *)__out_argv) -> niter);
  int *nthreads = (int *)(((struct OUT__1__9594___data *)__out_argv) -> nthreads_p);
  int _p_step;
  int _p_niter = niter;
  for (_p_step = 1; _p_step <= _p_niter; _p_step++) {
    if (_p_step % 20 == 0 || _p_step == 1) {
      if (XOMP_master()) 
        printf(" Time step %4d\n",_p_step);
    }
  }
#if defined(_OPENMP)
  if (XOMP_master()) 
     *nthreads = omp_get_num_threads();
#endif /* _OPENMP */
/* end parallel */
}
