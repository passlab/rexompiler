#include <stdio.h>
#include <omp.h>
#include "libxomp.h" 
static void OUT__1__9433__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int j;
//  int innerreps = 100;
  XOMP_parallel_start(OUT__1__9433__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/loop1.c",8);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/loop1.c",19);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9433__(void *__out_argv)
{
  int _p_j;
//   for (j=0; j<innerreps; j++)
{
{
      int _p_i;
      long p_index_;
      long p_lower_;
      long p_upper_;
      XOMP_loop_static_init(0,31,1,2);
      if (XOMP_loop_static_start(0,31,1,2,&p_lower_,&p_upper_)) {
        do {
          for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
            printf("thread %d is executing %d \n",(omp_get_thread_num()),p_index_);
//      delay(500);
          }
        }while (XOMP_loop_static_next(&p_lower_,&p_upper_));
      }
      XOMP_loop_end();
    }
  }
}
