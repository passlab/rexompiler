#include "rex_kmp.h" 
int N = 100;
int b[100];
int c[100];
int j;
static void OUT__1__3845__(int *__global_tid,int *__bound_tid,void *__out_argv);
static void OUT__2__3845__(int *__global_tid,int *__bound_tid,void *__out_argv);

void foo()
{
  __kmpc_fork_call(0,1,OUT__2__3845__,0);
  __kmpc_fork_call(0,1,OUT__1__3845__,0);
}

static void OUT__1__3845__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  int _p_c[100];
{
    int *c_ap = (int *)c;
    int *_p_c_ap = (int *)_p_c;
    int _p_i;
    for (_p_i = 0; _p_i < 100; _p_i++) 
       *(_p_c_ap + _p_i) =  *(c_ap + _p_i);
  }
  int _p_j;
  int __index_;
  int __lower_ = 0;
  int __upper_ = N - 1;
  int __stride_ = 1;
  int __last_iter_ = 0;
  __kmpc_for_static_init_4(0, *__global_tid,34,&__last_iter_,&__lower_,&__upper_,&__stride_,1,1);
  if (__upper_ > N - 1) 
    __upper_ = N - 1;
  for (__index_ = __lower_; __index_ <= __upper_; __index_ += 1) {
    b[__index_] = _p_c[__index_];
  }
  __kmpc_for_static_fini(0, *__global_tid);
  __kmpc_barrier(0, *__global_tid);
}

static void OUT__2__3845__(int *__global_tid,int *__bound_tid,void *__out_argv)
{
  int _p_c[100];
{
    int *c_ap = (int *)c;
    int *_p_c_ap = (int *)_p_c;
    int _p_i;
    for (_p_i = 0; _p_i < 100; _p_i++) 
       *(_p_c_ap + _p_i) =  *(c_ap + _p_i);
  }
  int _p_j;
  int __index_;
  int __lower_ = 0;
  int __upper_ = N - 1;
  int __stride_ = 1;
  int __last_iter_ = 0;
  __kmpc_for_static_init_4(0, *__global_tid,34,&__last_iter_,&__lower_,&__upper_,&__stride_,1,1);
  if (__upper_ > N - 1) 
    __upper_ = N - 1;
  for (__index_ = __lower_; __index_ <= __upper_; __index_ += 1) {
    b[__index_] = _p_c[__index_];
  }
  __kmpc_for_static_fini(0, *__global_tid);
  __kmpc_barrier(0, *__global_tid);
}
