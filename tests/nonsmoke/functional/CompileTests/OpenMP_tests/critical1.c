#include <omp.h>

int main (void)
{
  int a=0,b=0,c =0;
#pragma omp parallel 
  {
#pragma omp critical(aaa) hint(test)
    a=a+1;
#pragma omp critical (bbb), hint(test)
    b=b+1;
#pragma omp critical
    c=c+1;

  }
  return 0;
}
