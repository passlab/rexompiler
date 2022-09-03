#include <omp.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
  #pragma omp parallel
  #pragma omp single
  {
    #pragma omp taskgroup
    {
      {
        #pragma omp critical
        printf ("Task 1\n");
        {
          sleep(1);
          #pragma omp critical
          printf ("Task 2\n");
        }
      }
    } /* end taskgroup */
    {
      #pragma omp critical
      printf ("Task 3\n");
    }
  }
  return 0;
}
