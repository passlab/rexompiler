#include <stdlib.h> 
#include <assert.h> 
#include <omp.h> 
int main() 
{
 int res = 0, n = 10;
 #pragma omp teams distribute parallel for num_teams(n) reduction(+:res)
    {
    res = 1;
    if (res == 1)
      n = 0;
    }
}
