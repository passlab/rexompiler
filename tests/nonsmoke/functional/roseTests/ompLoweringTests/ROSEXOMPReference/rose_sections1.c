#include <omp.h>
#include <stdio.h>
#include "libxomp.h" 

struct OUT__1__9863___data 
{
  void *section_count_p;
}
;
static void OUT__1__9863__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int section_count = 0;
  struct OUT__1__9863___data __out_argv1__9863__;
  __out_argv1__9863__ . section_count_p = ((void *)(&section_count));
  XOMP_parallel_start(OUT__1__9863__,&__out_argv1__9863__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/sections1.c",6);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/sections1.c",19);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9863__(void *__out_argv)
{
  int *section_count = (int *)(((struct OUT__1__9863___data *)__out_argv) -> section_count_p);
  int _p_section_count =  *section_count;
  int xomp_section_1_save;
  int xomp_section_1 = XOMP_sections_init_next(2);
  while(xomp_section_1 >= 0){
    switch(xomp_section_1){
      case 0:
{
        _p_section_count++;
        printf("section_count %d\n",_p_section_count);
        break; 
      }
      case 1:
{
        _p_section_count++;
        printf("section_count %d\n",_p_section_count);
        break; 
      }
      default:
{
        abort();
      }
    }
    xomp_section_1_save = xomp_section_1;
    xomp_section_1 = XOMP_sections_next();
  }
  if (xomp_section_1_save == 1) 
     *section_count = _p_section_count;
  XOMP_sections_end();
}
