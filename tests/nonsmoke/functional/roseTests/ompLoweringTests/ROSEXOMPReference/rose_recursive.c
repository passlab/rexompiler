static void *xomp_critical_user_;
#include "libxomp.h" 
int counter = 0;

void foo()
{
  XOMP_critical_start(&xomp_critical_user_);
{
    counter++;
    if (counter < 100) 
      foo();
  }
  XOMP_critical_end(&xomp_critical_user_);
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
