#include "omp.h"        /* extern "C" declarations of user-visible routines */
void outlined_function(int* global_id, int* bound_id, int* a, int* b)
{
    int global_tid = __kmpc_global_thread_num(NULL);
    int tid = omp_get_thread_num();

    if (__kmpc_master(NULL, global_tid)) {
        *a=*a+1;
        __kmpc_end_master(NULL, global_tid);
    }


    if (__kmpc_single(NULL, global_tid)) {
        *b=*b+1;
        __kmpc_end_single(NULL, global_tid);
    }
}

int main(int argc, char *argv[])
{
    int num_threads = 8;
    int current_thread = __kmpc_global_thread_num(NULL);
    __kmpc_push_num_threads(NULL, current_thread, num_threads );  
    __kmpc_fork_call(NULL, 2, outlined_function, &a, &b);

    return 0;
}
