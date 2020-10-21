#include<assert.h> 
#include "rex_kmp.h" 
static void OUT__1__5994__(int *__global_tid,int *__bound_tid,void *__out_argv);

struct OUT__2__5994___data 
{
  int i;
  void *j_p;
}
;
static void OUT__2__5994__(int *__global_tid,int *__bound_tid,void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  int i = 100;
  int j = 77;
  struct OUT__2__5994___data __out_argv2__5994__;
  __out_argv2__5994__ . j_p = ((void *)(&j));
  __out_argv2__5994__ . i = i;
  __kmpc_fork_call(0,1,OUT__2__5994__,&__out_argv2__5994__);
  __kmpc_fork_call(0,1,OUT__1__5994__,0);
  return 0;
}

static void OUT__1__5994__(int *__global_tid,int *__bound_tid,void *__out_argv)
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

static void OUT__2__5994__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  int i = (int )(((struct OUT__2__5994___data *)__out_argv) -> i);
  int *j = (int *)(((struct OUT__2__5994___data *)__out_argv) -> j_p);
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
