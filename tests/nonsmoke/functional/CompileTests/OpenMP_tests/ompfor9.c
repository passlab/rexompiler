#include <math.h>

void main(int n, int m, float *a, float *b, float *y, float *z)
{
  int i;
  int j;
  #pragma omp parallel
  {
    #pragma omp for nowait linear(val(i,j):3) collapse(456) ordered(i)
      for (i=1; i<n; i++)
        b[i] = (a[i] + a[i-1]) / 2.0;
    #pragma omp for nowait ordered
      for (i=0; i<m; i++)
        y[i] = z[i];
  }
}
