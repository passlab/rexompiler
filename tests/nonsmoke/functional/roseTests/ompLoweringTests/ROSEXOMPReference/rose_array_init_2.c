// Test the handling of two loops under omp for
// watch the loop index replacement (private by default)
// and tje array outlining
#include <stdlib.h>
#include "libxomp.h" 

struct OUT__1__10161___data 
{
  void *u_p;
}
;
// This code has data races since j is shared
static void OUT__1__10161__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int j;
  float **u = (float **)(malloc(500 * sizeof(float *)));
  for (i = 0; i < 500; i++) 
    u[i] = ((float *)(malloc(500 * sizeof(float ))));
  struct OUT__1__10161___data __out_argv1__10161__;
  __out_argv1__10161__ . u_p = ((void *)(&u));
  XOMP_parallel_start(OUT__1__10161__,&__out_argv1__10161__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/array_init_2.c",13);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/array_init_2.c",18);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10161__(void *__out_argv)
{
  float ***u = (float ***)(((struct OUT__1__10161___data *)__out_argv) -> u_p);
  int _p_i;
  int _p_j;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,499,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    for (_p_j = 0; _p_j < 500; _p_j++) {
      ( *u)[p_index_][_p_j] = 0.0;
    }
  }
  XOMP_barrier();
}
