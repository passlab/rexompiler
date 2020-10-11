// A test case based on IS/is.c of npb3.2-omp 
// to test the handling of #if #endif during OpenMP translation
//
// 6/9/2010, Liao
//
#include <stdio.h>
#define NUM_KEYS 1000
#include "libxomp.h" 
int key_array[1000];
int key_buff_ptr_global[1000];

struct OUT__2__10133___data 
{
  void *j_p;
}
;
static void OUT__2__10133__(void *__out_argv);
static void OUT__3__10133__(void *__out_argv);

void full_verify()
{
  int i;
  int j;
  int k;
  int passed_verification = 0;
/*  Now, finally, sort the keys:  */
#ifdef SERIAL_SORT
/*  Copy keys into work array; keys in key_array will be reassigned. */
#ifdef _OPENMP
#endif
/* This is actual sorting */
#else /*SERIAL_SORT*/
/* Memory sorting can be done directly */
#ifdef _OPENMP
  XOMP_parallel_start(OUT__3__10133__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/full_verify.c",38);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/full_verify.c",44);
#endif
#endif /*SERIAL_SORT*/
/*  Confirm keys correctly sorted: count incorrectly sorted keys, if any */
  j = 0;
  struct OUT__2__10133___data __out_argv2__10133__;
  __out_argv2__10133__ . j_p = ((void *)(&j));
#ifdef _OPENMP
  XOMP_parallel_start(OUT__2__10133__,&__out_argv2__10133__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/full_verify.c",51);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/full_verify.c",55);
#endif
  if (j != 0) 
    printf("Full_verify: number of keys out of sort: %d\n",j);
   else 
    passed_verification++;
}
// This function is required to reproduce a bug
static void OUT__1__10133__(void *__out_argv);

void rank()
{
#ifdef _OPENMP
  XOMP_parallel_start(OUT__1__10133__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/full_verify.c",70);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/full_verify.c",75);
#endif
}

static void OUT__1__10133__(void *__out_argv)
{
  printf("nothing here");
}

static void OUT__2__10133__(void *__out_argv)
{
  int *j = (int *)(((struct OUT__2__10133___data *)__out_argv) -> j_p);
  int _p_i;
  int _p_j;
  _p_j = 0;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(1,999,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    if (key_array[p_index_ - 1] > key_array[p_index_]) 
      _p_j++;
  }
  XOMP_atomic_start();
   *j =  *j + _p_j;
  XOMP_atomic_end();
  XOMP_barrier();
}

static void OUT__3__10133__(void *__out_argv)
{
  int _p_i;
  int _p_k;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,999,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_i = (p_index_ == 0?0 : key_buff_ptr_global[p_index_ - 1]);
    while(_p_i < key_buff_ptr_global[p_index_])
      key_array[_p_i++] = p_index_;
  }
  XOMP_barrier();
}
