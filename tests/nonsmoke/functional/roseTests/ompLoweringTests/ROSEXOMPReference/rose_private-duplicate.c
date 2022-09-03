//A test case for possible duplicated declarations for private variables as loop index
// Coined based on the bug report from Allan Porterfield
#include "libxomp.h" 

struct OUT__1__10705___data 
{
  int (**a_p)[10];
}
;
static void OUT__1__10705__(void *__out_argv);

void foo(int a[10][10])
{
  int i;
  int j;
  struct OUT__1__10705___data __out_argv1__10705__;
  __out_argv1__10705__ . a_p = &a;
  XOMP_parallel_start(OUT__1__10705__,&__out_argv1__10705__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/private-duplicate.c",8);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/private-duplicate.c",12);
}

static void OUT__1__10705__(void *__out_argv)
{
  int (**a)[10] = (int (**)[10])(((struct OUT__1__10705___data *)__out_argv) -> a_p);
  int _p_i;
  int _p_j;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,9,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    for (_p_i = 0; _p_i < 10; _p_i++) 
      ( *a)[_p_i][p_index_] = 0;
  }
  XOMP_barrier();
}
