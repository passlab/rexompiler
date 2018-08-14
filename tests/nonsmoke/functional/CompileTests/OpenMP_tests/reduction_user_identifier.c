/*
1. A local copy of reduction variable  is made and initialized depending on the op(e.g. 0 for +).
2. Compiler finds standard reduction expressions containing op and uses them to update the local copy. 
3. Local copies are reduced into a single value and combined with the original global value.

*/
//#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int mymax(int r,int n) {
    int m;
    if (n>r) {
        m = n;
    } else {
        m = r;
    }
    return m;
}


int main()
{
  int i, m;
  long sum=0;
  int total=100;

/*
#pragma omp declare reduction \
    (rwz:int:omp_out=mymax(omp_out,omp_in)) \
    initializer(omp_priv=0)
*/
    m = 0;
#pragma omp parallel reduction(rwz:m)
    {
        m = mymax(m, total);
    }

  return 0;
}

