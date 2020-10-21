#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "libxomp.h" 

struct OUT__1__10881___data 
{
  int array[100];
}
;
static void OUT__1__10881__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int array[100];
  int i;
  for (i = 0; i < 100; i++) 
    array[i] = 10;
  struct OUT__1__10881___data __out_argv1__10881__;
  int __i1__;
  for (__i1__ = 0; __i1__ < 100; __i1__++) 
    __out_argv1__10881__ . array[__i1__] = array[__i1__];
  XOMP_parallel_start(OUT__1__10881__,&__out_argv1__10881__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/firstPrivateArray_2.c",12);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/firstPrivateArray_2.c",17);
  for (i = 0; i < 100; i++) {
    (((void )(sizeof(((array[i] == 10?1 : 0))))) , ((
{
      if (array[i] == 10) 
        ;
       else 
        __assert_fail("array[i]==10","firstPrivateArray_2.c",22,__PRETTY_FUNCTION__);
    })));
  }
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10881__(void *__out_argv)
{
  int array[100];
  int __i0__;
  for (__i0__ = 0; __i0__ < 100; __i0__++) 
    array[__i0__] = ((struct OUT__1__10881___data *)__out_argv) -> array[__i0__];
  int _p_array[100];
{
    int *array_ap = (int *)array;
    int *_p_array_ap = (int *)_p_array;
    int _p_i;
    for (_p_i = 0; _p_i < 100; _p_i++) 
       *(_p_array_ap + _p_i) =  *(array_ap + _p_i);
  }
  int i;
  for (i = 0; i < 100; i++) 
    _p_array[i] += i;
}
