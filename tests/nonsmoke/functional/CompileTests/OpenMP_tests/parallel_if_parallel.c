#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main()
{
  int a, b, c;
  a = 0;

#pragma omp parallel if (parallel : a == 0) if (a != 0)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");

    }

  return 0;
}

