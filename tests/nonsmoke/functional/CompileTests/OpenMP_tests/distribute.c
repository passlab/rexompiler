#include <stdlib.h> 
#include <omp.h>
int main() {
    const int N = 8;
    int a[N];
    int i;
    #pragma omp teams num_teams(2) thread_limit(N)
    #pragma omp distribute
    for (i=0; i<N; i++) {
        a[i] = omp_get_team_num();
    }
}
