/* Test multiple reduction clauses with different reduction operations
 */
#include <stdio.h>
#include <omp.h>
#define NUM_THREADS 4
#include "libxomp.h" 

struct OUT__1__9965___data 
{
  void *total_p;
  void *yy_p;
  void *zz_p;
  void *res_p;
}
;
static void OUT__1__9965__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int total = 1000000;
  int yy = 10000;
  int zz = 10000;
  double res = 0.0;
  omp_set_num_threads(4);
  struct OUT__1__9965___data __out_argv1__9965__;
  __out_argv1__9965__ . res_p = ((void *)(&res));
  __out_argv1__9965__ . zz_p = ((void *)(&zz));
  __out_argv1__9965__ . yy_p = ((void *)(&yy));
  __out_argv1__9965__ . total_p = ((void *)(&total));
  XOMP_parallel_start(OUT__1__9965__,&__out_argv1__9965__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction2.c",15);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/reduction2.c",21);
  printf("the sum of 1000000 is :%.0f\n",res);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9965__(void *__out_argv)
{
  int *total = (int *)(((struct OUT__1__9965___data *)__out_argv) -> total_p);
  int *yy = (int *)(((struct OUT__1__9965___data *)__out_argv) -> yy_p);
  int *zz = (int *)(((struct OUT__1__9965___data *)__out_argv) -> zz_p);
  double *res = (double *)(((struct OUT__1__9965___data *)__out_argv) -> res_p);
  int _p_i;
  int _p_yy;
  _p_yy = 1;
  int _p_zz;
  _p_zz = 1;
  double _p_res;
  _p_res = 0;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0, *total,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_res = _p_res + p_index_;
    _p_res = _p_res - (2 * p_index_);
    _p_yy *= 1;
    _p_zz *= 1;
  }
  XOMP_atomic_start();
   *yy =  *yy * _p_yy;
  XOMP_atomic_end();
  XOMP_atomic_start();
   *zz =  *zz * _p_zz;
  XOMP_atomic_end();
  XOMP_atomic_start();
   *res =  *res + _p_res;
  XOMP_atomic_end();
  XOMP_barrier();
}
