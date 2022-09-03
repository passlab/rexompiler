#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "libxomp.h" 

struct OUT__1__10189___data 
{
  void *is_p;
}
;
static void OUT__1__10189__(void *__out_argv);

struct OUT__2__10189___data 
{
  void *is_p;
}
;
static void OUT__2__10189__(void *__out_argv);

struct OUT__3__10189___data 
{
  void *is_p;
}
;
static void OUT__3__10189__(void *__out_argv);
static void OUT__4__10189__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int i;
  int is = 0;
  XOMP_parallel_start(OUT__4__10189__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lastprivate0.c",9);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lastprivate0.c",11);
  printf("%d=%d\n ",i,is);
  is = 0;
  struct OUT__3__10189___data __out_argv3__10189__;
  __out_argv3__10189__ . is_p = ((void *)(&is));
  XOMP_parallel_start(OUT__3__10189__,&__out_argv3__10189__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lastprivate0.c",16);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lastprivate0.c",18);
  printf("%d=%d\n ",i,is);
  is = 0;
  struct OUT__2__10189___data __out_argv2__10189__;
  __out_argv2__10189__ . is_p = ((void *)(&is));
  XOMP_parallel_start(OUT__2__10189__,&__out_argv2__10189__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lastprivate0.c",23);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lastprivate0.c",25);
  printf("%d=%d\n ",i,is);
  is = 0;
  struct OUT__1__10189___data __out_argv1__10189__;
  __out_argv1__10189__ . is_p = ((void *)(&is));
//#pragma omp parallel for lastprivate(is)
  XOMP_parallel_start(OUT__1__10189__,&__out_argv1__10189__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lastprivate0.c",31);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/lastprivate0.c",33);
/*The value of is depends on the num of threads and schedule method*/
  printf("%d, %d\n ",i,is);
  is = 0;
  for (i = 90; i < 100; i++) 
    is = is + i;
  printf("%d, %d\n ",i,is);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__10189__(void *__out_argv)
{
  int *is = (int *)(((struct OUT__1__10189___data *)__out_argv) -> is_p);
  int _p_i;
  int _p_is =  *is;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_static_init(0,99,1,30);
  if (XOMP_loop_static_start(0,99,1,30,&p_lower_,&p_upper_)) {
    do {
      for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
        _p_is = _p_is + p_index_;
      }
    }while (XOMP_loop_static_next(&p_lower_,&p_upper_));
  }
  if (p_index_ != p_lower_ && p_index_ > 99) 
     *is = _p_is;
  XOMP_loop_end();
}

static void OUT__2__10189__(void *__out_argv)
{
  int *is = (int *)(((struct OUT__2__10189___data *)__out_argv) -> is_p);
  int _p_i;
  int _p_is;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,99,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_is = _p_is + p_index_;
  }
  if (p_index_ != p_lower_ && p_index_ > 99) 
     *is = _p_is;
  XOMP_barrier();
}

static void OUT__3__10189__(void *__out_argv)
{
  int *is = (int *)(((struct OUT__3__10189___data *)__out_argv) -> is_p);
  int _p_i;
  int _p_is =  *is;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,99,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_is = _p_is + p_index_;
  }
  XOMP_barrier();
}

static void OUT__4__10189__(void *__out_argv)
{
  int _p_i;
  int _p_is;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0,99,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    _p_is = _p_is + p_index_;
  }
  XOMP_barrier();
}
