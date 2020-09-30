#include<stdio.h> 

#include "omp.h"        /* extern "C" declarations of user-visible routines */

void do_sth()
{
  printf ("hello.\n");  
}

void outlined_function()
{
    int global_tid = __kmpc_global_thread_num(NULL);
    int tid = omp_get_thread_num();
    printf ("hello.\n"); 

    if (__kmpc_barrier(NULL, global_tid)) {
        printf ("hello.\n"); 
    }
    
    printf ("hello.\n"); 
}

int main(int argc, char *argv[])
{
    //int num_threads = 8;
    //int current_thread = __kmpc_global_thread_num(NULL);
    // __kmpc_push_num_threads(NULL, current_thread, num_threads );  
    __kmpc_fork_call(NULL, 0, outlined_function);

    return 0;
}


