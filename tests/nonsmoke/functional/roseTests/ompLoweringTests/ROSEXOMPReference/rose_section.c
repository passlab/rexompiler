#include <omp.h>
#include <assert.h>
#include "libxomp.h" 

struct OUT__1__9699___data 
{
  void *sum_p;
  void *j_p;
  void *k_p;
}
;
static void OUT__1__9699__(void *__out_argv);

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  int sum = 7;
  int known_sum;
  int i;
  int j;
  int k = 111;
  struct OUT__1__9699___data __out_argv1__9699__;
  __out_argv1__9699__ . k_p = ((void *)(&k));
  __out_argv1__9699__ . j_p = ((void *)(&j));
  __out_argv1__9699__ . sum_p = ((void *)(&sum));
  XOMP_parallel_start(OUT__1__9699__,&__out_argv1__9699__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/section.c",10);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/section.c",36);
  known_sum = 999 * 1000 / 2 + 7;
  (((void )(sizeof(((known_sum == sum?1 : 0))))) , ((
{
    if (known_sum == sum) 
      ;
     else 
      __assert_fail("known_sum==sum","section.c",38,__PRETTY_FUNCTION__);
  })));
  (((void )(sizeof(((j == 888?1 : 0))))) , ((
{
    if (j == 888) 
      ;
     else 
      __assert_fail("j == 888","section.c",39,__PRETTY_FUNCTION__);
  })));
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9699__(void *__out_argv)
{
  int *sum = (int *)(((struct OUT__1__9699___data *)__out_argv) -> sum_p);
  int *j = (int *)(((struct OUT__1__9699___data *)__out_argv) -> j_p);
  int *k = (int *)(((struct OUT__1__9699___data *)__out_argv) -> k_p);
{
    int _p_sum;
    _p_sum = 0;
    int _p_i;
    int _p_j;
    int _p_k =  *k;
    int xomp_section_1_save;
    int xomp_section_1 = XOMP_sections_init_next(3);
    while(xomp_section_1 >= 0){
      switch(xomp_section_1){
        case 0:
{
          (((void )(sizeof(((_p_k == 111?1 : 0))))) , ((
{
            if (_p_k == 111) 
              ;
             else 
              __assert_fail("k == 111","section.c",16,__PRETTY_FUNCTION__);
          })));
          for (_p_i = 1; _p_i < 400; _p_i++) {
            _p_sum += _p_i;
          }
          break; 
        }
        case 1:
{
          (((void )(sizeof(((_p_k == 111?1 : 0))))) , ((
{
            if (_p_k == 111) 
              ;
             else 
              __assert_fail("k == 111","section.c",23,__PRETTY_FUNCTION__);
          })));
          for (_p_i = 400; _p_i < 700; _p_i++) 
            _p_sum += _p_i;
          break; 
        }
        case 2:
{
          (((void )(sizeof(((_p_k == 111?1 : 0))))) , ((
{
            if (_p_k == 111) 
              ;
             else 
              __assert_fail("k == 111","section.c",29,__PRETTY_FUNCTION__);
          })));
          _p_j = 888;
          for (_p_i = 700; _p_i < 1000; _p_i++) 
            _p_sum += _p_i;
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
    XOMP_atomic_start();
     *sum =  *sum + _p_sum;
    XOMP_atomic_end();
    if (xomp_section_1_save == 2) 
       *j = _p_j;
    XOMP_sections_end();
  }
/* end of parallel */
}
