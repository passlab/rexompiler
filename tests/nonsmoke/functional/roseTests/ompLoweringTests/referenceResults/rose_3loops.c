/* 
 *  Only the first level loop index variable should be private
 * */
#include <stdio.h>
#if defined (_OPENMP)
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__9550___data 
{
  double (*a_p)[10][9][8];
}
;
/*  Other loops are not affected by omp directive, so their index variables are still shared!
 *   Affected ==> collapse(n) or ordered (n)
 */
static void OUT__1__9550__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int jj;
  int kkk;
  double a[10][9][8];
  struct OUT__1__9550___data __out_argv1__9550__;
  __out_argv1__9550__ . a_p = &a;
  XOMP_parallel_start(OUT__1__9550__,&__out_argv1__9550__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/3loops.c",14);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/3loops.c",22);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9550__(void *__out_argv)
{
  double (*a)[10][9][8] = (double (*)[10][9][8])(((struct OUT__1__9550___data *)__out_argv) -> a_p);
  int _p_i;
  int _p_jj;
  int _p_kkk;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,9,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    for (_p_jj = 0; _p_jj < 9; _p_jj++) {
      for (_p_kkk = 0; _p_kkk < 8; _p_kkk++) {
        ( *a)[p_index_][_p_jj][_p_kkk] = 9.9;
//      printf("a[%d][%d][%d]=%f ",i,jj,kkk,a[i][jj][kkk]);
      }
    }
  }
  XOMP_barrier();
}
