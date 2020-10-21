#include<assert.h> 
#include "libxomp.h" 
static void OUT__1__9787__(void *__out_argv);

struct OUT__2__9787___data 
{
  int i;
  void *j_p;
}
;
static void OUT__2__9787__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i = 100;
  int j = 77;
  struct OUT__2__9787___data __out_argv2__9787__;
  __out_argv2__9787__ . j_p = ((void *)(&j));
  __out_argv2__9787__ . i = i;
  XOMP_parallel_start(OUT__2__9787__,&__out_argv2__9787__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel.c",7);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel.c",11);
  XOMP_parallel_start(OUT__1__9787__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel.c",12);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/parallel.c",15);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9787__(void *__out_argv)
{
  int _p_i;
  (((void )(sizeof(((_p_i != 100?1 : 0))))) , ((
{
    if (_p_i != 100) 
      ;
     else 
      __assert_fail("i != 100","parallel.c",14,__PRETTY_FUNCTION__);
  })));
}

static void OUT__2__9787__(void *__out_argv)
{
  int i = (int )(((struct OUT__2__9787___data *)__out_argv) -> i);
  int *j = (int *)(((struct OUT__2__9787___data *)__out_argv) -> j_p);
  int _p_i = i;
  (((void )(sizeof(((_p_i == 100?1 : 0))))) , ((
{
    if (_p_i == 100) 
      ;
     else 
      __assert_fail("i == 100","parallel.c",9,__PRETTY_FUNCTION__);
  })));
  (((void )(sizeof((( *j == 77?1 : 0))))) , ((
{
    if ( *j == 77) 
      ;
     else 
      __assert_fail("j == 77","parallel.c",10,__PRETTY_FUNCTION__);
  })));
}
