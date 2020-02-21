#include <stdlib.h> 
#include <assert.h> 
#include <omp.h> 
int main() 
{
 int res = 0, n = 10;
 #pragma omp teams num_teams(n) reduction(+:res)
    {
    res = omp_get_team_num();
    if (omp_get_team_num() == 0)
      n = omp_get_num_teams();
    }
 Assert (res == (n*(n-1))/2); 	// Sum of first n-1 natural numbers
}
