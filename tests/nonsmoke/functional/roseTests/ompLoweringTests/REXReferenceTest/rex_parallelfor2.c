// A tricky case
#include <omp.h>
#include "rex_kmp.h" 

struct OUT__1__3994___data 
{
  int (*a_p)[1000];
}
;
static void OUT__1__3994__(int *__global_tid,int *__bound_tid,void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  int i;
  int a[1000];
  struct OUT__1__3994___data __out_argv1__3994__;
  __out_argv1__3994__ . a_p = &a;
  if (1) 
    __kmpc_fork_call(0,1,OUT__1__3994__,&__out_argv1__3994__);
   else {
    int __global_tid = __kmpc_global_thread_num(0);
    OUT__1__3994__(&__global_tid,0,&__out_argv1__3994__);
  }
  return 0;
}

static void OUT__1__3994__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  int (*a)[1000] = (int (*)[1000])(((struct OUT__1__3994___data *)__out_argv) -> a_p);
  int _p_i;
  int __index_;
  int __lower_ = 0;
  int __upper_ = 999;
  int __stride_ = 1;
  int __last_iter_ = 0;
  __kmpc_for_static_init_4(0, *__global_tid,34,&__last_iter_,&__lower_,&__upper_,&__stride_,1,1);
  if (__upper_ > 999) 
    __upper_ = 999;
  for (__index_ = __lower_; __index_ <= __upper_; __index_ += 1) {
    ( *a)[__index_] = __index_ * 2;
  }
  __kmpc_for_static_fini(0, *__global_tid);
  __kmpc_barrier(0, *__global_tid);
}
