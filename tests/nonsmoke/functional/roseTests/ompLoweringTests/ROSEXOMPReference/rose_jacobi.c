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
  gettimeofday(&t,((void *)0));
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
* This c version program is translated by 
* Chunhua Liao, University of Houston, Jan, 2005 
* 
* Directives are used in this code to achieve paralleism. 
* All do loops are parallized with default 'static' scheduling.
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
 #define MSIZE 512
int n;
int m;
int mits;
double tol;
double relax = 1.0;
double alpha = 0.0543;
double u[512][512];
double f[512][512];
double uold[512][512];
double dx;
double dy;
// depending on MSIZE and precision (double vs. float) !!
double error_ref = 9.213041E-04;
double resid_ref = 2.355794E-08;
static void OUT__4__9558__(void *__out_argv);

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
  XOMP_parallel_start(OUT__4__9558__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c",77);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c",81);
#endif
  driver();
  XOMP_terminate(status);
  return 0;
}
/*************************************************************
* Subroutine driver () 
* This is where the arrays are allocated and initialized. 
*
* Working variables/arrays 
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
static void OUT__3__9558__(void *__out_argv);

void initialize()
{
  int i;
  int j;
  int xx;
  int yy;
//double PI=3.1415926;
  dx = 2.0 / (n - 1);
  dy = 2.0 / (m - 1);
/* Initialize initial condition and RHS */
  XOMP_parallel_start(OUT__3__9558__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c",130);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c",139);
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

struct OUT__2__9558___data 
{
  void *omega_p;
  void *error_p;
  void *ax_p;
  void *ay_p;
  void *b_p;
}
;
static void OUT__2__9558__(void *__out_argv);

void jacobi()
{
  double omega;
  int i;
  int j;
  int k;
  double error;
  double resid;
  double ax;
  double ay;
  double b;
//      double  error_local;
//      float ta,tb,tc,td,te,ta1,ta2,tb1,tb2,tc1,tc2,td1,td2;
//      float te1,te2;
//      float second;
  omega = relax;
/*
   * Initialize coefficients */
/* X-direction coef */
  ax = 1.0 / (dx * dx);
/* Y-direction coef */
  ay = 1.0 / (dy * dy);
/* Central coeff */
  b = - 2.0 / (dx * dx) - 2.0 / (dy * dy) - alpha;
  error = 10.0 * tol;
  k = 1;
  while(k <= mits && error > tol){
    error = 0.0;
    struct OUT__2__9558___data __out_argv2__9558__;
    __out_argv2__9558__ . b_p = ((void *)(&b));
    __out_argv2__9558__ . ay_p = ((void *)(&ay));
    __out_argv2__9558__ . ax_p = ((void *)(&ax));
    __out_argv2__9558__ . error_p = ((void *)(&error));
    __out_argv2__9558__ . omega_p = ((void *)(&omega));
/* Copy new solution into old */
    XOMP_parallel_start(OUT__2__9558__,&__out_argv2__9558__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c",191);
    XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c",209);
/*  omp end parallel */
/* Error check */
    k = k + 1;
    if (k % 500 == 0) 
      printf("Finished %d iteration.\n",k);
    error = sqrt(error) / (n * m);
/*  End iteration loop */
  }
  printf("Total Number of Iterations:%d\n",k);
  printf("Residual:%E\n",error);
  printf("Residual_ref :%E\n",resid_ref);
  printf("Diff ref=%E\n",(fabs(error - resid_ref)));
  (((void )(sizeof(((fabs(error - resid_ref) / resid_ref < 1E-5?1 : 0))))) , ((
{
    if (fabs(error - resid_ref) / resid_ref < 1E-5) 
      ;
     else 
      __assert_fail("fabs(error-resid_ref)/resid_ref < 1E-5","jacobi.c",225,__PRETTY_FUNCTION__);
  })));
}
/*      subroutine error_check (n,m,alpha,dx,dy,u,f) 
      implicit none 
************************************************************
* Checks error between numerical and exact solution 
*
************************************************************/

struct OUT__1__9558___data 
{
  void *error_p;
}
;
static void OUT__1__9558__(void *__out_argv);

void error_check()
{
  int i;
  int j;
  double xx;
  double yy;
  double temp;
  double error;
  dx = 2.0 / (n - 1);
  dy = 2.0 / (m - 1);
  error = 0.0;
  struct OUT__1__9558___data __out_argv1__9558__;
  __out_argv1__9558__ . error_p = ((void *)(&error));
  XOMP_parallel_start(OUT__1__9558__,&__out_argv1__9558__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c",242);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c",250);
  error = sqrt(error) / (n * m);
  printf("Solution Error :%E \n",error);
  printf("Solution Error Ref :%E \n",error_ref);
  printf("Diff ref=%E\n",(fabs(error - error_ref)));
  (((void )(sizeof(((fabs(error - error_ref) / error_ref < 1E-5?1 : 0))))) , ((
{
    if (fabs(error - error_ref) / error_ref < 1E-5) 
      ;
     else 
      __assert_fail("fabs(error-error_ref)/error_ref < 1E-5","jacobi.c",255,__PRETTY_FUNCTION__);
  })));
}

static void OUT__1__9558__(void *__out_argv)
{
  double *error = (double *)(((struct OUT__1__9558___data *)__out_argv) -> error_p);
  int _p_i;
  int _p_j;
  double _p_xx;
  double _p_yy;
  double _p_temp;
  double _p_error;
  _p_error = 0;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,n - 1,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    for (_p_j = 0; _p_j < m; _p_j++) {
      _p_xx = - 1.0 + dx * (p_index_ - 1);
      _p_yy = - 1.0 + dy * (_p_j - 1);
      _p_temp = u[p_index_][_p_j] - (1.0 - _p_xx * _p_xx) * (1.0 - _p_yy * _p_yy);
      _p_error = _p_error + _p_temp * _p_temp;
    }
  }
  XOMP_atomic_start();
   *error =  *error + _p_error;
  XOMP_atomic_end();
  XOMP_barrier();
}

