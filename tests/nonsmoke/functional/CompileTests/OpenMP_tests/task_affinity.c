double * alloc_init_B(double *A, int N);
void compute_on_B(double *B, int N);

void task_affinity(double *A, int N)
{
    double * B;
    #pragma omp task depend(out:B) shared(B) affinity(A)
    {
        B = alloc_init_B(A,N);
    }

    #pragma omp task depend( in:B) shared(B) affinity(A)
    {
        compute_on_B(B,N);
    }
}
