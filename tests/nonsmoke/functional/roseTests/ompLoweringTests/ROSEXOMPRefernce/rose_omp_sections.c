#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "libxomp.h" 
void funcA();
void funcB();
static void OUT__1__10241__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__10241__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/omp_sections.c",11);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/omp_sections.c",22);
  XOMP_terminate(status);
  return 0;
}

void funcA()
{
  printf("In funcA: this section is executed by thread %d\n",(omp_get_thread_num()));
}

void funcB()
{
  printf("In funcB: this section is executed by thread %d\n",(omp_get_thread_num()));
}

static void OUT__1__10241__(void *__out_argv)
{
{
    int xomp_section_1 = XOMP_sections_init_next(2);
    while(xomp_section_1 >= 0){
      switch(xomp_section_1){
        case 0:
{
          (void )(funcA());
          break; 
        }
        case 1:
{
          (void )(funcB());
          break; 
        }
        default:
{
          abort();
        }
      }
      xomp_section_1 = XOMP_sections_next();
    }
    XOMP_sections_end();
  }
/*-- End of parallel region --*/
}
