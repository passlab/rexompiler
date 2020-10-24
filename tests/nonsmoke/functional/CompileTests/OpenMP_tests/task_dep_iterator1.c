#include <stdio.h>

int main()
{
  int x = 1;
#pragma omp parallel 
#pragma omp single 
{
#pragma omp task  shared(x) depend(iterator ( int a=1:100:2 , b=1:50:6 , double c=1:100 )  , out : x)
    x = 2;
#pragma omp task  shared(x) depend(in : x)
    printf("x = %d\n",x);
  }
  return 0;
}
