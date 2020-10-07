#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
/*
Example use: 
  printf(" checking error diff ratio \n");
  diff_ratio (error, error_ref, 5); // 6 is better, 7 is very restrictive 
*/
// value, reference value, and the number of significant digits to be ensured.
#include "libxomp.h" 

double diff_ratio(double val,double ref,int significant_digits)
{
  (((void )(sizeof(((significant_digits >= 1?1 : 0))))) , ((
{
    if (significant_digits >= 1) 
      ;
     else 
      __assert_fail("significant_digits>=1","pi.c",15,__PRETTY_FUNCTION__);
  })));
  double diff_ratio = fabs(val - ref) / fabs(ref);
// 1.0/(double(10^significant_digits)) ;
  double upper_limit = pow(0.1,significant_digits);
//  printf("value :%E  ref_value: %E  diff_ratio: %E upper_limit: %E \n",val, ref, diff_ratio, upper_limit);
// ensure the number of the significant digits to be the same 
  if (diff_ratio >= upper_limit) 
    printf("value :%E  ref_value: %E  diff_ratio: %E >= upper_limit: %E \n",val,ref,diff_ratio,upper_limit);
  (((void )(sizeof(((diff_ratio < upper_limit?1 : 0))))) , ((
{
    if (diff_ratio < upper_limit) 
      ;
     else 
      __assert_fail("diff_ratio < upper_limit","pi.c",22,__PRETTY_FUNCTION__);
  })));
  return diff_ratio;
}
int num_steps = 10000;

struct OUT__1__9159___data 
{
  void *sum_p;
  void *step_p;
}
;
static void OUT__1__9159__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  double x = 0;
  double sum = 0.0;
  double pi;
  int i;
  double step = 1.0 / ((double )num_steps);
  struct OUT__1__9159___data __out_argv1__9159__;
  __out_argv1__9159__ . step_p = ((void *)(&step));
  __out_argv1__9159__ . sum_p = ((void *)(&sum));
  XOMP_parallel_start(OUT__1__9159__,&__out_argv1__9159__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/pi.c",34);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/pi.c",39);
  pi = step * sum;
  printf("%f, diff_ratio=%f\n",pi,(diff_ratio(pi,3.141593,6)));
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9159__(void *__out_argv)
{
  double *sum = (double *)(((struct OUT__1__9159___data *)__out_argv) -> sum_p);
  double *step = (double *)(((struct OUT__1__9159___data *)__out_argv) -> step_p);
  double _p_x;
  double _p_sum;
  _p_sum = 0;
  int _p_i;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,num_steps - 1,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ = p_index_ + 1) {
    _p_x = (p_index_ + 0.5) *  *step;
    _p_sum = _p_sum + 4.0 / (1.0 + _p_x * _p_x);
  }
  XOMP_atomic_start();
   *sum =  *sum + _p_sum;
  XOMP_atomic_end();
  XOMP_barrier();
}
