#include<math.h>
#include<string.h>
#define N 16
#include "libxomp.h" 

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int r;
  int b;
  int v;
  int a[16];
  int simd_scan[16];
  int scan_a;
  int scan_b;
  for (int i = 0; i < 16; i++) {
    a[i] = i;
    simd_scan[i] = 0;
  }
  scan_a = 0;
  scan_b = 10;
#pragma omp simd  reduction(inscan, + : scan_a)
  for (int i = 0; i < 16; i++) {
    simd_scan[i] = scan_a;
#pragma omp scan  exclusive(r,b,v)
    scan_a += a[i];
    scan_b -= a[i];
  }
  XOMP_terminate(status);
  return 0;
}
