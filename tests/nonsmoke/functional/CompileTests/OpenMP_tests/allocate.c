#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
void foo(int iend, int ist)
{
  int i;
#pragma omp parallel
  {
#pragma omp single
    printf ("Using %d threads.\n",omp_get_num_threads());
#pragma omp allocate(i) allocator(pteam_mem_alloc)
    for (i=iend;i>=ist;i--)
    {
      printf("Iteration %d is carried out by thread %d\n",i, omp_get_thread_num());
    }
  }
}

