int main(int argc, char* argv[])
{
  int i;
  int len = 1000;

  int a[1000];

  for (i=0; i<len; i++)
    a[i]= i;

#pragma omp target if(target:test)
#pragma omp parallel for
  for (i=0;i< len -1 ;i++)
    a[i]=a[i]+1;

  return 0;
}
