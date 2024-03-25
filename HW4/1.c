#include <stdio.h>
#include <omp.h>

int main() {
    #ifdef _OPENMP
    printf("OpenMP support is available.\n");
    printf("OpenMP version: %d\n", _OPENMP);
    #else
    printf("OpenMP support is not available.\n");
    #endif

    return 0;
}
