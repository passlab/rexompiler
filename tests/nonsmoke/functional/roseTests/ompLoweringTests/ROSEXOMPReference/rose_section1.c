#include <omp.h>
#include <stdio.h>
#include "libxomp.h" 
static void OUT__1__9748__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  XOMP_parallel_start(OUT__1__9748__,0,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/section1.c",5);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/section1.c",24);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9748__(void *__out_argv)
{
  if (XOMP_single()) {
    printf("This is from the single directive\n");
  }
  XOMP_barrier();
{
    int xomp_section_1 = XOMP_sections_init_next(3);
    while(xomp_section_1 >= 0){
      switch(xomp_section_1){
        case 0:
{
          printf("hello from section 1\n");
          break; 
        }
        case 1:
{
          printf("hello from section 2\n");
          break; 
        }
        case 2:
{
          printf("hello from section 3\n");
          break; 
        }
        default:
{
          abort();
        }
      }
      xomp_section_1 = XOMP_sections_next();
    }
    XOMP_sections_end_nowait();
  }
/* end of parallel */
}
