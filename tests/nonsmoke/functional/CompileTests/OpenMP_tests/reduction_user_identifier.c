
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main()
{
  int a, b, c;

#pragma omp parallel reduction (user_iden:a, b) reduction (user_iden:a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");

    }

#pragma omp parallel reduction (inscan, user_iden:a, b) reduction (user_iden:a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");

    }

  return 0;
}

