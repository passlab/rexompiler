static void *xomp_critical_user_;
#include "libxomp.h" 
int counter = 0;

int foo()
{
  XOMP_critical_start(&xomp_critical_user_);
{
    counter++;
  }
  XOMP_critical_end(&xomp_critical_user_);
  return 0;
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
