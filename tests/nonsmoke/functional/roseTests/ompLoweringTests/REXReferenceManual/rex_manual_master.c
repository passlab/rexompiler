/*A nasty example of a preprocessing information within a basic block*/
#include <stdio.h>
#include <omp.h>

void outlined_function(int* global_id, int* bound_id, int* niter, int* nthreads) {
  int global_tid = __kmpc_global_thread_num(NULL);
  int tid = omp_get_thread_num();
  int __niter = *niter;
  int step;
  for (step = 1; step <= __niter; step++) {
      if (step%20 == 0 || step == 1) {
        if (__kmpc_master(NULL, global_tid)) {
          printf(" Time step %4d\n", step);
          __kmpc_end_master(NULL, global_tid);
        }
      }
  }
#if defined(_OPENMP)
        if (__kmpc_master(NULL, global_tid)) {
          nthreads = omp_get_num_threads();
          __kmpc_end_master(NULL, global_tid);
        }
#endif /* _OPENMP */
}

void foo(int step, int niter)
{
  int nthreads;
  int num_threads = 8;
  int current_thread = __kmpc_global_thread_num(NULL);
  __kmpc_push_num_threads(NULL, current_thread, num_threads );  
  __kmpc_fork_call(NULL, 2, outlined_function, &niter, &nthreads);
}