static void OUT__2__9558__(void *__out_argv)
{
  double *omega = (double *)(((struct OUT__2__9558___data *)__out_argv) -> omega_p);
  double *error = (double *)(((struct OUT__2__9558___data *)__out_argv) -> error_p);
  double *ax = (double *)(((struct OUT__2__9558___data *)__out_argv) -> ax_p);
  double *ay = (double *)(((struct OUT__2__9558___data *)__out_argv) -> ay_p);
  double *b = (double *)(((struct OUT__2__9558___data *)__out_argv) -> b_p);
{
    int _p_i;
    int _p_j;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(0,n - 1,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      for (_p_j = 0; _p_j < m; _p_j++) 
        uold[p_index_][_p_j] = u[p_index_][_p_j];
    }
    XOMP_barrier();
  }
{
    int _p_i;
    int _p_j;
    double _p_error;
    _p_error = 0;
    double _p_resid;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(1,n - 1 - 1,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      for (_p_j = 1; _p_j < m - 1; _p_j++) {
        _p_resid = ( *ax * (uold[p_index_ - 1][_p_j] + uold[p_index_ + 1][_p_j]) +  *ay * (uold[p_index_][_p_j - 1] + uold[p_index_][_p_j + 1]) +  *b * uold[p_index_][_p_j] - f[p_index_][_p_j]) /  *b;
        u[p_index_][_p_j] = uold[p_index_][_p_j] -  *omega * _p_resid;
        _p_error = _p_error + _p_resid * _p_resid;
      }
    }
    XOMP_atomic_start();
     *error =  *error + _p_error;
    XOMP_atomic_end();
  }
}

static void OUT__3__9558__(void *__out_argv)
{
  int _p_i;
  int _p_j;
  int _p_xx;
  int _p_yy;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,n - 1,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    for (_p_j = 0; _p_j < m; _p_j++) {
      _p_xx = ((int )(- 1.0 + dx * (p_index_ - 1)));
      _p_yy = ((int )(- 1.0 + dy * (_p_j - 1)));
      u[p_index_][_p_j] = 0.0;
      f[p_index_][_p_j] = - 1.0 * alpha * (1.0 - (_p_xx * _p_xx)) * (1.0 - (_p_yy * _p_yy)) - 2.0 * (1.0 - (_p_xx * _p_xx)) - 2.0 * (1.0 - (_p_yy * _p_yy));
    }
  }
  XOMP_barrier();
}

static void OUT__4__9558__(void *__out_argv)
{
  if (XOMP_single()) {
    printf("Running using %d threads...\n",(omp_get_num_threads()));
  }
  XOMP_barrier();
}
