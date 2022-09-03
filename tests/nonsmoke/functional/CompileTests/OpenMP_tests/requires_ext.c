#include <stdio.h>
#include <stdlib.h>

#define N 100
#pragma omp requires ext_range_bar_only ext_another_warning

void error_handler(int wrong_value, int index) {
    printf(" Error in offload: A[%d]=%d\n", index,wrong_value);
    printf("        Expecting: A[i ]=i\n");
    exit(1);
}
int main() {
    int A[N];
    for (int i=0; i<N; i++) A[i] = i;
    A[N-1]=-1;
    for (int i=0; i<N; i++) {
        if (A[i] != i) {
            error_handler(A[i], i);
        }
    }
    return 0;
}
