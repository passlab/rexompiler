//Variable examples of using simd directives
void foo (int n, double *a, double* b)
{
#pragma omp simd
  for (int i=0; i<n; i++)
    a[i]=b[i];
}

void foo2 (int n, double *a, double* b)
{
#pragma omp simd safelen(16)
  for (int i=0; i<n; i++)
    a[i]=b[i];
}

void foo3 (int n, double *a, double* b)
{
  int j=0;
#pragma omp simd simdlen(16)
  for (int i=0; i<n; i++,j++)
  {
    a[i]=b[i]+j;
  }
}

void foo32 (int n, double *a, double* b)
{
  int j=0, k=0;
#pragma omp simd linear(j,k)
  for (int i=0; i<n; i++,j++,k++)
  {
    a[i]=b[i]+j+k;
  }
}

void foo33 (int n, double *a, double* b)
{
  int j=0, k=0;
#pragma omp simd linear(j,k:1)
  for (int i=0; i<n; i++,j++,k++)
  {
    a[i]=b[i]+j+k;
  }
}

void fooAligned (int n, double *a, double* b)
{
  int j=0, k=0;
#pragma omp simd aligned(j,k)
  for (int i=0; i<n; i++,j++,k++)
  {
    a[i]=b[i]+j+k;
  }
}


void fooAligned2 (int n, double *a, double* b)
{
  int j=0, k=0;
#pragma omp simd aligned(j,k:1)
  for (int i=0; i<n; i++,j++,k++)
  {
    a[i]=b[i]+j+k;
  }
}

double work( double *a, double *b, int n )
{
   int i; 
   double tmp, sum;
   sum = 0.0;
   #pragma omp simd private(tmp) reduction(+:sum)
   for (i = 0; i < n; i++) {
      tmp = a[i] + b[i];
      sum += tmp;
   }
   return sum;
}


#define N 45
int a[N], b[N], c[N];

void foo4(int i, double* P)
{
  int j; 
#pragma omp simd lastprivate(j)
  for (i = 0; i < 999; ++i) {
    j = P[i];
  }
}

int main() { return 0; }
