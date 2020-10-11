/*
Naive matrix-matrix multiplication(mmm)
By C. Liao
*/
#include <stdio.h>
#include <assert.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#define N 512
#define M 512
#define K 512
#define REAL float 
#include "libxomp.h" 
int i;
int j;
int k;
float a[512][512];
float b[512][512];
float c[512][512];
float c2[512][512];
int init();
int mmm();
int mmm2();
int verify();

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  init();
  mmm();
  mmm2();
  XOMP_terminate(status);
  return verify();
}

int init()
{
  for (i = 0; i < 512; i++) 
    for (j = 0; j < 512; j++) 
      a[i][j] = (3.0 * i * j / 512 / 512);
  for (i = 0; i < 512; i++) 
    for (j = 0; j < 512; j++) 
      b[i][j] = (5.0 * j * i / 512 / 512);
  for (i = 0; i < 512; i++) 
    for (j = 0; j < 512; j++) {
      c[i][j] = 0.0;
      c2[i][j] = 0.0;
    }
  return 0;
}
/*
TODO: try different i,j,k orders
a b     e f    a*e+ b*g , a*f+ b*h
c d  x  g h  = c*e+ d*g,  c*f+ d*h
*/

struct OUT__1__10499___data 
{
  void *__final_total_iters__2___p;
  void *__i_interval__3___p;
}
;
static void OUT__1__10499__(void *__out_argv);

int mmm()
{
  int __i_total_iters__0__ = 512;
  int __j_total_iters__1__ = 512;
  int __final_total_iters__2__ = 1 * __i_total_iters__0__ * __j_total_iters__1__;
  int __i_interval__3__ = __j_total_iters__1__ * 1;
  int __j_interval__4__ = 1;
  int __collapsed_index__5__;
  struct OUT__1__10499___data __out_argv1__10499__;
  __out_argv1__10499__ . __i_interval__3___p = ((void *)(&__i_interval__3__));
  __out_argv1__10499__ . __final_total_iters__2___p = ((void *)(&__final_total_iters__2__));
  XOMP_parallel_start(OUT__1__10499__,&__out_argv1__10499__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/matrixmultiply.c",60);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/matrixmultiply.c",64);
  return 0;
}

int mmm2()
{
  for (i = 0; i < 512; i++) 
    for (j = 0; j < 512; j++) 
      for (k = 0; k < 512; k++) 
        c2[i][j] = c2[i][j] + a[i][k] * b[k][j];
  return 0;
}

int verify()
{
  float sum = 0.0;
  float sum2 = 0.0;
  for (i = 0; i < 512; i++) 
    for (j = 0; j < 512; j++) {
      sum += c[i][j];
      sum2 += c2[i][j];
    }
  printf("sum of c[i][j] is %f\n",sum);
  printf("sum of c2[i][j] is %f\n",sum2);
  (((void )(sizeof(((sum == sum2?1 : 0))))) , ((
{
    if (sum == sum2) 
      ;
     else 
      __assert_fail("sum == sum2","matrixmultiply.c",91,__PRETTY_FUNCTION__);
  })));
  return 0;
}

static void OUT__1__10499__(void *__out_argv)
{
  int *__final_total_iters__2__ = (int *)(((struct OUT__1__10499___data *)__out_argv) -> __final_total_iters__2___p);
  int *__i_interval__3__ = (int *)(((struct OUT__1__10499___data *)__out_argv) -> __i_interval__3___p);
  int _p_i;
  int _p_j;
  int _p_k;
  int _p___collapsed_index__5__;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0, *__final_total_iters__2__ - 1,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_i = p_index_ /  *__i_interval__3__ * 1 + 0;
    _p_j = p_index_ %  *__i_interval__3__ * 1 + 0;
    for (_p_k = 0; _p_k < 512; _p_k++) 
      c[_p_i][_p_j] = c[_p_i][_p_j] + a[_p_i][_p_k] * b[_p_k][_p_j];
  }
  XOMP_barrier();
}
