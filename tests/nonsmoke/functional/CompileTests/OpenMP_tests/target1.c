#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main()
{
  int i;
  long sum=0;
  int total=100;

#pragma omp target in_reduction(+:sum)
  for (i=0; i<= total; i++){
    sum = sum + i;
  }

  long sum0;
#pragma omp parallel private(sum0)
  {
    sum0=0; 

#pragma omp for private(i)
    for (i=0; i<= total; i++)
      sum0=sum0+i;

#pragma omp critical
    sum = sum + sum0; 
  }
  printf("sum of 1 to %d = %ld\n",total,sum);

  return 0;
}

