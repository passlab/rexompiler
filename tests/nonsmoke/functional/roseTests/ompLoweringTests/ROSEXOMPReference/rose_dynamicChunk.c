#include <stdio.h>
#include <omp.h>
#include "libxomp.h" 

int foo()
{
  double a[1000];
  int i;
  int n;
  scanf("%d",&n);
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_dynamic_init(0,n - 1,1,50);
    if (XOMP_loop_dynamic_start(0,n - 1,1,50,&p_lower_,&p_upper_)) {
      do {
        for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
          a[p_index_] = ((double )p_index_) / 2.0;
        }
      }while (XOMP_loop_dynamic_next(&p_lower_,&p_upper_));
    }
    XOMP_loop_end();
  }
  printf("a[878]=%f\n",a[878]);
  return 0;
}
