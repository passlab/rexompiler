/*
Array typed firstprivate variables:
 element-by-element copy.
Contributed by Pranav Tendulkar
pranav@ics.forth.gr
4/12/2010
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "libxomp.h" 
int array[100];
static void OUT__1__10736__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
// Sara 5/13/2013
// Initialize all positions for run-time check
  int i;
  for (i = 0; i < 100; i++) 
    array[i] = 10;
  XOMP_parallel_start(OUT__1__10736__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/firstPrivateArray.c",24);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/firstPrivateArray.c",29);
// Sara 5/13/2013
// Check result since firsprivate copies were not properly handled
  for (i = 0; i < 100; i++) 
    (((void )(sizeof(((array[i] == 10?1 : 0))))) , ((
{
      if (array[i] == 10) 
        ;
       else 
        __assert_fail("array[i]==10","firstPrivateArray.c",34,__PRETTY_FUNCTION__);
    })));
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10736__(void *__out_argv)
{
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
