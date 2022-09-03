#include <stdlib.h> 
#include <assert.h> 
#include <omp.h> 
#include "libxomp.h" 

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int res = 0;
  int n = 10;
#pragma omp teams  num_teams(n) reduction(+ : res)
{
    res = omp_get_team_num();
    if (omp_get_team_num() == 0) 
      n = omp_get_num_teams();
  }
// Sum of first n-1 natural numbers
  Assert(res == n * (n - 1) / 2);
  XOMP_terminate(status);
}
