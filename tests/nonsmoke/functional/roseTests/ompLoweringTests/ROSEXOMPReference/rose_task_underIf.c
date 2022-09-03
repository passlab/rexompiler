/* test pragma under the true body of if statement 
 *  two cases: inside {} or directly attached to true/false body
 *
 *  Liao, 10/1/2008
 * */
#include "libxomp.h" 
extern void process(int );
extern void process2(int );
int item[100];
int cutoff = 100;

struct OUT__1__10189___data 
{
  int i;
}
;
static void OUT__1__10189__(void *__out_argv);

struct OUT__2__10189___data 
{
  int i;
}
;
static void OUT__2__10189__(void *__out_argv);

struct OUT__3__10189___data 
{
  int i;
}
;
static void OUT__3__10189__(void *__out_argv);

struct OUT__4__10189___data 
{
  int i;
}
;
static void OUT__4__10189__(void *__out_argv);

void foo(int i)
{
/*pragma needs scope fixes */
  if (i % 2 == 0) {
    struct OUT__4__10189___data __out_argv4__10189__;
    __out_argv4__10189__ . i = i;
    XOMP_task(OUT__4__10189__,&__out_argv4__10189__,0,sizeof(struct OUT__4__10189___data ),4,i < cutoff,0);
  }
   else {
    struct OUT__3__10189___data __out_argv3__10189__;
    __out_argv3__10189__ . i = i;
    XOMP_task(OUT__3__10189__,&__out_argv3__10189__,0,sizeof(struct OUT__3__10189___data ),4,1,0);
  }
/*pragma within explicit scopes */
  if (i % 2 == 0) {
    struct OUT__2__10189___data __out_argv2__10189__;
    __out_argv2__10189__ . i = i;
    XOMP_task(OUT__2__10189__,&__out_argv2__10189__,0,sizeof(struct OUT__2__10189___data ),4,1,0);
  }
   else {
    struct OUT__1__10189___data __out_argv1__10189__;
    __out_argv1__10189__ . i = i;
    XOMP_task(OUT__1__10189__,&__out_argv1__10189__,0,sizeof(struct OUT__1__10189___data ),4,1,0);
  }
}

static void OUT__1__10189__(void *__out_argv)
{
  int i = (int )(((struct OUT__1__10189___data *)__out_argv) -> i);
  int _p_i = i;
  process2(item[_p_i]);
}

static void OUT__2__10189__(void *__out_argv)
{
  int i = (int )(((struct OUT__2__10189___data *)__out_argv) -> i);
  int _p_i = i;
  process(item[_p_i]);
}

static void OUT__3__10189__(void *__out_argv)
{
  int i = (int )(((struct OUT__3__10189___data *)__out_argv) -> i);
  int _p_i = i;
  process2(item[_p_i]);
}

static void OUT__4__10189__(void *__out_argv)
{
  int i = (int )(((struct OUT__4__10189___data *)__out_argv) -> i);
  int _p_i = i;
  process(item[_p_i]);
}
