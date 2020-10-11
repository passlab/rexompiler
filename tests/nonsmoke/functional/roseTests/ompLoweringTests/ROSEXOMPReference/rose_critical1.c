static void *xomp_critical_user_aaa;
static void *xomp_critical_user_bbb;
static void *xomp_critical_user_;
#include <omp.h>
#include "libxomp.h" 

struct OUT__1__9834___data 
{
  void *a_p;
  void *b_p;
  void *c_p;
}
;
static void OUT__1__9834__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int a = 0;
  int b = 0;
  int c = 0;
  struct OUT__1__9834___data __out_argv1__9834__;
  __out_argv1__9834__ . c_p = ((void *)(&c));
  __out_argv1__9834__ . b_p = ((void *)(&b));
  __out_argv1__9834__ . a_p = ((void *)(&a));
  XOMP_parallel_start(OUT__1__9834__,&__out_argv1__9834__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/critical1.c",6);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/critical1.c",15);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9834__(void *__out_argv)
{
  int *a = (int *)(((struct OUT__1__9834___data *)__out_argv) -> a_p);
  int *b = (int *)(((struct OUT__1__9834___data *)__out_argv) -> b_p);
  int *c = (int *)(((struct OUT__1__9834___data *)__out_argv) -> c_p);
  XOMP_critical_start(&xomp_critical_user_aaa);
   *a =  *a + 1;
  XOMP_critical_end(&xomp_critical_user_aaa);
  XOMP_critical_start(&xomp_critical_user_bbb);
   *b =  *b + 1;
  XOMP_critical_end(&xomp_critical_user_bbb);
  XOMP_critical_start(&xomp_critical_user_);
   *c =  *c + 1;
  XOMP_critical_end(&xomp_critical_user_);
}
