//#include <omp.h>
#include "libxomp.h" 
int j = 0;

void foo(int n)
{
  int i;
{
    int _p_j;
    int _p_i;
    long p_index_;
    long p_lower_;
    long p_upper_;
    XOMP_loop_default(1,n - 1,1,&p_lower_,&p_upper_);
    for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
      _p_j = _p_j + 1;
    }
    if (p_index_ != p_lower_ && p_index_ > n - 1) 
      j = _p_j;
    XOMP_barrier();
  }
}
