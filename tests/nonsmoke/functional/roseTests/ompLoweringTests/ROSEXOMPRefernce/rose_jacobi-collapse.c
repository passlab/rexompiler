// test the collapse clause
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#ifdef _OPENMP
#include <omp.h>
#endif
// Add timing support
#include <sys/time.h>
#include "libxomp.h" 

double time_stamp()
{
  struct timeval t;
  double time;
  gettimeofday(&t,(struct timezone *)((void *)0));
  time = t . tv_sec + 1.0e-6 * t . tv_usec;
  return time;
}
double time1;
double time2;
void driver();
void initialize();
void jacobi();
void error_check();
/************************************************************
* program to solve a finite difference 
* discretization of Helmholtz equation :  
* (d2/dx2)u + (d2/dy2)u - alpha u = f 
* using Jacobi iterative method. 
*
* Modified: Sanjiv Shah,       Kuck and Associates, Inc. (KAI), 1998
* Author:   Joseph Robicheaux, Kuck and Associates, Inc. (KAI), 1998
*
* This c version program is translated by 
* Chunhua Liao, University of Houston, Jan, 2005 
* 
* Directives are used in this code to achieve parallelism. 
* All do loops are parallelized with default 'static' scheduling.
* 
* Input :  n - grid dimension in x direction 
*          m - grid dimension in y direction
*          alpha - Helmholtz constant (always greater than 0.0)
*          tol   - error tolerance for iterative solver
*          relax - Successice over relaxation parameter
*          mits  - Maximum iterations for iterative solver
*
* On output 
*       : u(n,m) - Dependent variable (solutions)
*       : f(n,m) - Right hand side function 
*************************************************************/
#define REAL float // flexible between float and double
#define MSIZE 512
// depending on MSIZE, precision, and number of threads !!
float error_ref = 9.212746E-04;
float resid_ref = 2.356027E-08;
int n;
int m;
int mits;
float tol;
float relax = 1.0;
float alpha = 0.0543;
float u[512][512];
float f[512][512];
float uold[512][512];
float dx;
float dy;
static void OUT__4__10454__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
//  float toler;
/*      printf("Input n,m (< %d) - grid dimension in x,y direction:\n",MSIZE); 
          scanf ("%d",&n);
          scanf ("%d",&m);
          printf("Input tol - error tolerance for iterative solver\n"); 
          scanf("%f",&toler);
          tol=(double)toler;
          printf("Input mits - Maximum iterations for solver\n"); 
          scanf("%d",&mits);
          */
  n = 512;
  m = 512;
  tol = 0.0000000001;
  mits = 5000;
#ifdef _OPENMP
  XOMP_parallel_start(OUT__4__10454__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi-collapse.c",80);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi-collapse.c",84);
#endif
  driver();
  XOMP_terminate(status);
  return 0;
}
/*************************************************************
* Subroutine driver () 
* This is where the arrays are allocated and initialzed. 
*
* Working varaibles/arrays 
*     dx  - grid spacing in x direction 
*     dy  - grid spacing in y direction 
*************************************************************/

void driver()
{
  initialize();
  time1 = time_stamp();
/* Solve Helmholtz equation */
  jacobi();
  time2 = time_stamp();
  printf("------------------------\n");
  printf("Execution time = %f\n",time2 - time1);
/* error_check (n,m,alpha,dx,dy,u,f)*/
  error_check();
}
/*      subroutine initialize (n,m,alpha,dx,dy,u,f) 
******************************************************
* Initializes data 
* Assumes exact solution is u(x,y) = (1-x^2)*(1-y^2)
*
******************************************************/

struct OUT__3__10454___data 
{
  void *__final_total_iters__20___p;
  void *__i_interval__21___p;
}
;
static void OUT__3__10454__(void *__out_argv);

