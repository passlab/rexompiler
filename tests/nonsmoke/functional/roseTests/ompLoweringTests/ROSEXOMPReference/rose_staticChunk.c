#include <omp.h>
#include <stdio.h>
#include "libxomp.h" 

int main(int argc,char **argv)
{
  int status = 0;
  XOMP_init(argc,argv);
  double a[1000];
  int i;
  int n;
  scanf("%d",&n);
{
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(0,n - 1,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      a[p_index_] = ((double )p_index_) / 2.0;
    }
    XOMP_barrier();
  }
  printf("a[878]=%f\n",a[878]);
  XOMP_terminate(status);
  return 0;
}
