
typedef struct ident {
    int reserved_1;
    int flags;
    int reserved_2;
    int reserved_3;
    char const *psource;
} ident_t;

void __kmpc_fork_call(ident_t*, int, void*, ...);
void __kmpc_atomic_start(void);
void __kmpc_atomic_end(void);
void __kmpc_push_num_threads(ident_t*, int, int);
int __kmpc_global_thread_num(ident_t*);
int __kmpc_single(ident_t*, int);
void __kmpc_end_single(ident_t*, int);
void __kmpc_barrier(ident_t*, int);
int __kmpc_serialized_parallel(ident_t*, int);
void __kmpc_end_serialized_parallel(ident_t*, int);
void __kmpc_for_static_init_4(ident_t*, int, int, int*, int*, int*, int*, int, int);
void __kmpc_for_static_fini(ident_t*, int);
void __kmpc_dispatch_init_4(ident_t*, int, int, int, int, int, int);
int __kmpc_dispatch_next_4(ident_t*, int, int*, int*, int*, int*);