void initialize()
{
  int i;
  int j;
  int xx;
  int yy;
//double PI=3.1415926;
  int __i_total_iters__18__ = (n - 1 - 0 + 1) % 1 == 0?(n - 1 - 0 + 1) / 1 : (n - 1 - 0 + 1) / 1 + 1;
  int __j_total_iters__19__ = (m - 1 - 0 + 1) % 1 == 0?(m - 1 - 0 + 1) / 1 : (m - 1 - 0 + 1) / 1 + 1;
  int __final_total_iters__20__ = 1 * __i_total_iters__18__ * __j_total_iters__19__;
  int __i_interval__21__ = __j_total_iters__19__ * 1;
  int __j_interval__22__ = 1;
  int __collapsed_index__23__;
  dx = (2.0 / (n - 1));
  dy = (2.0 / (m - 1));
  struct OUT__3__10454___data __out_argv3__10454__;
  __out_argv3__10454__ . __i_interval__21___p = ((void *)(&__i_interval__21__));
  __out_argv3__10454__ . __final_total_iters__20___p = ((void *)(&__final_total_iters__20__));
/* Initialize initial condition and RHS */
  XOMP_parallel_start(OUT__3__10454__,&__out_argv3__10454__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi-collapse.c",133);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi-collapse.c",142);
}
/*      subroutine jacobi (n,m,dx,dy,alpha,omega,u,f,tol,maxit)
******************************************************************
* Subroutine HelmholtzJ
* Solves poisson equation on rectangular grid assuming : 
* (1) Uniform discretization in each direction, and 
* (2) Dirichlect boundary conditions 
* 
* Jacobi method is used in this routine 
*
* Input : n,m   Number of grid points in the X/Y directions 
*         dx,dy Grid spacing in the X/Y directions 
*         alpha Helmholtz eqn. coefficient 
*         omega Relaxation factor 
*         f(n,m) Right hand side function 
*         u(n,m) Dependent variable/Solution
*         tol    Tolerance for iterative solver 
*         maxit  Maximum number of iterations 
*
* Output : u(n,m) - Solution 
*****************************************************************/

struct OUT__2__10454___data 
{
  void *omega_p;
  void *error_p;
  void *ax_p;
  void *ay_p;
  void *b_p;
}
;
static void OUT__2__10454__(void *__out_argv);

