#include <omp.h>
#include <stdio.h>
int main( )
 {
    int section_count = 0;
#pragma omp parallel
#pragma omp sections firstprivate( section_count ) lastprivate(section_count)
{
#pragma omp section
    {
        section_count++;
        printf( "section_count %d\n", section_count );
    }
#pragma omp section
    {
        section_count++;
        printf( "section_count %d\n", section_count );
    }
}
    return 0;
}
