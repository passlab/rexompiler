#include "libxomp.h" 
int N = 100;
int b[100];
int c[100];
int j;
static void OUT__1__10015__(void *__out_argv);
static void OUT__2__10015__(void *__out_argv);

void foo()
{
  XOMP_parallel_start(OUT__2__10015__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/twoRegions.c",7);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/twoRegions.c",11);
  XOMP_parallel_start(OUT__1__10015__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/twoRegions.c",12);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/twoRegions.c",16);
}

static void OUT__1__10015__(void *__out_argv)
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
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,N - 1,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    b[p_index_] = _p_c[p_index_];
  }
  XOMP_barrier();
}

static void OUT__2__10015__(void *__out_argv)
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
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,N - 1,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    b[p_index_] = _p_c[p_index_];
  }
  XOMP_barrier();
}
