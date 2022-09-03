#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main()
{
  int a, b, c;

#pragma omp parallel allocate (user_modi:a, b) allocate (user_modi:a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");

    }

  return 0;
}

