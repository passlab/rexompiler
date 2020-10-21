//from BikEfficient2001
//LU-factorization without pivoting
// outermost loop k is not parallelizable 
// inner loop i is.
//
#include "libxomp.h" 

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int j;
  int k;
  int n = 500;
  double a[n][n];
  for (k = 0; k < n - 1; k++) {
    for (i = k + 1; i < n; i++) {
      a[i][k] = a[i][k] / a[k][k];
      for (j = k + 1; j < n; j++) 
        a[i][j] = a[i][j] - a[i][k] * a[k][j];
    }
  }
  XOMP_terminate(status);
  return 0;
}
