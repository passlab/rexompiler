static void *xomp_critical_user_;
#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include "libxomp.h" 
static void OUT__1__9934__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__9934__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/taskgroup.c",7);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/taskgroup.c",26);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9934__(void *__out_argv)
{
  if (XOMP_single()) {
#pragma omp taskgroup 
{{
        XOMP_critical_start(&xomp_critical_user_);
        printf("Task 1\n");
        XOMP_critical_end(&xomp_critical_user_);
{
          sleep(1);
          XOMP_critical_start(&xomp_critical_user_);
          printf("Task 2\n");
          XOMP_critical_end(&xomp_critical_user_);
        }
      }
/* end taskgroup */
    }
{
      XOMP_critical_start(&xomp_critical_user_);
      printf("Task 3\n");
      XOMP_critical_end(&xomp_critical_user_);
    }
  }
  XOMP_barrier();
}
