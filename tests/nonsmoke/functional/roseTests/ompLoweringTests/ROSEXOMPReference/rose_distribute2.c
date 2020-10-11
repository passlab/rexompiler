#include <stdlib.h> 
#include <omp.h>
#include "libxomp.h" 

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  const int N = 8;
  int a[8];
  int i;
#pragma omp teams  num_teams(2) thread_limit(N)
#pragma omp distribute  dist_schedule(static , N)
  for (i = 0; i < N; i++) {
    a[i] = omp_get_team_num();
  }
  XOMP_terminate(status);
}
