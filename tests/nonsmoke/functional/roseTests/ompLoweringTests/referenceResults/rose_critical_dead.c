static void *xomp_critical_user_;
#include "libxomp.h" 
int counter = 0;

void foo()
{
  XOMP_critical_start(&xomp_critical_user_);
{
    if (counter < 100) 
      counter++;
//else 
//  return;
//    foo();
  }
  XOMP_critical_end(&xomp_critical_user_);
  foo();
}

int main(argc,argv)
int argc;
char **argv;
{
  int status = 0;
  XOMP_init(argc,argv);
  foo();
  XOMP_terminate(status);
  return 0;
}
