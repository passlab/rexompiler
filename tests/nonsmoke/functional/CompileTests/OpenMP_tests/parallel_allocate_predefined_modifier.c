#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main()
{
  int a, b, c;

#pragma omp parallel allocate (omp_cgroup_mem_alloc:a, b) allocate (a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
    }

#pragma omp parallel allocate (omp_default_mem_alloc:a, b) allocate (a)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
    }

#pragma omp parallel allocate (omp_const_mem_alloc:a, b) allocate (c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
    }

#pragma omp parallel allocate (omp_large_cap_mem_alloc:a, b) allocate (a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
    }

#pragma omp parallel allocate (omp_high_bw_mem_alloc:a, b) allocate (a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
    }

#pragma omp parallel allocate (omp_low_lat_mem_alloc:a, b) allocate (a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
    }

#pragma omp parallel allocate (omp_pteam_mem_alloc:a, b) allocate (a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
    }

#pragma omp parallel allocate (omp_thread_mem_alloc:a, b) allocate (a, c)
    {
        printf("This is for testing parser and AST construction, which could be only syntax correct.\n");
    }

  return 0;
}

