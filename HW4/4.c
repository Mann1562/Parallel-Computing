#include <stdio.h>
#include <omp.h>

int main() {
    int num_threads = omp_get_max_threads();
    int num_iterations = 10;

    printf("Number of threads: %d\n", num_threads);
    printf("Number of iterations: %d\n", num_iterations);
    printf("Default scheduling: dynamic\n");

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        #pragma omp for
        for (int i = 0; i < num_iterations; i++) {
            printf("Thread %d: Iteration %d\n", thread_id, i);
        }
    }

    return 0;
}
