
int main(int arg, char *argc[]) {
    int a, x;

    #pragma omp parallel reduction(+:a)
	{

    #pragma omp task in_reduction(+:a)
      {
        a = a + x;
      }
    }

    return(0);
}

