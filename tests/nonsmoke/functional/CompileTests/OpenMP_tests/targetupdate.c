void main(float*p, float*v1, float*v2, int N)
{
    int i;
    #pragma omp target data device(1)
    {
        #pragma omp target
        for (i=0; i<N; i++)
            p[i] = v1[i]*v2[i];
        #pragma omp target update if(target update:1)
        #pragma omp parallel for
        for (i=0; i<N; i++)
          p[i] = p[i] + (v1[i]*v2[i]);
    }
}