void jacobi()
{
  float omega;
  int i;
  int j;
  int k;
  float error;
  float resid;
  float ax;
  float ay;
  float b;
//      double  error_local;
//      float ta,tb,tc,td,te,ta1,ta2,tb1,tb2,tc1,tc2,td1,td2;
//      float te1,te2;
//      float second;
  omega = relax;
/*
   * Initialize coefficients */
/* X-direction coef */
  ax = (1.0 / (dx * dx));
/* Y-direction coef */
  ay = (1.0 / (dy * dy));
/* Central coeff */
  b = (- 2.0 / (dx * dx) - 2.0 / (dy * dy) - alpha);
  error = (10.0 * tol);
  k = 1;
  while(k <= mits && error > tol){
    error = 0.0;
    struct OUT__2__10454___data __out_argv2__10454__;
    __out_argv2__10454__ . b_p = ((void *)(&b));
    __out_argv2__10454__ . ay_p = ((void *)(&ay));
    __out_argv2__10454__ . ax_p = ((void *)(&ax));
    __out_argv2__10454__ . error_p = ((void *)(&error));
    __out_argv2__10454__ . omega_p = ((void *)(&omega));
    XOMP_parallel_start(OUT__2__10454__,&__out_argv2__10454__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi-collapse.c",193);
    XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi-collapse.c",211);
/*  omp end parallel */
/* Error check */
    if (k % 500 == 0) 
      printf("Finished %d iteration with error =%f\n",k,error);
    error = (sqrt(error) / (n * m));
    k = k + 1;
/*  End iteration loop */
  }
  printf("Total Number of Iterations:%d\n",k);
  printf("Residual:%E\n",error);
  printf("Residual_ref :%E\n",resid_ref);
  printf("Diff ref=%E\n",(fabs((error - resid_ref))));
  (((void )(sizeof(((fabs((error - resid_ref)) / resid_ref < 1E-4?1 : 0))))) , ((
{
    if (fabs((error - resid_ref)) / resid_ref < 1E-4) 
      ;
     else 
      __assert_fail("fabs(error-resid_ref)/resid_ref < 1E-4","jacobi-collapse.c",227,__PRETTY_FUNCTION__);
  })));
}
/*      subroutine error_check (n,m,alpha,dx,dy,u,f) 
      implicit none 
************************************************************
* Checks error between numerical and exact solution 
*
************************************************************/

struct OUT__1__10454___data 
{
  void *error_p;
  void *__final_total_iters__2___p;
  void *__i_interval__3___p;
}
;
static void OUT__1__10454__(void *__out_argv);

void error_check()
{
  int i;
  int j;
  float xx;
  float yy;
  float temp;
  float error;
  int __i_total_iters__0__ = (n - 1 - 0 + 1) % 1 == 0?(n - 1 - 0 + 1) / 1 : (n - 1 - 0 + 1) / 1 + 1;
  int __j_total_iters__1__ = (m - 1 - 0 + 1) % 1 == 0?(m - 1 - 0 + 1) / 1 : (m - 1 - 0 + 1) / 1 + 1;
  int __final_total_iters__2__ = 1 * __i_total_iters__0__ * __j_total_iters__1__;
  int __i_interval__3__ = __j_total_iters__1__ * 1;
  int __j_interval__4__ = 1;
  int __collapsed_index__5__;
  dx = (2.0 / (n - 1));
  dy = (2.0 / (m - 1));
  error = 0.0;
  struct OUT__1__10454___data __out_argv1__10454__;
  __out_argv1__10454__ . __i_interval__3___p = ((void *)(&__i_interval__3__));
  __out_argv1__10454__ . __final_total_iters__2___p = ((void *)(&__final_total_iters__2__));
  __out_argv1__10454__ . error_p = ((void *)(&error));
  XOMP_parallel_start(OUT__1__10454__,&__out_argv1__10454__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi-collapse.c",246);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi-collapse.c",254);
  error = (sqrt(error) / (n * m));
  printf("Solution Error :%E \n",error);
  printf("Solution Error Ref :%E \n",error_ref);
  printf("Diff ref=%E\n",(fabs((error - error_ref))));
  (((void )(sizeof(((fabs((error - error_ref)) / error_ref < 1E-4?1 : 0))))) , ((
{
    if (fabs((error - error_ref)) / error_ref < 1E-4) 
      ;
     else 
      __assert_fail("fabs(error-error_ref)/error_ref < 1E-4","jacobi-collapse.c",259,__PRETTY_FUNCTION__);
  })));
}

static void OUT__1__10454__(void *__out_argv)
{
  float *error = (float *)(((struct OUT__1__10454___data *)__out_argv) -> error_p);
  int *__final_total_iters__2__ = (int *)(((struct OUT__1__10454___data *)__out_argv) -> __final_total_iters__2___p);
  int *__i_interval__3__ = (int *)(((struct OUT__1__10454___data *)__out_argv) -> __i_interval__3___p);
  int _p___collapsed_index__5__;
  int _p_i;
  int _p_j;
  float _p_xx;
  float _p_yy;
  float _p_temp;
  float _p_error;
  _p_error = 0;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0, *__final_total_iters__2__ - 1,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_i = p_index_ /  *__i_interval__3__ * 1 + 0;
    _p_j = p_index_ %  *__i_interval__3__ * 1 + 0;
    _p_xx = ((float )(-1.00000 + ((double )(dx * ((float )(_p_i - 1))))));
    _p_yy = ((float )(-1.00000 + ((double )(dy * ((float )(_p_j - 1))))));
    _p_temp = ((float )(((double )u[_p_i][_p_j]) - (1.0 - ((double )(_p_xx * _p_xx))) * (1.0 - ((double )(_p_yy * _p_yy)))));
    _p_error = _p_error + _p_temp * _p_temp;
  }
  XOMP_atomic_start();
   *error =  *error + _p_error;
  XOMP_atomic_end();
  XOMP_barrier();
}

