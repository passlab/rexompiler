
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main()
{
  int a, b, c;

#pragma omp parallel reduction (inscan, +:a, b) reduction (-:a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");

    }

#pragma omp parallel reduction (task, +:a, b) reduction (task, +:a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");

    }

#pragma omp parallel reduction (default, -:a, b) reduction (inscan, -:a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");

    }

  return 0;
}

