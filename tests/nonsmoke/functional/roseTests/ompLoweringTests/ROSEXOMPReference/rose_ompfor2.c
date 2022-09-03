/*
 loop scheduling
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include "libxomp.h" 
int a[20];

void foo(int lower,int upper,int stride)
{
  int i;
  if (XOMP_single()) {
    printf("---------default schedule--------------\n");
  }
  XOMP_barrier();
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(lower,upper - 1,stride,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += stride) {
      a[p_index_] = p_index_ * 2;
      printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
    }
  }
  XOMP_barrier();
  if (XOMP_single()) {
    printf("---------static schedule--------------\n");
  }
  XOMP_barrier();
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(lower,upper - 1,stride,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += stride) {
      a[p_index_] = p_index_ * 2;
      printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
    }
    XOMP_barrier();
  }
  if (XOMP_single()) {
    printf("---------(static,5) schedule--------------\n");
  }
  XOMP_barrier();
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_static_init(lower,upper - 1,stride,5);
    if (XOMP_loop_static_start(lower,upper - 1,stride,5,&p_lower_,&p_upper_)) {
      do {
        for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += stride) {
          a[p_index_] = p_index_ * 2;
          printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
        }
      }while (XOMP_loop_static_next(&p_lower_,&p_upper_));
    }
    XOMP_loop_end();
  }
  if (XOMP_single()) {
    printf("---------(dynamic,3) schedule--------------\n");
  }
  XOMP_barrier();
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_dynamic_init(lower,upper - 1,stride,3);
    if (XOMP_loop_dynamic_start(lower,upper - 1,stride,3,&p_lower_,&p_upper_)) {
      do {
        for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += stride) {
          a[p_index_] = p_index_ * 2;
          printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
        }
      }while (XOMP_loop_dynamic_next(&p_lower_,&p_upper_));
    }
    XOMP_loop_end();
  }
#if 1
  if (XOMP_single()) {
    printf("---------(guided) schedule--------------\n");
  }
  XOMP_barrier();
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_guided_init(lower,upper - 1,stride,1);
    if (XOMP_loop_guided_start(lower,upper - 1,stride,1,&p_lower_,&p_upper_)) {
      do {
        for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += stride) {
          a[p_index_] = p_index_ * 2;
          printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
        }
      }while (XOMP_loop_guided_next(&p_lower_,&p_upper_));
    }
    XOMP_loop_end();
  }
#endif
  if (XOMP_single()) {
    printf("---------(runtime) ordered schedule--------------\n");
  }
  XOMP_barrier();
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_ordered_runtime_init(lower,upper - 1,stride);
    if (XOMP_loop_ordered_runtime_start(lower,upper - 1,stride,&p_lower_,&p_upper_)) {
      do {
        for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += stride) {
          a[p_index_] = p_index_ * 2;
          printf("Iteration %2d is carried out by thread %2d\n",p_index_,(omp_get_thread_num()));
        }
      }while (XOMP_loop_ordered_runtime_next(&p_lower_,&p_upper_));
    }
    XOMP_loop_end();
  }
}
static void OUT__1__9651__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
//#pragma omp parallel for schedule (auto)
  XOMP_parallel_start(OUT__1__9651__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor2.c",81);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/ompfor2.c",86);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9651__(void *__out_argv)
{
  if (XOMP_single()) {
    printf("Using %d threads.\n",(omp_get_num_threads()));
  }
  XOMP_barrier();
  foo(0,20,3);
}
