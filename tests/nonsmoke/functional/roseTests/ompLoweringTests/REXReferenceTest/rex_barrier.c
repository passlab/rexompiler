#include<stdio.h> 
#include "rex_kmp.h" 

void do_sth()
{
  printf("hello.\n");
}
static void OUT__1__3515__(int *__global_tid,int *__bound_tid,void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  __kmpc_fork_call(0,1,OUT__1__3515__,0);
  return 0;
}

static void OUT__1__3515__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  do_sth();
  XOMP_barrier();
  do_sth();
}
