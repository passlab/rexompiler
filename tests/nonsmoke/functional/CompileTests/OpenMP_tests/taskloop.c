#include <omp.h>

void main ( omp_lock_t*lock, int n )
{
int data1 = 10;
int N = 100;
int M = 50;
#pragma omp parallel
{
  #pragma omp single
  #pragma omp taskgroup
  {
        compute_update(data1);

    #pragma omp taskloop collapse(2) nogroup
     for (int i=0; i<N; i++) 
        for (int j=0; j<M; j++)
           data1 = data1 + 1.3;
  }
}
}
