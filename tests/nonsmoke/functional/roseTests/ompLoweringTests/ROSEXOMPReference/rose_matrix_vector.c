// double-precision floating-point matrix-vector product
// from BikEfficient2001
// segmentation fault ???
// May 30, 2008
#include <stdio.h>
#define SIZE 5000
#include "libxomp.h" 

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  double d;
  double a[5000][5000];
  double x[5000];
  double y[5000];
  int i;
  int j;
  int n;
  n = 5000;
  for (i = 0; i < n; i++) {
    y[i] = ((double )i) * i;
    for (j = 0; j < n; j++) 
      a[i][j] = ((double )(i / j));
  }
  for (i = 0; i < n; i++) {
    d = 0.0;
    for (j = 0; j < n; j++) {
      d += a[i][j] * y[j];
    }
    x[i] = d;
  }
  printf("%f\n",x[0]);
  XOMP_terminate(status);
  return 0;
}