static void OUT__2__10454__(void *__out_argv)
{
  float *omega = (float *)(((struct OUT__2__10454___data *)__out_argv) -> omega_p);
  float *error = (float *)(((struct OUT__2__10454___data *)__out_argv) -> error_p);
  float *ax = (float *)(((struct OUT__2__10454___data *)__out_argv) -> ax_p);
  float *ay = (float *)(((struct OUT__2__10454___data *)__out_argv) -> ay_p);
  float *b = (float *)(((struct OUT__2__10454___data *)__out_argv) -> b_p);
  int __i_total_iters__6__ = (n - 1 - 1 - 1 + 1) % 1 == 0?(n - 1 - 1 - 1 + 1) / 1 : (n - 1 - 1 - 1 + 1) / 1 + 1;
  int __j_total_iters__7__ = (m - 1 - 1 - 1 + 1) % 1 == 0?(m - 1 - 1 - 1 + 1) / 1 : (m - 1 - 1 - 1 + 1) / 1 + 1;
  int __final_total_iters__8__ = 1 * __i_total_iters__6__ * __j_total_iters__7__;
  int __i_interval__9__ = __j_total_iters__7__ * 1;
  int __j_interval__10__ = 1;
  int __collapsed_index__11__;
  int __i_total_iters__12__ = (n - 1 - 0 + 1) % 1 == 0?(n - 1 - 0 + 1) / 1 : (n - 1 - 0 + 1) / 1 + 1;
  int __j_total_iters__13__ = (m - 1 - 0 + 1) % 1 == 0?(m - 1 - 0 + 1) / 1 : (m - 1 - 0 + 1) / 1 + 1;
  int __final_total_iters__14__ = 1 * __i_total_iters__12__ * __j_total_iters__13__;
  int __i_interval__15__ = __j_total_iters__13__ * 1;
  int __j_interval__16__ = 1;
  int __collapsed_index__17__;
{
    int _p___collapsed_index__17__;
    int _p_i;
    int _p_j;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(0,__final_total_iters__14__ - 1,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      _p_i = p_index_ / __i_interval__15__ * 1 + 0;
      _p_j = p_index_ % __i_interval__15__ * 1 + 0;
      uold[_p_i][_p_j] = u[_p_i][_p_j];
    }
    XOMP_barrier();
  }
{
    int _p___collapsed_index__11__;
    int _p_i;
    int _p_j;
    float _p_error;
    _p_error = 0;
    float _p_resid;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(0,__final_total_iters__8__ - 1,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      _p_i = p_index_ / __i_interval__9__ * 1 + 1;
      _p_j = p_index_ % __i_interval__9__ * 1 + 1;
      _p_resid = ( *ax * (uold[_p_i - 1][_p_j] + uold[_p_i + 1][_p_j]) +  *ay * (uold[_p_i][_p_j - 1] + uold[_p_i][_p_j + 1]) +  *b * uold[_p_i][_p_j] - f[_p_i][_p_j]) /  *b;
      u[_p_i][_p_j] = uold[_p_i][_p_j] -  *omega * _p_resid;
      _p_error = _p_error + _p_resid * _p_resid;
    }
    XOMP_atomic_start();
     *error =  *error + _p_error;
    XOMP_atomic_end();
  }
}

static void OUT__3__10454__(void *__out_argv)
{
  int *__final_total_iters__20__ = (int *)(((struct OUT__3__10454___data *)__out_argv) -> __final_total_iters__20___p);
  int *__i_interval__21__ = (int *)(((struct OUT__3__10454___data *)__out_argv) -> __i_interval__21___p);
  int _p___collapsed_index__23__;
  int _p_i;
  int _p_j;
  int _p_xx;
  int _p_yy;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0, *__final_total_iters__20__ - 1,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_i = p_index_ /  *__i_interval__21__ * 1 + 0;
    _p_j = p_index_ %  *__i_interval__21__ * 1 + 0;
    _p_xx = ((int )(-1.00000 + ((double )(dx * ((float )(_p_i - 1))))));
    _p_yy = ((int )(-1.00000 + ((double )(dy * ((float )(_p_j - 1))))));
    u[_p_i][_p_j] = ((float )0.0);
    f[_p_i][_p_j] = ((float )(-1.00000 * ((double )alpha) * (1.0 - ((double )(_p_xx * _p_xx))) * (1.0 - ((double )(_p_yy * _p_yy))) - 2.0 * (1.0 - ((double )(_p_xx * _p_xx))) - 2.0 * (1.0 - ((double )(_p_yy * _p_yy)))));
  }
  XOMP_barrier();
}

static void OUT__4__10454__(void *__out_argv)
{
  if (XOMP_single()) {
    printf("Running using %d threads...\n",(omp_get_num_threads()));
  }
  XOMP_barrier();
}
