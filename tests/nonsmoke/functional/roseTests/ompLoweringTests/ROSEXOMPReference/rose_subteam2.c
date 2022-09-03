/* test two omp for loops in two subteams
 and a single thread in the 3rd subteam
*/
#include <stdio.h>
#include <stdlib.h>
#if defined(_OPENMP)
#include <omp.h>
#endif /* _OPENMP */
/*by Liao, new data types and functions to support thread subteams*/
/*compiler generated new data type to store thread ids in a subteam*/
#include "libxomp.h" 
typedef struct {
int iCount;
int *iThreadIds;}omp_id_set_t;
omp_id_set_t idSet1;
omp_id_set_t idSet2;
omp_id_set_t idSet3;
extern int __ompc_is_in_idset();
extern void __ompc_subteam_create();
/*use it as &threadsubteam*/
void *subteam1;
void *subteam2;
void *subteam3;
#define NUMELEMENT 100

struct OUT__1__9745___data 
{
  int (*a_p)[100];
  void *j_p;
  void *sum_p;
}
;
static void OUT__1__9745__(void *__out_argv);

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  int a[100];
  int i;
  int j = 0;
  int sum = 0;
  int sum2 = 0;
/* assume 5 threads */
#ifdef _OPENMP
  omp_set_num_threads(5);
#endif
/* manual code to generate the thread subteams' ID sets currently */
/*stuff code to get ids from the thread ids in the subteam*/
  idSet1 . iCount = 2;
  idSet1 . iThreadIds = ((int *)(malloc(2 * sizeof(int ))));
  idSet1 . iThreadIds[0] = 1;
  idSet1 . iThreadIds[1] = 3;
  idSet2 . iCount = 2;
  idSet2 . iThreadIds = ((int *)(malloc(2 * sizeof(int ))));
  idSet2 . iThreadIds[0] = 0;
  idSet2 . iThreadIds[1] = 2;
  idSet3 . iCount = 1;
  idSet3 . iThreadIds = ((int *)(malloc(1 * sizeof(int ))));
  idSet3 . iThreadIds[0] = 1;
  struct OUT__1__9745___data __out_argv1__9745__;
  __out_argv1__9745__ . sum_p = ((void *)(&sum));
  __out_argv1__9745__ . j_p = ((void *)(&j));
  __out_argv1__9745__ . a_p = &a;
  XOMP_parallel_start(OUT__1__9745__,&__out_argv1__9745__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/subteam2.c",52);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/subteam2.c",76);
/*------verify results---------------*/
  for (i = 0; i < 100; i++) {
    sum2 = sum2 + a[i];
  }
  printf("sum=%d\n",sum);
  printf("sum2=%d\n",sum2);
  XOMP_terminate(status);
  return 0;
}

static void OUT__1__9745__(void *__out_argv)
{
  int (*a)[100] = (int (*)[100])(((struct OUT__1__9745___data *)__out_argv) -> a_p);
  int *j = (int *)(((struct OUT__1__9745___data *)__out_argv) -> j_p);
  int *sum = (int *)(((struct OUT__1__9745___data *)__out_argv) -> sum_p);
/* onthreads(0,2) */
{
    int _p_i;
    int _p_sum;
    _p_sum = 0;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(1,100,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      _p_sum = _p_sum + p_index_;
    }
    XOMP_atomic_start();
     *sum =  *sum + _p_sum;
    XOMP_atomic_end();
    XOMP_barrier();
  }
/* onthreads(1,3) */
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_dynamic_init(0,99,1,5);
    if (XOMP_loop_dynamic_start(0,99,1,5,&p_lower_,&p_upper_)) {
      do {
        for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
          ( *a)[p_index_] = 9;
        }
      }while (XOMP_loop_dynamic_next(&p_lower_,&p_upper_));
    }
    XOMP_loop_end();
  }
/* onthread 4 */
  if (XOMP_single()) {
#ifdef _OPENMP
     *j = omp_get_thread_num();
#endif
    printf("I am the single one: %d\n", *j);
  }
  XOMP_barrier();
/*end of parallel */
}
