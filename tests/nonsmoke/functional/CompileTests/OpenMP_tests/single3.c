/*
*/
#include <stdio.h>
#include <omp.h>
#include<assert.h> 

int a;
int b;
int main(void)
{

#pragma omp parallel
  {

#pragma omp single
    {
      int num_threads = 2;
    }
#pragma omp single nowait copyprivate(a,b)
    {
      int num_threads = 3;
    }

  }
  return 0;
}

