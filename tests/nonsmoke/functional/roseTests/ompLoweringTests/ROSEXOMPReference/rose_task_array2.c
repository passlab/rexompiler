#include <omp.h>
#include "libxomp.h" 
typedef int int_arr[1];

struct OUT__1__10065___data 
{
  int_arr A[1];
}
;
static void OUT__1__10065__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int_arr A[1];
  struct OUT__1__10065___data __out_argv1__10065__;
  int __i2__;
  int __i3__;
  for (__i2__ = 0; __i2__ < 1; __i2__++) 
    for (__i3__ = 0; __i3__ < 1; __i3__++) 
      __out_argv1__10065__ . A[__i2__][__i3__] = A[__i2__][__i3__];
  XOMP_task(OUT__1__10065__,&__out_argv1__10065__,0,sizeof(struct OUT__1__10065___data ),4,1,0);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10065__(void *__out_argv)
{
  int_arr A[1];
  int __i0__;
  int __i1__;
  for (__i0__ = 0; __i0__ < 1; __i0__++) 
    for (__i1__ = 0; __i1__ < 1; __i1__++) 
      A[__i0__][__i1__] = ((struct OUT__1__10065___data *)__out_argv) -> A[__i0__][__i1__];
  int_arr _p_A[1];
{
    int *A_ap = (int *)A;
    int *_p_A_ap = (int *)_p_A;
    int _p_i;
    for (_p_i = 0; _p_i < 1; _p_i++) 
       *(_p_A_ap + _p_i) =  *(A_ap + _p_i);
  }
  _p_A[0][0] = 0;
}
