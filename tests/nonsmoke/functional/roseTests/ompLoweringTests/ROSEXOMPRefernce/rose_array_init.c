// Test the handling of two loops under omp for
// watch the loop index replacement (private by default)
#include "libxomp.h" 

struct OUT__1__10016___data 
{
  float (*u_p)[500][500];
}
;
// j is shared, causing data races
static void OUT__1__10016__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int j;
  float u[500][500];
  struct OUT__1__10016___data __out_argv1__10016__;
  __out_argv1__10016__ . u_p = &u;
  XOMP_parallel_start(OUT__1__10016__,&__out_argv1__10016__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/array_init.c",8);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/array_init.c",13);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10016__(void *__out_argv)
{
  float (*u)[500][500] = (float (*)[500][500])(((struct OUT__1__10016___data *)__out_argv) -> u_p);
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
