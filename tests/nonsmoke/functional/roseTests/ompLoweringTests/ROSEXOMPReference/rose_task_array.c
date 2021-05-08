#include <omp.h>
#include "libxomp.h" 

struct OUT__1__10015___data 
{
  int A[1];
}
;
static void OUT__1__10015__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int A[1] = {(1)};
  struct OUT__1__10015___data __out_argv1__10015__;
  int __i1__;
  for (__i1__ = 0; __i1__ < 1; __i1__++) 
    __out_argv1__10015__ . A[__i1__] = A[__i1__];
  XOMP_task(OUT__1__10015__,&__out_argv1__10015__,0,sizeof(struct OUT__1__10015___data ),4,1,0);
  XOMP_terminate(status);
  return A[0];
}

static void OUT__1__10015__(void *__out_argv)
{
  int A[1];
  int __i0__;
  for (__i0__ = 0; __i0__ < 1; __i0__++) 
    A[__i0__] = ((struct OUT__1__10015___data *)__out_argv) -> A[__i0__];
  int _p_A[1];
{
    int *A_ap = (int *)A;
    int *_p_A_ap = (int *)_p_A;
    int _p_i;
    for (_p_i = 0; _p_i < 1; _p_i++) 
       *(_p_A_ap + _p_i) =  *(A_ap + _p_i);
  }
  _p_A[0] = 1;
}
