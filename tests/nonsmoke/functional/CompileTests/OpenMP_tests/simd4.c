#include <math.h>

void main(int n,int m,float *a,float *b)
{
  int i;
#pragma omp simd if(simd:test) simdlen(8) safelen(8)
{
    for (i = 1; i < n; i++) 
      b[i] = ((a[i] + a[i - 1]) / 2.0);
}
}
