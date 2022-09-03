#include<math.h>
#include<string.h>
#define N 16

int main(){
    int r,b,v;
    int a[N], simd_scan[N], scan_a, scan_b;
    for(int i = 0; i < N; i++){
        a[i] = i;
        simd_scan[i] = 0;
    }
    scan_a = 0;
    scan_b = 10;
    #pragma omp simd reduction(inscan, +:scan_a)
    for(int i = 0; i < N; i++){
        simd_scan[i] = scan_a;
        #pragma omp scan exclusive(r,b,v)
	   scan_a += a[i];
           scan_b -= a[i];
    }
    return 0;

}
