#include <stdio.h>
#include <omp.h>

#define NUM_ITERATIONS 10

int main() {
    int num_threads;

    printf("Enter the number of threads: ");
    scanf("%d", &num_threads);

    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        printf("Thread %d:", thread_id);

        int iterations_per_thread = NUM_ITERATIONS / num_threads;
        int start = thread_id * iterations_per_thread;
        int end = (thread_id == num_threads - 1) ? NUM_ITERATIONS : start + iterations_per_thread;

        for (int i = start; i < end; i++) {
            printf("Iteration %d", i);
            if (i != end - 1) printf(", ");
        }

        printf("\n");
    }

    return 0;
}
