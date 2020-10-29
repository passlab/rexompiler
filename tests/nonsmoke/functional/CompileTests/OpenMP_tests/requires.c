#include <stdio.h>
#include <stdlib.h>

#define N 100

#pragma omp requires reverse_offload
int main() {
    int A[N];
    for (int i=0; i<N; i++) A[i] = i;
    A[N-1]=-1;
    return 0;
}
