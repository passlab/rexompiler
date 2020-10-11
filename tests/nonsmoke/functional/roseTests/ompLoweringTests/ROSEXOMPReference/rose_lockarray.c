static void *xomp_critical_user_;
/*
test fine grained locks instead of critical section
by Chunhua Liao
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#define LOCKNUM 100
#endif
#define SIZE 5000
#include "libxomp.h" 

struct OUT__1__9910___data 
{
  int (*a_p)[5000];
  void *sum_p;
}
;
static void OUT__1__9910__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int a[5000];
  int i;
  int j;
  int sum;
  int lock_index;
#ifdef _OPENMP
  omp_lock_t lck[100];
  for (i = 0; i < 100; i++) 
    omp_init_lock(&lck[i]);
#endif
  for (i = 0; i < 5000; i++) 
    a[i] = 0;
  struct OUT__1__9910___data __out_argv1__9910__;
  __out_argv1__9910__ . sum_p = ((void *)(&sum));
  __out_argv1__9910__ . a_p = &a;
  XOMP_parallel_start(OUT__1__9910__,&__out_argv1__9910__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lockarray.c",27);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lockarray.c",66);
/* destroy locks*/
#ifdef _OPENMP
  for (i = 0; i < 100; i++) 
    omp_destroy_lock(&lck[i]);
#endif
  printf("sum of a[] = %d\n",sum);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9910__(void *__out_argv)
{
  int (*a)[5000] = (int (*)[5000])(((struct OUT__1__9910___data *)__out_argv) -> a_p);
  int *sum = (int *)(((struct OUT__1__9910___data *)__out_argv) -> sum_p);
  int _p_i;
  int _p_j;
  int _p_lock_index;
/*critical version*/
{
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_dynamic_init(0,4999,1,1);
    if (XOMP_loop_dynamic_start(0,4999,1,1,&p_lower_,&p_upper_)) {
      do {
        for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
          _p_j = p_index_ * p_index_ % 5000;
          XOMP_critical_start(&xomp_critical_user_);
{
            ( *a)[_p_j] = ( *a)[_p_j] + 5;
          }
          XOMP_critical_end(&xomp_critical_user_);
        }
      }while (XOMP_loop_dynamic_next(&p_lower_,&p_upper_));
    }
    XOMP_loop_end();
  }
/* fine grained lock version*/
{
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_dynamic_init(0,4999,1,1);
    if (XOMP_loop_dynamic_start(0,4999,1,1,&p_lower_,&p_upper_)) {
      do {
        for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
          _p_j = p_index_ * p_index_ % 5000;
#ifdef _OPENMP
          _p_lock_index = _p_j % 100;
//    omp_set_lock(lck[lock_index]);
#endif
          ( *a)[_p_j] = ( *a)[_p_j] - 5;
#ifdef _OPENMP
//    omp_unset_lock(lck[lock_index]);
#endif
        }
      }while (XOMP_loop_dynamic_next(&p_lower_,&p_upper_));
    }
    XOMP_loop_end();
  }
/*verify the result*/
   *sum = 0;
{
    int _p_sum;
    _p_sum = 0;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(0,4999,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      _p_sum += ( *a)[p_index_];
    }
    XOMP_atomic_start();
     *sum =  *sum + _p_sum;
    XOMP_atomic_end();
    XOMP_barrier();
  }
}
