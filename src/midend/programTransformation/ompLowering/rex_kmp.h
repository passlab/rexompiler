
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
